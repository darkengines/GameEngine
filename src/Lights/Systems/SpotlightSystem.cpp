#include "../Components/LightPerspective.hpp"
#include "SpotlightSystem.hpp"
#include "../../Spatials/Components/Spatial.hpp"
#include "../../Objects/Components/Dirty.hpp"
#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace drk::Lights::Systems {
	SpotlightSystem::SpotlightSystem(
		const Devices::DeviceContext& deviceContext,
		Engine::EngineState& engineState,
		entt::registry& registry
	) : System(engineState, registry) {}
	void SpotlightSystem::update(
		Models::Spotlight& model,
		const Components::Spotlight& component,
		const Stores::StoreItem<Models::Light>& lightStoreItem,
		const Stores::StoreItem<Spatials::Models::Spatial>& spatialStoreItem,
		const Stores::StoreItem<Models::LightPerspective>& lightPerspective
	) {
		model.innerConeAngle = component.innerConeAngle;
		model.outerConeAngle = component.outerConeAngle;
		model.constantAttenuation = component.constantAttenuation;
		model.linearAttenuation = component.linearAttenuation;
		model.quadraticAttenuation = component.quadraticAttenuation;

		model.lightStoreItemLocation = lightStoreItem.frameStoreItems[engineState.getFrameIndex()];
		model.spatialStoreItemLocation = spatialStoreItem.frameStoreItems[engineState.getFrameIndex()];
		model.lightPerspectiveStoreItemLocation = lightPerspective.frameStoreItems[engineState.getFrameIndex()];
	}

	void SpotlightSystem::ProcessDirtyItems() {
		/*auto dirtySpotlightView = registry.view<
			Components::Spotlight,
			Components::LightPerspective,
			Spatials::Components::Spatial,
			Objects::Dirty<Spatials::Components::Spatial>
		>();
		dirtySpotlightView.each(
			[&](
				entt::entity spotlightEntity,
				Components::Spotlight& spotlight,
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

					registry.emplace_or_replace<Graphics::SynchronizationState<Models::Spotlight>>(
						spotlightEntity,
						static_cast<uint32_t>(engineState.getFrameCount())
					);
			});*/
	}
}