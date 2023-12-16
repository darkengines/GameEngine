#include "DirectionalLightSystem.hpp"
#include "../../Spatials/Components/Spatial.hpp"
#include "../../Objects/Dirty.hpp"
#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace drk::Lights::Systems {
	DirectionalLightSystem::DirectionalLightSystem(
		const Devices::DeviceContext& deviceContext,
		Engine::EngineState& engineState,
		entt::registry& registry
	) : System(engineState, registry) {}
	void DirectionalLightSystem::Update(
		Models::DirectionalLight& model,
		const Components::DirectionalLight& directionalLight,
		const Stores::StoreItem<Models::Light>& lightStoreItem,
		const Stores::StoreItem<Spatials::Models::Spatial>& spatialStoreItem
	) {
		model.view = directionalLight.view;
		model.perspective = directionalLight.perspective;
		model.view = directionalLight.view;
		model.relativeDirection = directionalLight.relativeDirection;
		model.relativeUp = directionalLight.relativeUp;
		model.absoluteDirection = directionalLight.absoluteDirection;
		model.absoluteUp = directionalLight.absoluteUp;
		model.shadowMapRect = directionalLight.shadowMapRect;

		model.lightStoreItemLocation = lightStoreItem.frameStoreItems[engineState.getFrameIndex()];
		model.spatialStoreItemLocation = spatialStoreItem.frameStoreItems[engineState.getFrameIndex()];
	}

	void DirectionalLightSystem::ProcessDirtyItems() {
		auto dirtyCameraView = registry.view<
			Components::DirectionalLight, 
			Spatials::Components::Spatial, 
			Objects::Dirty<Spatials::Components::Spatial>
		>();
		dirtyCameraView.each(
			[&](
				entt::entity directionalLightEntity,
				Components::DirectionalLight& directionalLight,
				Spatials::Components::Spatial& spatial,
				Objects::Dirty<Spatials::Components::Spatial>& dirty
				) {
					auto absoluteRotation = glm::toMat4(spatial.absoluteRotation);
					directionalLight.absoluteDirection = absoluteRotation * directionalLight.relativeDirection;
					directionalLight.absoluteUp = absoluteRotation * directionalLight.relativeUp;
					directionalLight.view = glm::lookAt(
						glm::zero<glm::vec3>(),
						glm::make_vec3(glm::zero<glm::vec4>() + directionalLight.absoluteDirection),
						glm::make_vec3(directionalLight.absoluteUp));
					directionalLight.perspective = glm::perspectiveZO<float>(
						1,
						1,
						0.1,
						1024
					);
					directionalLight.perspective[1][1] *= -1.0f;

					registry.emplace_or_replace<Graphics::SynchronizationState<Models::DirectionalLight>>(
						directionalLightEntity,
						static_cast<uint32_t>(engineState.getFrameCount())
					);
			}
		);
	}
}
