
#include "PointLightSystem.hpp"

namespace drk::Lights {
	PointLightSystem::PointLightSystem(
		const Devices::DeviceContext& deviceContext,
		Engine::EngineState& engineState,
		entt::registry& registry
	) : System(engineState, registry), deviceContext(deviceContext) {}
	void PointLightSystem::Update(Models::PointLight& model, const Components::PointLight& component)
	{
		model.relativePosition = component.relativePosition;
		model.absolutePosition = component.absolutePosition;
		model.perspective = component.perspective;
		model.topView = component.topView;
		model.bottomView = component.bottomView;
		model.leftView = component.leftView;
		model.rightView = component.rightView;
		model.backView = component.backView;
		model.frontView = component.frontView;
		model.topShadowMapRect = component.topShadowMapRect;
		model.bottomShadowMapRect = component.bottomShadowMapRect;
		model.leftShadowMapRect = component.leftShadowMapRect;
		model.rightShadowMapRect = component.rightShadowMapRect;
		model.backShadowMapRect = component.backShadowMapRect;
		model.frontShadowMapRect = component.frontShadowMapRect;
		model.normalizedTopShadowMapRect = component.normalizedTopShadowMapRect;
		model.normalizedBottomShadowMapRect = component.normalizedBottomShadowMapRect;
		model.normalizedLeftShadowMapRect = component.normalizedLeftShadowMapRect;
		model.normalizedRightShadowMapRect = component.normalizedRightShadowMapRect;
		model.normalizedBackShadowMapRect = component.normalizedBackShadowMapRect;
		model.normalizedFrontShadowMapRect = component.normalizedFrontShadowMapRect;

		model.constantAttenuation = component.constantAttenuation;
		model.linearAttenuation = component.linearAttenuation;
		model.quadraticAttenuation = component.quadraticAttenuation;
		model.far = component.far;
	}
}