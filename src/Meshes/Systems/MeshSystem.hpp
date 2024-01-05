#pragma once

#include "../../Devices/DeviceContext.hpp"
#include "../../Engine/EngineState.hpp"
#include "../Models/Mesh.hpp"
#include "../../Draws/Systems/DrawSystem.hpp"
#include "../../Systems/System.hpp"
#include "../Components/Mesh.hpp"
#include "../../Objects/Models/Object.hpp"
#include "../../Objects/Components/ObjectMesh.hpp"
#include "../../Cameras/Components/Camera.hpp"
#include "../../Cameras/Models/Camera.hpp"
#include "../../Lights/Components/LightPerspective.hpp"
#include "../../Lights/Models/LightPerspective.hpp"
#include "../../Spatials/Components/Spatial.hpp"
#include "../../Spatials/Models/Spatial.hpp"
#include "../../Stores/Models/StoreItemLocation.hpp"

namespace drk::Meshes::Systems {
	class MeshSystem : public Draws::Systems::DrawSystem, public drk::Systems::System<Models::Mesh, Components::Mesh> {
	protected:
	public:
		void update(Models::Mesh& model, const Components::Mesh& mesh) override;
	protected:
		const Devices::DeviceContext& deviceContext;
	public:
		MeshSystem(
			const Devices::DeviceContext& deviceContext,
			Engine::EngineState& engineState,
			entt::registry& registry
		);
		void uploadMeshes();
		void emitDraws();
		void updateDraw(entt::entity drawEntity, int drawIndex);
		Draws::Components::DrawVertexBufferInfo getVertexBufferInfo(entt::entity drawEntity);
		static entt::entity
			copyMeshEntity(const entt::registry& source, entt::registry& destination, entt::entity sourceEntity);
		void processDirtyDraws();
	};
}