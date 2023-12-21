#include "../Models/LightPerspective.hpp"
#include "../Components/LightPerspective.hpp"
#include "PointLightSystem.hpp"
#include "../../Spatials/Components/Spatial.hpp"
#include "../../Objects/Dirty.hpp"
#include "../Components/LightPerspectiveCollection.hpp"
#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

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
		model.constantAttenuation = component.constantAttenuation;
		model.linearAttenuation = component.linearAttenuation;
		model.quadraticAttenuation = component.quadraticAttenuation;

		model.lightStoreItemLocation = lightStoreItem.frameStoreItems[engineState.getFrameIndex()];
		model.spatialStoreItemLocation = spatialStoreItem.frameStoreItems[engineState.getFrameIndex()];
	}

	void PointLightSystem::ProcessDirtyItems() {
		/*auto dirtyPointLightView = registry.view<
			Components::PointLight,
			Spatials::Components::Spatial,
			Components::LightPerspectiveCollection,
			Objects::Dirty<Spatials::Components::Spatial>
		>();
		dirtyPointLightView.each(
			[&](
				entt::entity pointLightEntity,
				Components::PointLight& pointLight,
				Spatials::Components::Spatial& spatial,
				Components::LightPerspectiveCollection& lightPerspectiveCollection,
				Objects::Dirty<Spatials::Components::Spatial>& dirty
			) {
				for (auto lightPerspectiveEntity : lightPerspectiveCollection.lightPerspectives) {
					auto& lightPerspective = registry.get<Components::LightPerspective>(lightPerspectiveEntity);
					auto absoluteRotation = glm::toMat4(spatial.absoluteRotation);
					lightPerspective.absoluteFront = absoluteRotation * lightPerspective.relativeFront;
					lightPerspective.absoluteUp = absoluteRotation * lightPerspective.relativeUp;
					lightPerspective.view = glm::lookAt(
						glm::make_vec3(spatial.absolutePosition),
						glm::make_vec3(spatial.absolutePosition + lightPerspective.absoluteFront),
						glm::make_vec3(lightPerspective.absoluteUp));
					lightPerspective.perspective = glm::perspectiveZO(
						lightPerspective.verticalFov,
						lightPerspective.aspectRatio,
						lightPerspective.near,
						lightPerspective.far
					);
					lightPerspective.perspective[1][1] *= -1.0f;

					registry.emplace_or_replace<Graphics::SynchronizationState<Models::LightPerspective>>(
						lightPerspectiveEntity,
						static_cast<uint32_t>(engineState.getFrameCount())
					);
				}
			}
		);*/
	}
}