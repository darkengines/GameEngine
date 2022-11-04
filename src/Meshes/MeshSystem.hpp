#pragma once

#include "../Devices/DeviceContext.hpp"
#include "../Engine/EngineState.hpp"
#include "Models/Mesh.hpp"

namespace drk::Meshes {
	class MeshSystem {
	protected:
		const Devices::DeviceContext& DeviceContext;
		Engine::EngineState& EngineState;
		entt::registry& Registry;
		void UpdateStoreItem(const MeshInfo *mesh, Models::Mesh &meshModel);

	public:
		MeshSystem(const Devices::DeviceContext& deviceContext, Engine::EngineState& engineState, entt::registry& registry);
		static void AddMeshSystem(entt::registry &registry);
		static void RemoveMeshSystem(entt::registry &registry);
		static void OnMeshConstruct(entt::registry &registry, entt::entity meshEntity);
		void UploadMeshes();
		void UpdateMeshes();
		void StoreMeshes();
	};
}