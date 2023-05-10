#include "MeshSystem.hpp"
#include "MeshInfo.hpp"
#include "Models/Mesh.hpp"
#include "../Graphics/SynchronizationState.hpp"
#include "../Materials/Models/Material.hpp"
#include "../Objects/Models/Object.hpp"
#include "MeshGroup.hpp"
#include "../Spatials/Components/Spatial.hpp"
#include "../Cameras/Camera.hpp"
#include "Models/MeshDraw.hpp"
#include "../Scenes/Draws/SceneDraw.hpp"
#include "../Meshes/Pipelines/MeshPipeline.hpp"
#include "Components/MeshDraw.hpp"

namespace drk::Meshes {

	MeshSystem::MeshSystem(
		const Devices::DeviceContext& deviceContext,
		Engine::EngineState& engineState,
		entt::registry& registry
	)
		: deviceContext(deviceContext), engineState(engineState), registry(registry) {}

	void MeshSystem::UpdateStoreItem(const MeshInfo* mesh, Models::Mesh& meshModel) {
		auto materialEntity = mesh->materialEntity;
		auto materialStoreItem = registry.get<Stores::StoreItem<Materials::Models::Material>>(
			materialEntity
		);

		const auto& frameStoreItem = materialStoreItem.frameStoreItems[engineState.getFrameIndex()];
		meshModel.materialItemLocation.storeIndex = frameStoreItem.pStore->descriptorArrayElement;
		meshModel.materialItemLocation.itemIndex = frameStoreItem.index;
	}

	void MeshSystem::UploadMeshes() {
		auto meshEntities = registry.view<MeshInfo*>(entt::exclude<Mesh>);
		std::vector<entt::entity> processedMeshEntities;
		std::vector<Meshes::MeshInfo*> meshes;
		meshEntities.each(
			[&meshes, &processedMeshEntities](const auto meshEntity, const auto& meshInfo) {
				processedMeshEntities.push_back(meshEntity);
				meshes.push_back(meshInfo);
			}
		);
		if (!meshes.empty()) {
			const auto result = engineState.UploadMeshes(meshes);
			for (auto meshIndex = 0u; meshIndex < processedMeshEntities.size(); meshIndex++) {
				registry.emplace<Mesh>(processedMeshEntities[meshIndex], result.meshes[meshIndex]);
			}
		}
	}

	void MeshSystem::StoreMeshes() {
		engineState.Store<Models::Mesh, MeshInfo*>();
	}

	void MeshSystem::UpdateMeshes() {
		Graphics::SynchronizationState<Models::Mesh>::Update<MeshInfo*>(
			registry,
			engineState.getFrameIndex(),
			std::function < void(Models::Mesh & , MeshInfo * const&)>(
			[&](
				Models::Mesh& model,
				MeshInfo* const& component
			) { UpdateStoreItem(component, model); }
		)
		);
	}

	void MeshSystem::AddMeshSystem(entt::registry& registry) {
		registry.on_construct<MeshInfo*>().connect<MeshSystem::OnMeshConstruct>();
	}

	void MeshSystem::RemoveMeshSystem(entt::registry& registry) {
		registry.on_construct<MeshInfo*>().disconnect<MeshSystem::OnMeshConstruct>();
	}

	void MeshSystem::OnMeshConstruct(entt::registry& registry, entt::entity meshEntity) {

	}
	void MeshSystem::UpdateDraw(entt::entity drawEntity, int drawIndex) {
		const auto& meshDraw = registry.get<Components::MeshDraw>(drawEntity);
		auto& frameState = engineState.getCurrentFrameState();
		//todo: optimization - fetch uniform store in parent scope and give as argument
		auto& drawStore = frameState.getUniformStore<Models::MeshDraw>();
		const auto& meshItemLocation = drawStore.get(drawIndex);
		meshItemLocation.pItem->meshItemLocation.storeIndex = meshDraw.meshItemLocation.storeIndex;
		meshItemLocation.pItem->meshItemLocation.itemIndex = meshDraw.meshItemLocation.itemIndex;
		meshItemLocation.pItem->objectItemLocation.storeIndex = meshDraw.objectItemLocation.storeIndex;
		meshItemLocation.pItem->objectItemLocation.itemIndex = meshDraw.objectItemLocation.itemIndex;
	}
	void MeshSystem::EmitDraws() {
		auto objectEntities = registry.view<Stores::StoreItem<Objects::Models::Object>, Meshes::MeshGroup, Spatials::Components::Spatial>();
		auto cameraEntity = engineState.CameraEntity;
		auto camera = registry.get<Cameras::Camera>(cameraEntity);
		objectEntities.each(
			[&](
				entt::entity objectEntity,
				auto& objectStoreItem,
				auto& meshGroup,
				auto& spatial
			) {
				const auto& objectStoreItemLocation = objectStoreItem.frameStoreItems[engineState.getFrameIndex()];
				for (const auto& meshEntity: meshGroup.meshEntities) {
					Meshes::MeshInfo* meshInfo = registry.get<Meshes::MeshInfo*>(meshEntity);
					const Meshes::Mesh& mesh = registry.get<Meshes::Mesh>(meshEntity);
					const Stores::StoreItem<Meshes::Models::Mesh> meshStoreItem = registry.get<Stores::StoreItem<Meshes::Models::Mesh>>(
						meshEntity
					);
					const auto& meshStoreItemLocation = meshStoreItem.frameStoreItems[engineState.getFrameIndex()];
					Scenes::Draws::SceneDraw draw = {
						.drawSystem = this,
						.indexBufferView = mesh.IndexBufferView,
						.vertexBufferView = mesh.VertexBufferView,
						.hasTransparency = meshInfo->pMaterial->hasTransparency,
						.depth = glm::distance(camera.absolutePosition, spatial.absolutePosition),
					};
					Components::MeshDraw meshDraw = {
						.meshInfo = meshInfo,
						.mesh = mesh,
						.meshItemLocation = meshStoreItemLocation,
						.objectItemLocation = objectStoreItemLocation
					};
					auto entity = registry.create();
					registry.emplace_or_replace<Scenes::Draws::SceneDraw>(entity, std::move(draw));
					registry.emplace_or_replace<Components::MeshDraw>(entity, std::move(meshDraw));
				}
			}
		);
	}
	Draws::DrawVertexBufferInfo MeshSystem::GetVertexBufferInfo(entt::entity drawEntity) {
		auto meshDraw = registry.get<Components::MeshDraw>(drawEntity);
		Draws::DrawVertexBufferInfo bufferInfo{
			static_cast<uint32_t>(meshDraw.meshInfo->indices.size()),
			static_cast<uint32_t>(meshDraw.mesh.IndexBufferView.byteOffset / sizeof(VertexIndex)),
			static_cast<int32_t>(meshDraw.mesh.VertexBufferView.byteOffset / sizeof(Vertex))
		};
		return bufferInfo;
	}
}
