
#pragma once

#include "../../Systems/StorageSystem.hpp"
#include "../Models/Light.hpp"
#include "../Components/Light.hpp"

namespace drk::Lights::Systems {
	class LightSystem : public drk::Systems::StorageSystem<
		Models::Light,
		Components::Light
	> {
	public:
		LightSystem(
			Engine::EngineState& engineState,
			entt::registry& registry
		) : StorageSystem(engineState, registry) {};
		void update(Models::Light& lightModel, const Components::Light& lightComponent) override;
	};
}