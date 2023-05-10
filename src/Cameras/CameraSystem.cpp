#include "CameraSystem.hpp"
#include "../Objects/Dirty.hpp"
#include <entt/entt.hpp>
#include <glm/gtx/quaternion.hpp>
#include "../Spatials/Components/Spatial.hpp"
#include "../Objects/Relationship.hpp"
#include "../Objects/Object.hpp"
#include "../Graphics/SynchronizationState.hpp"
#include "Models/Camera.hpp"

namespace drk::Cameras {

	CameraSystem::CameraSystem(
		const Devices::DeviceContext& deviceContext,
		Engine::EngineState& engineState,
		entt::registry& registry
	)
		: DeviceContext(deviceContext), EngineState(engineState), Registry(registry) {}

	void CameraSystem::StoreCameras() {
		EngineState.Store<Models::Camera, Camera>();
	}

	void CameraSystem::UpdateCameras() {
		Graphics::SynchronizationState<Models::Camera>::Update<Camera>(
			Registry,
			EngineState.getFrameIndex(),
			std::function < void(Models::Camera & ,
		const Camera&)>(
			[=](
				Models::Camera& model,
				const Camera& component
			) { UpdateStoreItem(component, model); }
		)
		);
	}

	void CameraSystem::UpdateStoreItem(const Camera& camera, Models::Camera& cameraModel) {
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
		auto dirtyCameraView = Registry.view<Camera, Spatials::Components::Spatial, Objects::Dirty<Spatials::Components::Spatial>>();
		dirtyCameraView.each(
			[&](
				entt::entity cameraEntity,
				Camera& camera,
				Spatials::Components::Spatial& spatial,
				Objects::Dirty<Spatials::Components::Spatial>& dirty
			) {
				camera.absolutePosition = spatial.absolutePosition;
				auto absoluteRotation = glm::toMat4(spatial.absoluteRotation);
				camera.absoluteFront = absoluteRotation * camera.relativeFront;
				camera.absoluteUp = absoluteRotation * camera.relativeUp;
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

				Registry.emplace_or_replace<Graphics::SynchronizationState<Models::Camera>>(
					cameraEntity,
					static_cast<uint32_t>(EngineState.getFrameCount())
				);
			}
		);
	}

	void CameraSystem::AddCameraSystem(entt::registry& registry) {
		registry.on_construct<Camera>().connect<CameraSystem::OnCameraConstruct>();
	}

	void CameraSystem::RemoveCameraSystem(entt::registry& registry) {
		registry.on_construct<Camera>().disconnect<CameraSystem::OnCameraConstruct>();
	}

	void CameraSystem::OnCameraConstruct(entt::registry& registry, entt::entity cameraEntity) {

	}

	entt::entity CameraSystem::CreateCamera(
		glm::vec4 position,
		glm::vec4 front,
		glm::vec4 up,
		float verticalFov,
		float aspectRatio,
		float near,
		float far
	) const {
		auto cameraEntity = Registry.create();
		Camera camera = {
			.relativePosition = position,
			.relativeFront = front,
			.relativeUp = up,
			.verticalFov = verticalFov,
			.aspectRatio = aspectRatio,
			.near = near,
			.far = far
		};
		Spatials::Components::Spatial cameraSpatial = {
			.relativeScale = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f),
			.relativeRotation = glm::quat(1, 0, 0, 0),
			.relativePosition = position
		};
		Objects::Relationship cameraRelationship = {
			.childCount = 0,
			.firstChild = entt::null,
			.previousSibling = entt::null,
			.nextSibling = entt::null,
			.parent = entt::null
		};
		Objects::Object cameraObject = {
			.Name = "Default camera"
		};

		Registry.emplace<Camera>(cameraEntity, std::move(camera));
		Registry.emplace<Spatials::Components::Spatial>(cameraEntity, std::move(cameraSpatial));
		Registry.emplace<Objects::Relationship>(cameraEntity, std::move(cameraRelationship));
		Registry.emplace<Objects::Object>(cameraEntity, std::move(cameraObject));

		return cameraEntity;
	}
}
