#include "../Models/LightPerspective.hpp"
#include "../Components/LightPerspective.hpp"
#include "PointLightSystem.hpp"
#include "../../Spatials/Components/Spatial.hpp"
#include "../../Objects/Components/Dirty.hpp"
#include "../Components/LightPerspectiveCollection.hpp"
#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace drk::Lights::Systems {
	PointLightSystem::PointLightSystem(
		const Devices::DeviceContext& deviceContext,
		Engine::EngineState& engineState,
		entt::registry& registry,
		ShadowMappingSystem& shadowMappingSystem
	) : System(engineState, registry), deviceContext(deviceContext), shadowMappingSystem(shadowMappingSystem) {}
	void PointLightSystem::update(
		Models::PointLight& model,
		const Components::PointLight& component,
		const Stores::StoreItem<Models::Light>& lightStoreItem,
		const Stores::StoreItem<Spatials::Models::Spatial>& spatialStoreItem
	) {
		model.constantAttenuation = component.constantAttenuation;
		model.linearAttenuation = component.linearAttenuation;
		model.quadraticAttenuation = component.quadraticAttenuation;

		const auto& frontLightPerspectiveStoreItem = registry.get<Stores::StoreItem<Models::LightPerspective>>(component.frontLightPerspectiveEntity);
		const auto& backLightPerspectiveStoreItem = registry.get<Stores::StoreItem<Models::LightPerspective>>(component.backLightPerspectiveEntity);
		const auto& leftLightPerspectiveStoreItem = registry.get<Stores::StoreItem<Models::LightPerspective>>(component.leftLightPerspectiveEntity);
		const auto& rightLightPerspectiveStoreItem = registry.get<Stores::StoreItem<Models::LightPerspective>>(component.rightLightPerspectiveEntity);
		const auto& topLightPerspectiveStoreItem = registry.get<Stores::StoreItem<Models::LightPerspective>>(component.topLightPerspectiveEntity);
		const auto& downLightPerspectiveStoreItem = registry.get<Stores::StoreItem<Models::LightPerspective>>(component.downLightPerspectiveEntity);

		model.lightStoreItemLocation = lightStoreItem.frameStoreItems[engineState.getFrameIndex()];
		model.spatialStoreItemLocation = spatialStoreItem.frameStoreItems[engineState.getFrameIndex()];

		model.frontLightPerspectiveStoreItemLocation = frontLightPerspectiveStoreItem.frameStoreItems[engineState.getFrameIndex()];
		model.backLightPerspectiveStoreItemLocation = backLightPerspectiveStoreItem.frameStoreItems[engineState.getFrameIndex()];
		model.topLightPerspectiveStoreItemLocation = topLightPerspectiveStoreItem.frameStoreItems[engineState.getFrameIndex()];
		model.downLightPerspectiveStoreItemLocation = downLightPerspectiveStoreItem.frameStoreItems[engineState.getFrameIndex()];
		model.leftLightPerspectiveStoreItemLocation = leftLightPerspectiveStoreItem.frameStoreItems[engineState.getFrameIndex()];
		model.rightLightPerspectiveStoreItemLocation = rightLightPerspectiveStoreItem.frameStoreItems[engineState.getFrameIndex()];
	}

	void PointLightSystem::processDirtyItems() {
		auto dirtyPointLightView = registry.view<
			Components::PointLight,
			Spatials::Components::Spatial,
			Components::LightPerspectiveCollection,
			Objects::Components::Dirty<Spatials::Components::Spatial>
		>();
		dirtyPointLightView.each(
			[&](
				entt::entity pointLightEntity,
				Components::PointLight& pointLight,
				Spatials::Components::Spatial& spatial,
				Components::LightPerspectiveCollection& lightPerspectiveCollection
			) {
					for (auto lightPerspectiveEntity : lightPerspectiveCollection.lightPerspectives) {
						auto& lightPerspective = registry.get<Components::LightPerspective>(lightPerspectiveEntity);
						if (lightPerspective.shadowMapRect.extent.width == 0) {
							auto allocation = shadowMappingSystem.shadowMapAllocator.allocate({ 512, 512 });
							lightPerspective.shadowMapRect = allocation.scissor;
						}
						auto absoluteRotation = glm::toMat4(spatial.absoluteRotation);
						/*lightPerspective.absoluteFront = absoluteRotation * lightPerspective.relativeFront;
						lightPerspective.absoluteUp = absoluteRotation * lightPerspective.relativeUp;*/
						lightPerspective.absoluteFront = lightPerspective.relativeFront;
						lightPerspective.absoluteUp = lightPerspective.relativeUp;
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
		);
	}
}