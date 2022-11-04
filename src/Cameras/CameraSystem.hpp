#pragma once

#include "../Devices/DeviceContext.hpp"
#include "../Engine/EngineState.hpp"
#include "Camera.hpp"
#include "Models/Camera.hpp"

namespace drk::Cameras {
	class CameraSystem {
	protected:
		const Devices::DeviceContext& DeviceContext;
		Engine::EngineState& EngineState;
		entt::registry& Registry;
	public:
		CameraSystem(
			const Devices::DeviceContext& deviceContext,
			Engine::EngineState& engineState,
			entt::registry& registry
		);
		static void AddCameraSystem(entt::registry &registry);
		static void RemoveCameraSystem(entt::registry &registry);
		static void OnCameraConstruct(entt::registry &registry, entt::entity cameraEntity);
		void UpdateStoreItem(const Camera &camera, Models::Camera &cameraModel);
		void StoreCameras();
		void UpdateCameras();
		void ProcessDirtyItems();

		entt::entity CreateCamera(
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