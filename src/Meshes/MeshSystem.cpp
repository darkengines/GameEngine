#include "MeshSystem.hpp"
#include "MeshInfo.hpp"
#include "Models/Mesh.hpp"
#include "../Materials/Models/Material.hpp"

namespace drk::Meshes {

	MeshSystem::MeshSystem(
		drk::Devices::DeviceContext *pContext,
		drk::Graphics::EngineState *pState
	)
		: DeviceContext(pContext), EngineState(pState) {}

	void MeshSystem::UpdateStoreItem(const MeshInfo *mesh, Models::Mesh &meshModel) {
		auto materialEntity = mesh->materialEntity;
		auto materialStoreItem = EngineState->Registry.get<Stores::StoreItem<Materials::Models::Material>>(
			materialEntity
		);
		Graphics::Models::StoreItemLocation materialStoreItemLocation = {
			.storeIndex = materialStoreItem.frameStoreItems[EngineState->FrameIndex].pStore->descriptorArrayElement,
			.itemIndex = materialStoreItem.frameStoreItems[EngineState->FrameIndex].index
		};
		meshModel.materialItemLocation = materialStoreItemLocation;
	}

	void MeshSystem::UploadMeshes() {
		auto meshEntities = EngineState->Registry.view<MeshInfo *>(entt::exclude<Mesh>);
		std::vector<entt::entity> processedMeshEntities;
		std::vector<Meshes::MeshInfo *> meshes;
		meshEntities.each(
			[&meshes, &processedMeshEntities](const auto meshEntity, const auto &meshInfo) {
				processedMeshEntities.push_back(meshEntity);
				meshes.push_back(meshInfo);
			}
		);
		const auto result = EngineState->UploadMeshes(meshes);
		for (auto meshIndex = 0u; meshIndex < processedMeshEntities.size(); meshIndex++) {
			EngineState->Registry.emplace<Mesh>(processedMeshEntities[meshIndex], result.meshes[meshIndex]);
		}

		EngineState->Buffers.push_back(result.indexBuffer);
		EngineState->Buffers.push_back(result.vertexBuffer);
	}

	void MeshSystem::StoreMeshes() {
		EngineState->Store<Models::Mesh, MeshInfo *>();
	}

	void MeshSystem::UpdateMeshes() {
		Graphics::SynchronizationState<Models::Mesh>::Update<MeshInfo *>(
			EngineState->Registry,
			EngineState->FrameIndex,
			[=](
				Models::Mesh &model,
				const MeshInfo *component
			) { UpdateStoreItem(component, model); }
		);
	}

	void MeshSystem::AddMeshSystem(entt::registry &registry) {
		registry.on_construct<MeshInfo *>().connect<MeshSystem::OnMeshConstruct>();
	}

	void MeshSystem::RemoveMeshSystem(entt::registry &registry) {
		registry.on_construct<MeshInfo *>().disconnect<MeshSystem::OnMeshConstruct>();
	}

	void MeshSystem::OnMeshConstruct(entt::registry &registry, entt::entity meshEntity) {
		registry.emplace<Graphics::SynchronizationState<Models::Mesh>>(meshEntity, 2u);
	}
}
