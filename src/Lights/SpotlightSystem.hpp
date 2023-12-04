
#pragma once

#include "Models/Spotlight.hpp"
#include "Components/Spotlight.hpp"
#include "../Systems/System.hpp"

namespace drk::Lights {
	class SpotlightSystem: public Systems::System<Models::Spotlight, Components::Spotlight> {
	public:
		SpotlightSystem(
			const Devices::DeviceContext& deviceContext,
			Engine::EngineState& engineState,
			entt::registry& registry
		);
		void Update(Models::Spotlight& model, const Components::Spotlight& component);
	};
}