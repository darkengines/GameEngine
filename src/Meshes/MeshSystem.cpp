#include "MeshSystem.hpp"
#include "MeshInfo.hpp"
#include "Models/Mesh.hpp"

namespace drk::Meshes {

	MeshSystem::MeshSystem(
		drk::Devices::DeviceContext *pContext,
		drk::Graphics::EngineState *pState
	)
		: DeviceContext(pContext), EngineState(pState) {}

	void MeshSystem::UpdateStoreItem(const MeshInfo *mesh, Models::Mesh &meshModel) {
		mesh->
	}

	void MeshSystem::UploadMeshes() {
		auto meshEntities = EngineState->Registry.view<MeshInfo *>(entt::exclude<Devices::BufferView>);
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
		EngineState->Store<MeshInfo *, Models::Mesh>();
	}

	void MeshSystem::UpdateMeshes() {
		Graphics::SynchronizationState<Models::Mesh>::Update<MeshInfo *>(
			EngineState->Registry,
			EngineState->FrameIndex,
			[=](
				const MeshInfo *component,
				Models::Mesh &model
			) { UpdateStoreItem(component, model); }
		);
	}
}
