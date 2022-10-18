#include "CameraSystem.hpp"

namespace drk::Cameras {

	CameraSystem::CameraSystem(drk::Devices::DeviceContext *pContext, drk::Graphics::EngineState *pState)
		: DeviceContext(pContext), EngineState(pState) {}

	void CameraSystem::StoreCameras() {
		EngineState->Store<Models::Camera, Camera>();
	}

	void CameraSystem::UpdateCameras() {
		Graphics::SynchronizationState<Models::Camera>::Update<Camera>(
			EngineState->Registry,
			EngineState->FrameIndex,
			[=](
				Models::Camera &model,
				const Camera &component
			) { UpdateStoreItem(component, model); }
		);
	}

	void CameraSystem::UpdateStoreItem(const Camera &camera, Models::Camera &cameraModel) {
		cameraModel.perspective = camera.perspective;
		cameraModel.view = camera.view;
		cameraModel.relativePosition = camera.relativePosition;
		cameraModel.relativeFront = camera.relativeFront;
		cameraModel.relativeUp = camera.relativeUp;
		cameraModel.absolutePosition = camera.absolutePosition;
		cameraModel.absoluteFront = camera.absoluteFront;
		cameraModel.absoluteUp = camera.absoluteUp;
		cameraModel.verticalFov = camera.verticalFov;
		cameraModel.aspectRatio = camera.aspectRatio;
		cameraModel.near = camera.near;
		cameraModel.far = camera.far;
	}

	void CameraSystem::AddCameraSystem(entt::registry &registry) {
		registry.on_construct<Camera>().connect<CameraSystem::OnCameraConstruct>();
	}

	void CameraSystem::RemoveCameraSystem(entt::registry &registry) {
		registry.on_construct<Camera>().disconnect<CameraSystem::OnCameraConstruct>();
	}

	void CameraSystem::OnCameraConstruct(entt::registry &registry, entt::entity cameraEntity) {

	}
}
