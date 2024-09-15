#include "CameraSystem.hpp"

#include <entt/entt.hpp>
#include <glm/gtx/quaternion.hpp>

#include "../../Common/Components/Dirty.hpp"
#include "../../Common/Components/Name.hpp"
#include "../../Frustums/Components/Frustum.hpp"
#include "../../GlmExtensions.hpp"
#include "../../Nodes/Components/Node.hpp"
#include "../../Spatials/Components/Spatial.hpp"

namespace drk::Cameras::Systems {

CameraSystem::CameraSystem(const Devices::DeviceContext& deviceContext, Engine::EngineState& engineState, entt::registry& registry)
	: System(engineState, registry), deviceContext(deviceContext) {}

void CameraSystem::update(Models::Camera& cameraModel, const Components::Camera& camera) {
	cameraModel.perspective = camera.perspective;
	cameraModel.view = camera.view;
	cameraModel.position = glm::vec4(camera.position, 1.f);
	cameraModel.front = glm::vec4(camera.front, 0.f);
	cameraModel.up = glm::vec4(camera.up, 0.f);
	cameraModel.verticalFov = camera.verticalFov;
	cameraModel.aspectRatio = camera.aspectRatio;
	cameraModel.near = camera.near;
	cameraModel.far = camera.far;
}

void CameraSystem::processDirtyItems() {
	auto dirtyCameraView = registry.view<
		Components::Camera,
		Spatials::Components::Spatial<Spatials::Components::Relative>,
		Spatials::Components::Spatial<Spatials::Components::Absolute>,
		Common::Components::Dirty<Spatials::Components::Spatial<Spatials::Components::Relative>>>();
	dirtyCameraView.each([&](entt::entity cameraEntity,
							 Components::Camera& camera,
							 Spatials::Components::Spatial<Spatials::Components::Relative>& relativeSpatial,
							 Spatials::Components::Spatial<Spatials::Components::Absolute>& absoluteSpatial) {
		auto eye = absoluteSpatial.model * glm::vec4(camera.position, 1.f);
		auto front = glm::normalize(absoluteSpatial.model * glm::vec4(camera.front, 0.0f));
		auto up = glm::normalize(absoluteSpatial.model * glm::vec4(camera.up, 0.0f));
		auto lookAt = eye + front;
		camera.view = glm::lookAt((glm::vec3)eye, (glm::vec3)lookAt, (glm::vec3)up);
		camera.perspective = glm::perspectiveZO(camera.verticalFov, camera.aspectRatio, camera.near, camera.far);
		camera.perspective[1][1] *= -1.0f;

		registry.emplace_or_replace<Graphics::SynchronizationState<Models::Camera>>(cameraEntity, static_cast<uint32_t>(engineState.getFrameCount()));
	});
}

entt::entity CameraSystem::createCamera(glm::vec4 position, glm::vec4 front, glm::vec4 up, float verticalFov, float aspectRatio, float near, float far) const {
	auto cameraEntity = registry.create();
	Components::Camera camera = {
		.position = glm::vec4(0.f, 0.f, 0.f, 1.f), .front = front, .up = up, .verticalFov = verticalFov, .aspectRatio = aspectRatio, .near = near, .far = far
	};
	Spatials::Components::Spatial<Spatials::Components::Relative> cameraSpatial = {.position = position, .rotation = glm::quat(1, 0, 0, 0), .scale = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f)
	};
	Nodes::Components::Node cameraRelationship = {.parent = entt::null};

	auto frustum = Frustums::Components::Frustum::createFrustumFromView(position, front, up, verticalFov, aspectRatio, near, far);
	registry.emplace<Frustums::Components::Frustum>(cameraEntity, std::move(frustum));
	registry.emplace<Components::Camera>(cameraEntity, std::move(camera));
	registry.emplace<Spatials::Components::Spatial<Spatials::Components::Relative>>(cameraEntity, std::move(cameraSpatial));
	registry.emplace<Nodes::Components::Node>(cameraEntity, std::move(cameraRelationship));
	registry.emplace<Common::Components::Name>(cameraEntity, "DefaultCamera");

	return cameraEntity;
}
}  // namespace drk::Cameras::Systems
