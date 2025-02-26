#include "LightPerspectiveSystem.hpp"
#include "../../Spatials/Components/Spatial.hpp"
#include "../../Common/Components/Dirty.hpp"
#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace drk::Lights::Systems {
	LightPerspectiveSystem::LightPerspectiveSystem(
		const Devices::DeviceContext& deviceContext,
		Engine::EngineState& engineState,
		entt::registry& registry,
		ShadowMappingSystem& shadowMappingSystem
	) : shadowMappingSystem(shadowMappingSystem),
		StorageSystem(engineState, registry) {}
	void LightPerspectiveSystem::update(
		Models::LightPerspective& model,
		const Components::LightPerspective& lightPerspective
	) {
		model.view = lightPerspective.view;
		model.perspective = lightPerspective.perspective;
		model.relativeFront = lightPerspective.relativeFront;
		model.relativeUp = lightPerspective.relativeUp;
		model.absoluteFront = lightPerspective.absoluteFront;
		model.absoluteUp = lightPerspective.absoluteUp;
		model.shadowMapRect = {
			lightPerspective.shadowMapRect.offset.x / (float) ShadowMappingSystem::shadowMapWidth,
			lightPerspective.shadowMapRect.offset.y / (float) ShadowMappingSystem::shadowMapHeight,
			lightPerspective.shadowMapRect.extent.width / (float) ShadowMappingSystem::shadowMapWidth,
			lightPerspective.shadowMapRect.extent.height / (float) ShadowMappingSystem::shadowMapHeight
		};
		model.near = lightPerspective.near;
		model.far = lightPerspective.far;
		model.aspectRatio = lightPerspective.aspectRatio;
		model.verticalFov = lightPerspective.verticalFov;
	}

	void LightPerspectiveSystem::processDirtyItems() {
		auto dirtyLightPerspectiveView = registry.view<
			Components::LightPerspective,
			Spatials::Components::Spatial<Spatials::Components::Absolute>,
			Common::Components::Dirty<Spatials::Components::Spatial<Spatials::Components::Absolute>>
		>();
		dirtyLightPerspectiveView.each(
			[&](
				entt::entity lightPerspectiveEntity,
				Components::LightPerspective& lightPerspective,
				Spatials::Components::Spatial<Spatials::Components::Absolute>& spatial
			) {
				if (lightPerspective.shadowMapRect.extent.width == 0) {
					auto allocation = shadowMappingSystem.shadowMapAllocator.allocate({1024, 1024});
					lightPerspective.shadowMapRect = allocation.scissor;
				}
				lightPerspective.absoluteFront = spatial.rotation * lightPerspective.relativeFront;
				lightPerspective.absoluteUp = spatial.rotation * lightPerspective.relativeUp;
				//lightPerspective.absoluteUp = lightPerspective.relativeUp;
				lightPerspective.view = glm::lookAt(
					glm::make_vec3(spatial.position),
					glm::make_vec3(spatial.position + lightPerspective.absoluteFront),
					glm::make_vec3(lightPerspective.absoluteUp));
				lightPerspective.perspective = glm::perspectiveZO<float>(
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
		);
	}
}
