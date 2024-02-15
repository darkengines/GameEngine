#include "MeshSystem.hpp"
#include "../../Animations/Components/SkinnedBufferView.hpp"
#include "../../Cameras/Components/CameraRefrence.hpp"
#include "../Components/HasShadowDraw.hpp"
#include "../Components/MeshResource.hpp"
#include "../Models/Mesh.hpp"
#include "../../Graphics/SynchronizationState.hpp"
#include "../../Materials/Models/Material.hpp"
#include "../Models/MeshDraw.hpp"
#include "../Models/ShadowMeshDraw.hpp"
#include "../../Scenes/Draws/SceneDraw.hpp"
#include "../../Scenes/Draws/ShadowSceneDraw.hpp"
#include "../Pipelines/MeshPipeline.hpp"
#include "../Pipelines/ShadowMeshPipeline.hpp"
#include "../Components/MeshDraw.hpp"
#include "../Components/ShadowMeshDraw.hpp"
#include "../../Materials/Systems/MaterialSystem.hpp"
#include <algorithm>
#include "../../Common/Components/Dirty.hpp"
#include "../../Lights/Components/PointLight.hpp"
#include "../../Lights/Components/DirectionalLight.hpp"
#include "../../Lights/Components/Spotlight.hpp"
#include "../../Lights/Models/PointLight.hpp"
#include "../../Lights/Models/DirectionalLight.hpp"
#include "../../Lights/Models/Spotlight.hpp"
#include "../../Lights/Components/LightPerspectiveCollection.hpp"

namespace drk::Meshes::Systems {

	MeshSystem::MeshSystem(
		const Devices::DeviceContext& deviceContext,
		Engine::EngineState& engineState,
		entt::registry& registry,
		Graphics::GlobalSystem& globalSystem,
		Resources::MeshResourceManager& meshResourceManager
	) : System(engineState, registry),
		deviceContext(deviceContext),
		globalSystem(globalSystem),
		cameraChangedConnection(globalSystem.cameraChanged.connect([&](entt::entity entity) { onCameraChanged(entity); })),
		meshResourceManager(meshResourceManager) {}
	MeshSystem::~MeshSystem() {
		cameraChangedConnection.disconnect();
	}
	void MeshSystem::onCameraChanged(entt::entity cameraEntity) {
		operations |= MeshSystemOperation::SetCamera;
		registry.view<Components::MeshDraw>().each(
			[this](entt::entity entity, auto& meshDraw) {
				registry.emplace_or_replace<Graphics::SynchronizationState<Scenes::Draws::SceneDraw>>(
					entity,
					engineState.getFrameCount());
			}
		);
	}
	void MeshSystem::update(
		Models::Mesh& model,
		const Materials::Components::MaterialReference& materialReference
	) {
		auto materialStoreItem = registry.get<Stores::StoreItem<Materials::Models::Material>>(materialReference.materialEntity);
		const auto& frameStoreItem = materialStoreItem.frameStoreItems[engineState.getFrameIndex()];
		model.materialItemLocation.storeIndex = frameStoreItem.pStore->descriptorArrayElement;
		model.materialItemLocation.itemIndex = frameStoreItem.index;
	}

