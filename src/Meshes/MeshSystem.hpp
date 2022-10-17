#pragma once

#include "../Devices/DeviceContext.hpp"
#include "../Graphics/EngineState.hpp"
#include "Models/Mesh.hpp"

namespace drk::Meshes {
	class MeshSystem {
	protected:
		Devices::DeviceContext *DeviceContext;
		Graphics::EngineState *EngineState;
		void UpdateStoreItem(const MeshInfo *mesh, Models::Mesh &meshModel);

	public:
		MeshSystem(Devices::DeviceContext *pContext, Graphics::EngineState *pState);
		static void AddMeshSystem(entt::registry &registry);
		static void RemoveMeshSystem(entt::registry &registry);
		static void OnMeshConstruct(entt::registry &registry, entt::entity meshEntity);
		void UploadMeshes();
		void UpdateMeshes();
		void StoreMeshes();
	};
}