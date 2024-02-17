#include "DirectionalLightSystem.hpp"

namespace drk::Lights::Systems {
	DirectionalLightSystem::DirectionalLightSystem(
		const Devices::DeviceContext& deviceContext,
		Engine::EngineState& engineState,
		entt::registry& registry
	) : System(engineState, registry) {}
	void DirectionalLightSystem::update(
		Models::DirectionalLight& model,
		const Components::DirectionalLight& directionalLight,
		const Stores::StoreItem<Models::Light>& lightStoreItem,
		const Stores::StoreItem<Spatials::Models::Spatial>& spatialStoreItem,
		const Stores::StoreItem<Models::LightPerspective>& lightPerspective
	) {
		model.lightStoreItemLocation = lightStoreItem.frameStoreItems[engineState.getFrameIndex()];
		model.spatialStoreItemLocation = spatialStoreItem.frameStoreItems[engineState.getFrameIndex()];
		model.lightPerspectiveStoreItemLocation = lightPerspective.frameStoreItems[engineState.getFrameIndex()];
	}

	void DirectionalLightSystem::processDirtyItems() {
		/*auto dirtyDirectionalLightView = registry.view<
			Components::DirectionalLight,
			Components::LightPerspective,
			Spatials::Components::Spatial, 
			Objects::Dirty<Spatials::Components::Spatial>
		>();
		dirtyDirectionalLightView.each(
			[&](
				entt::entity directionalLightEntity,
				Components::DirectionalLight& directionalLight,
				Components::LightPerspective& lightPerspective,
				Spatials::Components::Spatial& spatial,
				Objects::Dirty<Spatials::Components::Spatial>& dirty
				) {
					auto absoluteRotation = glm::toMat4(spatial.absoluteRotation);
					lightPerspective.absoluteFront = absoluteRotation * lightPerspective.relativeFront;
					lightPerspective.absoluteUp = absoluteRotation * lightPerspective.relativeUp;
					lightPerspective.view = glm::lookAt(
						glm::zero<glm::vec3>(),
						glm::make_vec3(glm::zero<glm::vec4>() + lightPerspective.absoluteFront),
						glm::make_vec3(lightPerspective.absoluteUp));
					lightPerspective.perspective = glm::perspectiveZO<float>(
						1,
						1,
						0.1,
						1024
					);
					lightPerspective.perspective[1][1] *= -1.0f;

					registry.emplace_or_replace<Graphics::SynchronizationState<Models::DirectionalLight>>(
						directionalLightEntity,
						static_cast<uint32_t>(engineState.getFrameCount())
					);
			}
		);*/
	}
}
