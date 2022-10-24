#pragma once

#include "../Devices/DeviceContext.hpp"
#include "../Graphics/EngineState.hpp"
#include "Camera.hpp"
#include "Models/Camera.hpp"

namespace drk::Cameras {
	class CameraSystem {
	protected:
		Devices::DeviceContext *DeviceContext;
		Graphics::EngineState *EngineState;
	public:
		CameraSystem(
			drk::Devices::DeviceContext *pContext,
			drk::Graphics::EngineState *pState
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
		);
	};
}