	void MeshSystem::uploadMeshes() {
		auto meshEntities = registry.view<std::shared_ptr<Components::MeshResource>>(entt::exclude<Components::MeshBufferView>);
		std::vector<entt::entity> processedMeshEntities;
		std::vector<std::shared_ptr<Meshes::Components::MeshResource>> meshes;
		meshEntities.each(
			[&meshes, &processedMeshEntities](const auto meshEntity, const auto& meshInfo) {
				processedMeshEntities.push_back(meshEntity);
				meshes.push_back(meshInfo);
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
		const auto& [meshDraw, meshReference, objectReference] = registry.get<
			Components::MeshDraw,
			Components::MeshReference,
			Nodes::Components::ObjectReference
		>(drawEntity);
		auto& frameState = engineState.getCurrentFrameState();
		//todo: optimization - fetch uniform store in parent scope and give as argument
		auto& drawStore = frameState.getUniformStore<Models::MeshDraw>();
		auto& meshStoreItem = registry.get<Stores::StoreItem<Models::Mesh>>(meshReference.meshEntity);
		auto& objectStoreItem = registry.get<Stores::StoreItem<Nodes::Models::Object>>(objectReference.objectEntity);
		auto& cameraStoreItem = registry.get<Stores::StoreItem<Cameras::Models::Camera>>(engineState.cameraEntity);
		const auto& meshItemLocation = drawStore.get(drawIndex);
		auto frameIndex = engineState.getFrameIndex();
		meshItemLocation.pItem->meshItemLocation = meshStoreItem.frameStoreItems[frameIndex];
		meshItemLocation.pItem->objectItemLocation = objectStoreItem.frameStoreItems[frameIndex];
		meshItemLocation.pItem->cameraItemLocation = cameraStoreItem.frameStoreItems[frameIndex];
	}
	void MeshSystem::emitDraws() {
		const auto& [camera, cameraStoreItem] = registry.get<
			Cameras::Components::Camera,
			Stores::StoreItem<Cameras::Models::Camera>
		>(engineState.cameraEntity);
		auto objectMeshEntities = registry.view<
			Nodes::Components::ObjectReference,
			Meshes::Components::MeshReference,
			Meshes::Components::Mesh
		>(entt::exclude<Components::MeshDraw>);
		const auto& cameraStoreItemLocation = cameraStoreItem.frameStoreItems[engineState.getFrameIndex()];

		objectMeshEntities.each(
			[&](
				entt::entity objectMeshEntity,
				const Nodes::Components::ObjectReference& objectReference,
				const Meshes::Components::MeshReference& meshReference,
				const Meshes::Components::Mesh& mesh
			) {
				const auto& [meshResource, meshBufferView, materialReference] = registry.get<
					std::shared_ptr<Meshes::Components::MeshResource>,
					Meshes::Components::MeshBufferView,
					Materials::Components::MaterialReference
				>(meshReference.meshEntity);
				const auto& spatial = registry.get<Spatials::Components::Spatial<Spatials::Components::Absolute>>(
					objectReference.objectEntity
				);
				auto& material = registry.get<std::shared_ptr<Materials::Components::Material>>(materialReference.materialEntity);
				auto animationVertexBufferViewPtr = registry.try_get<Animations::Components::SkinnedBufferView>(
					objectMeshEntity
				);
				Devices::BufferView* bufferView = &meshBufferView.VertexBufferView;
				if (animationVertexBufferViewPtr != nullptr) {
					bufferView = &animationVertexBufferViewPtr->frameSkinnedBufferViews[engineState.getFrameIndex()];
				}
				Scenes::Draws::SceneDraw draw = {
					.drawSystem = this,
					.pipelineTypeIndex = std::type_index(typeid(Pipelines::MeshPipeline)),
					.indexBufferView = meshBufferView.IndexBufferView,
					.vertexBufferView = *bufferView,
					.hasTransparency = material->hasTransparency,
					.depth = glm::distance(camera.absolutePosition, spatial.position),
				};
				Components::MeshDraw meshDraw = {
					.meshResource = meshResource,
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
					engineState.getFrameCount());
			}
		);
	}
	void MeshSystem::processDirtyDraws() {
		if (registry.any_of<Common::Components::Dirty<Spatials::Components::Spatial<Spatials::Components::Relative>>>(
			engineState.cameraEntity
		)) {
			auto camera = registry.get<Cameras::Components::Camera>(engineState.cameraEntity);
			auto view = registry.view<
				Nodes::Components::ObjectReference,
				Components::MeshDraw,
				Scenes::Draws::SceneDraw
			>();
			view.each(
				[&](
					entt::entity objectMeshEntity,
					const Nodes::Components::ObjectReference& objectReference,
					const Components::MeshDraw& meshDraw,
					Scenes::Draws::SceneDraw& sceneDraw
				) {
					const auto& spatial = registry.get<Spatials::Components::Spatial<Spatials::Components::Absolute>>(
						objectReference.objectEntity
					);
					sceneDraw.depth = glm::distance(camera.absolutePosition, spatial.position);
					registry.emplace_or_replace<Graphics::SynchronizationState<Scenes::Draws::SceneDraw>>(
						objectMeshEntity,
						engineState.getFrameCount());
				}
			);
		}
	}
	Draws::Components::DrawVertexBufferInfo MeshSystem::getVertexBufferInfo(entt::entity drawEntity) {
		const auto& meshDraw = registry.get<Components::MeshDraw>(drawEntity);
		Draws::Components::DrawVertexBufferInfo bufferInfo{
			static_cast<uint32_t>(meshDraw.meshResource->indices.size()),
			static_cast<uint32_t>(meshDraw.meshBufferView.IndexBufferView.byteOffset / sizeof(VertexIndex)),
			static_cast<int32_t>(meshDraw.meshBufferView.VertexBufferView.byteOffset / sizeof(Vertex))
		};
		return bufferInfo;
	}
	entt::entity
	MeshSystem::copyMeshEntity(const entt::registry& source, entt::registry& destination, entt::entity sourceEntity) {
		const auto& materialReference = source.get<Materials::Components::MaterialReference>(sourceEntity);
		auto meshResource = source.try_get<std::shared_ptr<Components::MeshResource>>(sourceEntity);
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
			destination.emplace<std::shared_ptr<Components::MeshResource>>(destinationEntity, *meshResource);
		}

		return destinationEntity;
	}
}
