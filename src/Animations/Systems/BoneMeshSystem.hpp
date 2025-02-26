#pragma once

#include "../Models/BoneMesh.hpp"
#include "../Components/BoneMesh.hpp"
#include "../../Systems/StorageSystem.hpp"
#include "../../Engine/EngineState.hpp"
#include <entt/entt.hpp>

namespace drk::Animations::Systems {
	class BoneMeshSystem : public drk::Systems::StorageSystem<Models::BoneMesh, Components::BoneMesh> {
	public:
		BoneMeshSystem(
			Engine::EngineState& engineState,
			entt::registry& registry
		);
		void update(Models::BoneMesh& boneMeshModel, const Components::BoneMesh& boneMeshComponent) override;
	};
}