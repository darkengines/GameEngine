#pragma once

#include "../../Draws/Systems/DrawSystem.hpp"
#include "../../Lights/Components/LightPerspective.hpp"
#include "../../Engine/EngineState.hpp"
#include "../../Cameras/Components/Camera.hpp"
#include "../../Spatials/Components/Spatial.hpp"
#include <entt/entt.hpp>

namespace drk::Meshes::Systems {
	class MeshShadowSystem : public Draws::Systems::DrawSystem {
	protected:
		entt::registry& registry;
		Engine::EngineState& engineState;
	public:
		MeshShadowSystem(
			entt::registry& registry,
			Engine::EngineState& engineState
		);
		void updateDraw(entt::entity drawEntity, int drawIndex) override;
		void updateDraws() override;
		void emitDraws() override;
	protected:
		entt::entity ProcessObjectEntity(
			entt::entity objectMeshEntity,
			entt::entity lightEntity,
			entt::entity lightPerspectiveEntity,
			const Lights::Components::LightPerspective& perspective,
			const Cameras::Components::Camera& camera,
			const Spatials::Components::Spatial& spatial,
			const Materials::Components::Material& material,
			const Meshes::Components::MeshBufferView& meshBufferView,
			std::shared_ptr<Meshes::Components::MeshResource> pMeshResource,
			const Stores::Models::StoreItemLocation& objectStoreItemLocation,
			const Stores::Models::StoreItemLocation& meshStoreItemLocation,
			const Stores::Models::StoreItemLocation& cameraStoreItemLocation,
			const Stores::Models::StoreItemLocation& lightPerspectiveStoreItemLocation,
			const Stores::Models::StoreItemLocation& lightPerspectiveSpatialStoreItemLocation
		);
	};
}