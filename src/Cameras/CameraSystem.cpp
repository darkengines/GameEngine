#include "CameraSystem.hpp"
#include "../Objects/Dirty.hpp"
#include "../Spatials/Spatial.hpp"
#include <entt/entt.hpp>
#include <glm/gtx/quaternion.hpp>

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

	void CameraSystem::ProcessDirtyItems() {
		auto dirtyCameraView = EngineState->Registry.view<Camera, Spatials::Spatial, Objects::Dirty<Spatials::Spatial>>();
		dirtyCameraView.each(
			[&](
				entt::entity cameraEntity,
				Camera &camera,
				Spatials::Spatial &spatial,
				Objects::Dirty<Spatials::Spatial> &dirty
			) {
				camera.absolutePosition = spatial.absolutePosition;
				auto absoluteRotation = glm::toMat4(spatial.absoluteRotation);
				camera.absoluteFront =  absoluteRotation * camera.relativeFront;
				camera.absoluteUp =  absoluteRotation * camera.relativeUp;
				camera.view = glm::lookAt(
					glm::make_vec3(camera.absolutePosition),
					glm::make_vec3(camera.absolutePosition + camera.absoluteFront),
					glm::make_vec3(camera.absoluteUp));
				camera.perspective = glm::perspectiveZO(
					camera.verticalFov,
					camera.aspectRatio,
					camera.near,
					camera.far
				);
				camera.perspective[1][1] *= -1.0f;

				EngineState->Registry.emplace_or_replace<Graphics::SynchronizationState<Models::Camera>>(
					cameraEntity,
					static_cast<uint32_t>(EngineState->FrameStates.size())
				);
			}
		);
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
