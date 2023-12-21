#include "LightPerspectiveSystem.hpp"
#include "../../Spatials/Components/Spatial.hpp"
#include "../../Objects/Dirty.hpp"
#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace drk::Lights::Systems {
	LightPerspectiveSystem::LightPerspectiveSystem(
		const Devices::DeviceContext& deviceContext,
		Engine::EngineState& engineState,
		entt::registry& registry
	) : System(engineState, registry) {}
	void LightPerspectiveSystem::Update(
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
			lightPerspective.shadowMapRect.offset.x, 
			lightPerspective.shadowMapRect.offset.y, 
			lightPerspective.shadowMapRect.extent.width, 
			lightPerspective.shadowMapRect.extent.height 
		};
	}

	void LightPerspectiveSystem::ProcessDirtyItems() {
		auto dirtyLightPerspectiveView = registry.view<
			Components::LightPerspective,
			Spatials::Components::Spatial,
			Objects::Dirty<Spatials::Components::Spatial>
		>();
		dirtyLightPerspectiveView.each(
			[&](
				entt::entity lightPerspectiveEntity,
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

					registry.emplace_or_replace<Graphics::SynchronizationState<Models::LightPerspective>>(
						lightPerspectiveEntity,
						static_cast<uint32_t>(engineState.getFrameCount())
					);
			}
		);
	}
}
