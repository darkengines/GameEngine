#include "MeshSystem.hpp"
#include "MeshInfo.hpp"
#include "Models/Mesh.hpp"
#include "../Graphics/SynchronizationState.hpp"
#include "../Materials/Models/Material.hpp"

namespace drk::Meshes {

	MeshSystem::MeshSystem(
		const Devices::DeviceContext& deviceContext,
		Engine::EngineState& engineState,
		entt::registry& registry
	)
		: DeviceContext(deviceContext), EngineState(engineState), Registry(registry) {}

	void MeshSystem::UpdateStoreItem(const MeshInfo* mesh, Models::Mesh& meshModel) {
		auto materialEntity = mesh->materialEntity;
		auto materialStoreItem = Registry.get<Stores::StoreItem<Materials::Models::Material>>(
			materialEntity
		);

		const auto& frameStoreItem = materialStoreItem.frameStoreItems[EngineState.getFrameIndex()];
		meshModel.materialItemLocation.storeIndex = frameStoreItem.pStore->descriptorArrayElement;
		meshModel.materialItemLocation.itemIndex = frameStoreItem.index;
	}

	void MeshSystem::UploadMeshes() {
		auto meshEntities = Registry.view<MeshInfo*>(entt::exclude<Mesh>);
		std::vector<entt::entity> processedMeshEntities;
		std::vector<Meshes::MeshInfo*> meshes;
		meshEntities.each(
			[&meshes, &processedMeshEntities](const auto meshEntity, const auto& meshInfo) {
				processedMeshEntities.push_back(meshEntity);
				meshes.push_back(meshInfo);
			}
		);
		if (!meshes.empty()) {
			const auto result = EngineState.UploadMeshes(meshes);
			for (auto meshIndex = 0u; meshIndex < processedMeshEntities.size(); meshIndex++) {
				Registry.emplace<Mesh>(processedMeshEntities[meshIndex], result.meshes[meshIndex]);
			}
		}
	}

	void MeshSystem::StoreMeshes() {
		EngineState.Store<Models::Mesh, MeshInfo*>();
	}

	void MeshSystem::UpdateMeshes() {
		Graphics::SynchronizationState<Models::Mesh>::Update<MeshInfo*>(
			Registry,
			EngineState.getFrameIndex(),
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
}
