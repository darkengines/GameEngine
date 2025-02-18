
#pragma once

#include <entt/entity/registry.hpp>
#include "../../Engine/EngineState.hpp"

namespace drk::Scenes::Systems {
	class SceneSystem {
	protected:
		entt::registry& registry;
		Engine::EngineState& engineState;
	public:
		SceneSystem(entt::registry& registry, Engine::EngineState& engineState);
		void updateDraws();
		void updateShadowDraws();
	};
}