#pragma once

#include "../../Systems/System.hpp"
#include "../Models/LightPerspective.hpp"
#include "../Components/LightPerspective.hpp"

namespace drk::Lights::Systems {
	class LightPerspectiveSystem : public drk::Systems::System<
		Models::LightPerspective,
		Components::LightPerspective
	> {
	public:
		LightPerspectiveSystem(
			const Devices::DeviceContext& deviceContext,
			Engine::EngineState& engineState,
			entt::registry& registry
		);
		void Update(
			Models::LightPerspective& model,
			const Components::LightPerspective& lightPerspective
		);
		void ProcessDirtyItems();
	};
}