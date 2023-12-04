
#include "SpotlightSystem.hpp"

namespace drk::Lights {
	SpotlightSystem::SpotlightSystem(
		const Devices::DeviceContext& deviceContext,
		Engine::EngineState& engineState,
		entt::registry& registry
	) : System(engineState, registry) {}
	void SpotlightSystem::Update(Models::Spotlight& model, const Components::Spotlight& component) {
		model.relativePosition = component.relativePosition;
		model.relativeDirection = component.relativeDirection;
		model.relativeUp = component.relativeUp;
		model.absolutePosition = component.absolutePosition;
		model.absoluteDirection = component.absoluteDirection;
		model.absoluteUp = component.absoluteUp;
		model.shadowMapRect = component.shadowMapRect;
		model.view = component.view;
		model.perspective = component.perspective;
		model.innerConeAngle = component.innerConeAngle;
		model.outerConeAngle = component.outerConeAngle;
		model.constantAttenuation = component.constantAttenuation;
		model.linearAttenuation = component.linearAttenuation;
		model.quadraticAttenuation = component.quadraticAttenuation;
	}
}