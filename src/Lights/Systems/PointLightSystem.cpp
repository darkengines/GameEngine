
#include "PointLightSystem.hpp"

namespace drk::Lights::Systems {
	PointLightSystem::PointLightSystem(
		const Devices::DeviceContext& deviceContext,
		Engine::EngineState& engineState,
		entt::registry& registry
	) : System(engineState, registry), deviceContext(deviceContext) {}
	void PointLightSystem::Update(
		Models::PointLight& model,
		const Components::PointLight& component,
		const Stores::StoreItem<Models::Light>& lightStoreItem,
		const Stores::StoreItem<Spatials::Models::Spatial>& spatialStoreItem
	) {
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

		model.lightStoreItemLocation = lightStoreItem.frameStoreItems[engineState.getFrameIndex()];
		model.spatialStoreItemLocation = spatialStoreItem.frameStoreItems[engineState.getFrameIndex()];
	}

	void CameraSystem::ProcessDirtyItems() {
		auto dirtyCameraView = registry.view<Components::Camera, Spatials::Components::Spatial, Objects::Dirty<Spatials::Components::Spatial>>();
		dirtyCameraView.each(
			[&](
				entt::entity cameraEntity,
				Components::Camera& camera,
				Spatials::Components::Spatial& spatial,
				Objects::Dirty<Spatials::Components::Spatial>& dirty
				) {
					camera.absolutePosition = spatial.absolutePosition;
					auto absoluteRotation = glm::toMat4(spatial.absoluteRotation);
					camera.absoluteFront = absoluteRotation * camera.relativeFront;
					camera.absoluteUp = absoluteRotation * camera.relativeUp;
					camera.view = glm::lookAt(
						glm::make_vec3(camera.absolutePosition),
						glm::make_vec3(camera.absolutePosition + camera.absoluteFront),
						glm::make_vec3(camera.absoluteUp));
					camera.perspective = glm::perspectiveZO(
						camera.verticalFov,
						camera.aspectRatio,
						camera.near,
						camera.far
					);
					camera.perspective[1][1] *= -1.0f;

					registry.emplace_or_replace<Graphics::SynchronizationState<Models::Camera>>(
						cameraEntity,
						static_cast<uint32_t>(engineState.getFrameCount())
					);
			}
		);
	}
}