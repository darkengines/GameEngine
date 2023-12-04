#pragma once

#include "../Systems/System.hpp"
#include "Models/DirectionalLight.hpp"
#include "Components/DirectionalLight.hpp"

namespace drk::Lights {
	class DirectionalLightSystem : public Systems::System<Models::DirectionalLight, Components::DirectionalLight> {
	public:
		DirectionalLightSystem(
			const Devices::DeviceContext& deviceContext,
			Engine::EngineState& engineState,
			entt::registry& registry
		);
		void Update(Models::DirectionalLight& model, const Components::DirectionalLight& component);
	};
}