
#include "SpotlightSystem.hpp"

namespace drk::Lights::Systems {
	SpotlightSystem::SpotlightSystem(
		const Devices::DeviceContext& deviceContext,
		Engine::EngineState& engineState,
		entt::registry& registry
	) : System(engineState, registry) {}
	void SpotlightSystem::Update(
		Models::Spotlight& model,
		const Components::Spotlight& component,
		const Stores::StoreItem<Models::Light>& lightStoreItem,
		const Stores::StoreItem<Spatials::Models::Spatial>& spatialStoreItem
	) {
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