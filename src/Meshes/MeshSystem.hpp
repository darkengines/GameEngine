#pragma once

#include "../Devices/DeviceContext.hpp"
#include "../Engine/EngineState.hpp"
#include "Models/Mesh.hpp"
#include "../Draws/DrawSystem.hpp"

namespace drk::Meshes {
	class MeshSystem : public Draws::DrawSystem {
	protected:
		const Devices::DeviceContext& deviceContext;
		Engine::EngineState& engineState;
		entt::registry& registry;
		void UpdateStoreItem(const MeshInfo* mesh, Models::Mesh& meshModel);

	public:
		MeshSystem(
			const Devices::DeviceContext& deviceContext,
			Engine::EngineState& engineState,
			entt::registry& registry
		);
		static void AddMeshSystem(entt::registry& registry);
		static void RemoveMeshSystem(entt::registry& registry);
		static void OnMeshConstruct(entt::registry& registry, entt::entity meshEntity);
		void UploadMeshes();
		void UpdateMeshes();
		void StoreMeshes();
		void EmitDraws();
		void UpdateDraw(entt::entity drawEntity, int drawIndex);
		Draws::DrawVertexBufferInfo GetVertexBufferInfo(entt::entity drawEntity);
	};
}