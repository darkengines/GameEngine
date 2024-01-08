#include "CameraSystem.hpp"
#include "../../Objects/Components/Dirty.hpp"
#include <entt/entt.hpp>
#include <glm/gtx/quaternion.hpp>
#include "../../Spatials/Components/Spatial.hpp"
#include "../../Objects/Components/Relationship.hpp"
#include "../../Objects/Components/Object.hpp"
#include "../../Graphics/SynchronizationState.hpp"
#include "../../Frustums/Components/Frustum.hpp"
#include "../Models/Camera.hpp"

namespace drk::Cameras::Systems {

	CameraSystem::CameraSystem(
		const Devices::DeviceContext& deviceContext,
		Engine::EngineState& engineState,
		entt::registry& registry
	) : System(engineState, registry), deviceContext(deviceContext) {}

	void CameraSystem::update(Models::Camera& cameraModel, const Components::Camera& camera) {
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

	void CameraSystem::processDirtyItems() {
		auto dirtyCameraView = registry.view<
			Components::Camera,
			Spatials::Components::Spatial,
			Objects::Components::Dirty<Spatials::Components::Spatial>
		>();
		dirtyCameraView.each(
			[&](
				entt::entity cameraEntity,
				Components::Camera& camera,
				Spatials::Components::Spatial& spatial
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

					registry.emplace_or_replace<Graphics::SynchronizationState<Models::Camera>>(
						cameraEntity,
						static_cast<uint32_t>(engineState.getFrameCount())
					);
			}
		);
	}

	entt::entity CameraSystem::createCamera(
		glm::vec4 position,
		glm::vec4 front,
		glm::vec4 up,
		float verticalFov,
		float aspectRatio,
		float near,
		float far
	) const {
		auto cameraEntity = registry.create();
		Components::Camera camera = {
			.relativePosition = position,
			.relativeFront = front,
			.relativeUp = up,
			.verticalFov = verticalFov,
			.aspectRatio = aspectRatio,
			.near = near,
			.far = far
		};
		Spatials::Components::Spatial cameraSpatial = {
			.relativeScale = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
			.relativeRotation = glm::quat(1, 0, 0, 0),
			.relativePosition = position
		};
		Objects::Components::Relationship cameraRelationship = {
			.parent = entt::null
		};
		Objects::Components::Object cameraObject = {
			.Name = "Default camera"
		};

		auto frustum = Frustums::Components::Frustum::createFrustumFromView(position, front, up, verticalFov, aspectRatio, near, far);
		registry.emplace<Frustums::Components::Frustum>(cameraEntity, std::move(frustum));
		registry.emplace<Components::Camera>(cameraEntity, std::move(camera));
		registry.emplace<Spatials::Components::Spatial>(cameraEntity, std::move(cameraSpatial));
		registry.emplace<Objects::Components::Relationship>(cameraEntity, std::move(cameraRelationship));
		registry.emplace<Objects::Components::Object>(cameraEntity, std::move(cameraObject));

		return cameraEntity;
	}
}
