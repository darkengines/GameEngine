#pragma once

#include "../../Devices/DeviceContext.hpp"
#include "../../Engine/EngineState.hpp"
#include "../Components/Camera.hpp"
#include "../Models/Camera.hpp"
#include "../../Systems/System.hpp"

namespace drk::Cameras::Systems {
	class CameraSystem : public drk::Systems::System<Models::Camera, Components::Camera> {
	protected:
		const Devices::DeviceContext& deviceContext;
	public:
		CameraSystem(
			const Devices::DeviceContext& deviceContext,
			Engine::EngineState& engineState,
			entt::registry& registry
		);
		void update(Models::Camera& cameraModel, const Components::Camera& camera);
		void processDirtyItems();

		entt::entity createCamera(
			glm::vec4 position,
			glm::vec4 front,
			glm::vec4 up,
			float verticalFov,
			float aspectRatio,
			float near,
			float far
		) const;
	};
}