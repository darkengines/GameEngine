#include "PointLightSystem.hpp"
#include "../../Spatials/Components/Spatial.hpp"
#include "../../Objects/Dirty.hpp"
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

	void PointLightSystem::ProcessDirtyItems() {
		auto dirtyPointLightView = registry.view<Components::PointLight, Spatials::Components::Spatial, Objects::Dirty<Spatials::Components::Spatial>>();
		dirtyPointLightView.each(
			[&](
				entt::entity pointLightEntity,
				Components::PointLight& pointLight,
				Spatials::Components::Spatial& spatial,
				Objects::Dirty<Spatials::Components::Spatial>& dirty
				) {
					pointLight.absolutePosition = spatial.absolutePosition;
					auto absoluteRotation = glm::toMat4(spatial.absoluteRotation);

					registry.emplace_or_replace<Graphics::SynchronizationState<Models::PointLight>>(
						pointLightEntity,
						static_cast<uint32_t>(engineState.getFrameCount())
					);
			}
		);
	}
}