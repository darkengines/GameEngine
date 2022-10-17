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
				const Camera& component
			) { UpdateStoreItem(component, model); }
		);
	}

	void CameraSystem::UpdateStoreItem(const Camera& camera, Models::Camera &cameraModel) {
		cameraModel = {
			.perspective = camera.perspective,
			.view = camera.view,
			.relativePosition = camera.relativePosition,
			.relativeFront = camera.relativeFront,
			.relativeUp = camera.relativeUp,
			.absolutePosition = camera.absolutePosition,
			.absoluteFront = camera.absoluteFront,
			.absoluteUp = camera.absoluteUp,
			.verticalFov = camera.verticalFov,
			.aspectRatio = camera.aspectRatio,
			.near = camera.near,
			.far = camera.far,
		};
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
