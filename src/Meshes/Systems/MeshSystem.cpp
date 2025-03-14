#include "MeshSystem.hpp"
#include "../../Animations/Components/SkinnedBufferView.hpp"
#include "../../Cameras/Components/CameraRefrence.hpp"
#include "../../Common/Components/Dirty.hpp"
#include "../../Lights/Components/LightPerspectiveCollection.hpp"
#include "../../Materials/Models/Material.hpp"
#include "../../Materials/Systems/MaterialSystem.hpp"
#include "../../Scenes/Draws/SceneDraw.hpp"
#include "../../Scenes/Draws/ShadowSceneDraw.hpp"
#include "../../Spatials/Components/Spatial.hpp"
#include "../Components/HasShadowDraw.hpp"
#include "../Components/MeshDraw.hpp"
#include "../Models/MeshDraw.hpp"
#include "../Pipelines/MeshPipeline.hpp"

namespace drk::Meshes::Systems {

MeshSystem::MeshSystem(
	const Devices::DeviceContext &deviceContext,
	Engine::EngineState &engineState,
	entt::registry &registry,
	Graphics::GlobalSystem &globalSystem,
	Resources::MeshResourceManager &meshResourceManager
)
	: StorageSystem(engineState, registry),
	  deviceContext(deviceContext),
	  globalSystem(globalSystem),
	  cameraChangedConnection(globalSystem.cameraChanged.connect([&](entt::entity entity) { onCameraChanged(entity); })),
        meshResourceManager(meshResourceManager)
  {
    const auto& meshView = registry.view<Components::MeshResource>(entt::exclude<Components::MeshBufferView>);
  }
  MeshSystem::~MeshSystem() {
	cameraChangedConnection.disconnect();
}
void MeshSystem::onCameraChanged(entt::entity cameraEntity) {
	operations |= MeshSystemOperation::SetCamera;
	registry.view<Components::MeshDraw>().each(
		[this](entt::entity entity, auto &meshDraw) {
			registry.emplace_or_replace<Graphics::SynchronizationState<Scenes::Draws::SceneDraw>>(
				entity,
				engineState.getFrameCount()
			);
		}
	);
}
void MeshSystem::update(
	Models::Mesh &model,
	const Materials::Components::MaterialReference &materialReference
) {
	auto materialStoreItem = registry.get<Stores::StoreItem<Materials::Models::Material>>(materialReference.materialEntity);
	const auto &frameStoreItem = materialStoreItem.frameStoreItems[engineState.getFrameIndex()];
	model.materialItemLocation.storeIndex = frameStoreItem.pStore->descriptorArrayElement;
	model.materialItemLocation.itemIndex = frameStoreItem.index;
}

void MeshSystem::uploadMeshes() {
	auto meshEntities = registry.view<Components::MeshResource>(entt::exclude<Components::MeshBufferView>);
	std::vector<entt::entity> processedMeshEntities;
	std::vector<Meshes::Components::MeshResource *> meshes;
	meshEntities.each(
		[&meshes, &processedMeshEntities](
			const auto meshEntity,
			auto &meshInfo
		) {
			processedMeshEntities.push_back(meshEntity);
			meshes.emplace_back(&meshInfo);
		}
	);
	if (!meshes.empty()) {
		const auto result = meshResourceManager.uploadMeshes(meshes);
		for (auto meshIndex = 0u; meshIndex < processedMeshEntities.size(); meshIndex++) {
			registry.emplace<Components::MeshBufferView>(
				processedMeshEntities[meshIndex],
				result.meshes[meshIndex]
			);
		}
	}
}
void MeshSystem::doOperations(MeshSystemOperation operations) {
	if (operations != MeshSystemOperation::None) {
		auto view = registry.view<Components::MeshDraw>();
		registry.insert<Cameras::Components::CameraReference>(view.begin(), view.end(), {engineState.cameraEntity});
	}
}
void MeshSystem::updateDraw(entt::entity drawEntity, int drawIndex) {
	const auto &[meshDraw, nodeMesh] = registry.get<
		Components::MeshDraw,
		Nodes::Components::NodeMesh>(drawEntity);
	auto &frameState = engineState.getCurrentFrameState();
	//todo: optimization - fetch uniform store in parent scope and give as argument
	auto &drawStore = frameState.getUniformStore<Models::MeshDraw>();
	auto &meshStoreItem = registry.get<Stores::StoreItem<Models::Mesh>>(nodeMesh.meshEntity);
	auto &objectStoreItem = registry.get<Stores::StoreItem<Nodes::Models::Node>>(nodeMesh.nodeEntity);
	auto &cameraStoreItem = registry.get<Stores::StoreItem<Cameras::Models::Camera>>(engineState.cameraEntity);
	const auto &meshItemLocation = drawStore.get(drawIndex);
	auto frameIndex = engineState.getFrameIndex();
	meshItemLocation.pItem->meshItemLocation = meshStoreItem.frameStoreItems[frameIndex];
	meshItemLocation.pItem->objectItemLocation = objectStoreItem.frameStoreItems[frameIndex];
	meshItemLocation.pItem->cameraItemLocation = cameraStoreItem.frameStoreItems[frameIndex];
}
void MeshSystem::emitDraws() {
	const auto &[camera, cameraStoreItem, cameraSpatial] = registry.get<
		Cameras::Components::Camera,
		Stores::StoreItem<Cameras::Models::Camera>,
		Spatials::Components::Spatial<Spatials::Components::Absolute>>(engineState.cameraEntity);
	auto nodeMeshEntities = registry.view<
		Nodes::Components::NodeMesh,
		Meshes::Components::Mesh>(entt::exclude<Components::MeshDraw>);
	const auto &cameraStoreItemLocation = cameraStoreItem.frameStoreItems[engineState.getFrameIndex()];

	nodeMeshEntities.each(
		[&](
			entt::entity objectMeshEntity,
			const Nodes::Components::NodeMesh &nodeMesh,
			const Meshes::Components::Mesh &mesh
		) {
			const auto &[meshResource, meshBufferView, materialReference] = registry.get<
				Meshes::Components::MeshResource,
				Meshes::Components::MeshBufferView,
				Materials::Components::MaterialReference>(nodeMesh.meshEntity);
			const auto &spatial = registry.get<Spatials::Components::Spatial<Spatials::Components::Absolute>>(
				nodeMesh.nodeEntity
			);
			const auto &material = registry.get<Materials::Components::Material>(materialReference.materialEntity);
			auto animationVertexBufferViewPtr = registry.try_get<Animations::Components::SkinnedBufferView>(
				objectMeshEntity
			);
			Devices::BufferView *bufferView = &meshBufferView.VertexBufferView;
			if (animationVertexBufferViewPtr != nullptr) {
				bufferView = &animationVertexBufferViewPtr->frameSkinnedBufferViews[engineState.getFrameIndex()];
			}
			Scenes::Draws::SceneDraw draw = {
				.nodeEntity = nodeMesh.nodeEntity,
				.drawSystem = this,
				.pipelineTypeIndex = std::type_index(typeid(Pipelines::MeshPipeline)),
				.indexBufferView = meshBufferView.IndexBufferView,
				.vertexBufferView = *bufferView,
				.hasTransparency = material.hasTransparency,
				.depth = glm::distance(cameraSpatial.position, spatial.position),
			};
			Components::MeshDraw meshDraw = {
				.indexCount = static_cast<uint32_t>(meshResource.indices.size()),
				.meshBufferView = {
					.IndexBufferView = meshBufferView.IndexBufferView,
					.VertexBufferView = *bufferView,
				}
			};
			auto entity = registry.create();
			registry.emplace_or_replace<Scenes::Draws::SceneDraw>(objectMeshEntity, std::move(draw));
			registry.emplace_or_replace<Components::MeshDraw>(objectMeshEntity, std::move(meshDraw));
			registry.emplace_or_replace<Graphics::SynchronizationState<Scenes::Draws::SceneDraw>>(
				objectMeshEntity,
				engineState.getFrameCount()
			);
		}
	);
}
void MeshSystem::processDirtyDraws() {
	if (registry.any_of<Common::Components::Dirty<Spatials::Components::Spatial<Spatials::Components::Relative>>>(
			engineState.cameraEntity
		)) {
		const auto &[camera, cameraSpatial] = registry.get<Cameras::Components::Camera, Spatials::Components::Spatial<Spatials::Components::Absolute>>(engineState.cameraEntity);
		auto view = registry.view<
			Components::MeshDraw,
			Scenes::Draws::SceneDraw>();
		view.each(
			[&](
				entt::entity objectMeshEntity,
				const Components::MeshDraw &meshDraw,
				Scenes::Draws::SceneDraw &sceneDraw
			) {
				const auto &spatial = registry.get<Spatials::Components::Spatial<Spatials::Components::Absolute>>(
					sceneDraw.nodeEntity
				);
				sceneDraw.depth = glm::distance(cameraSpatial.position, spatial.position);
				registry.emplace_or_replace<Graphics::SynchronizationState<Scenes::Draws::SceneDraw>>(
					objectMeshEntity,
					engineState.getFrameCount()
				);
			}
		);
	}
}
Draws::Components::DrawVertexBufferInfo MeshSystem::getVertexBufferInfo(entt::entity drawEntity) {
	const auto &meshDraw = registry.get<Components::MeshDraw>(drawEntity);
	Draws::Components::DrawVertexBufferInfo bufferInfo{
		meshDraw.indexCount,
		static_cast<uint32_t>(meshDraw.meshBufferView.IndexBufferView.byteOffset / sizeof(VertexIndex)),
		static_cast<int32_t>(meshDraw.meshBufferView.VertexBufferView.byteOffset / sizeof(Vertex))
	};
	return bufferInfo;
}
entt::entity
MeshSystem::copyMeshEntity(const entt::registry &source, entt::registry &destination, entt::entity sourceEntity) {
	const auto &materialReference = source.get<Materials::Components::MaterialReference>(sourceEntity);
	auto meshResource = source.try_get<Components::MeshResource>(sourceEntity);
	auto meshBufferView = source.try_get<Components::MeshBufferView>(sourceEntity);

	auto destinationEntity = destination.create();
	auto detinationMaterialEntity = Materials::Systems::MaterialSystem::copyMaterialEntity(
		source,
		destination,
		materialReference.materialEntity
	);
	destination.emplace<Materials::Components::MaterialReference>(destinationEntity, detinationMaterialEntity);
	if (meshBufferView != nullptr) {
		destination.emplace<Components::MeshBufferView>(destinationEntity, *meshBufferView);
	}
	if (meshResource != nullptr) {
		destination.emplace<Components::MeshResource>(destinationEntity, *meshResource);
	}

	return destinationEntity;
}
} // namespace drk::Meshes::Systems
