
#include "SpotlightSystem.hpp"
#include "../../Spatials/Components/Spatial.hpp"
#include "../../Objects/Dirty.hpp"
#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

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

	void SpotlightSystem::ProcessDirtyItems() {
		auto dirtyCameraView = registry.view<Components::Spotlight, Spatials::Components::Spatial, Objects::Dirty<Spatials::Components::Spatial>>();
		dirtyCameraView.each(
			[&](
				entt::entity spotlightEntity,
				Components::Spotlight& spotlight,
				Spatials::Components::Spatial& spatial,
				Objects::Dirty<Spatials::Components::Spatial>& dirty
				) {
					spotlight.absolutePosition = spatial.absolutePosition;
					auto absoluteRotation = glm::toMat4(spatial.absoluteRotation);
					spotlight.absoluteDirection = absoluteRotation * spotlight.relativeDirection;
					spotlight.absoluteUp = absoluteRotation * spotlight.relativeUp;
					spotlight.perspective[1][1] *= -1.0f;

					registry.emplace_or_replace<Graphics::SynchronizationState<Models::Spotlight>>(
						spotlightEntity,
						static_cast<uint32_t>(engineState.getFrameCount())
					);
			}
		);
	}
}