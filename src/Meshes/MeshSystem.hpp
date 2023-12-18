#pragma once

#include "../Devices/DeviceContext.hpp"
#include "../Engine/EngineState.hpp"
#include "Models/Mesh.hpp"
#include "../Draws/DrawSystem.hpp"
#include "../Systems/System.hpp"
#include "MeshGroup.hpp"
#include "Components/Mesh.hpp"

namespace drk::Meshes {
	class MeshSystem : public Draws::DrawSystem, public Systems::System<Models::Mesh, Components::Mesh> {
	protected:
	public:
		void Update(Models::Mesh& model, const Components::Mesh& mesh) override;
	protected:
		const Devices::DeviceContext& deviceContext;

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
		bool EmitDraws();
		void UpdateDraw(entt::entity drawEntity, int drawIndex);
		void UpdateDraws() { throw std::runtime_error("Not supported"); }
		Draws::DrawVertexBufferInfo GetVertexBufferInfo(entt::entity drawEntity);
		static entt::entity
			copyMeshEntity(const entt::registry& source, entt::registry& destination, entt::entity sourceEntity);
		static MeshGroup
			copyMeshGroup(const entt::registry& source, entt::registry& destination, const MeshGroup& sourceMeshGroup);
		void ProcessDirtyDraws();
	};
}