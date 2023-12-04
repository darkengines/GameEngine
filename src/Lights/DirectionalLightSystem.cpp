#include "DirectionalLightSystem.hpp"

namespace drk::Lights {
	DirectionalLightSystem::DirectionalLightSystem(
		const Devices::DeviceContext& deviceContext,
		Engine::EngineState& engineState,
		entt::registry& registry
	) : System(engineState, registry) {}
	void
	DirectionalLightSystem::Update(Models::DirectionalLight& model, const Components::DirectionalLight& component) {
		model.view = component.view;
		model.perspective = component.perspective;
		model.view = component.view;
		model.relativeDirection = component.relativeDirection;
		model.relativeUp = component.relativeUp;
		model.absoluteDirection = component.absoluteDirection;
		model.absoluteUp = component.absoluteUp;
		model.shadowMapRect = component.shadowMapRect;
	}
}
