#include "FlyCamController.hpp"
#include "../Cameras/Camera.hpp"
#include "../Spatials/Spatial.hpp"
#include "../GlmExtensions.hpp"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/quaternion.hpp>
#include "../Graphics/SynchronizationState.hpp"
#include "../Spatials/Models/Spatial.hpp"
#include "../Objects/Dirty.hpp"

namespace drk::Controllers {

	FlyCamController::FlyCamController(entt::registry *registry) : Registry(registry) {

	}

	void FlyCamController::Attach(entt::entity cameraEntity) {
		CameraEntity = cameraEntity;
	}

	void FlyCamController::OnKeyboardEvent(int key, int scancode, int action, int mods) {
		switch (key) {
			case (GLFW_KEY_W): {
				if (action == GLFW_PRESS) {
					MoveForward = true;
				}
				if (action == GLFW_RELEASE) {
					MoveForward = false;
				}
				break;
			}
			case (GLFW_KEY_S): {
				if (action == GLFW_PRESS) {
					MoveBackward = true;
				}
				if (action == GLFW_RELEASE) {
					MoveBackward = false;
				}
				break;
			}
			case (GLFW_KEY_A): {
				if (action == GLFW_PRESS) {
					MoveLeft = true;
				}
				if (action == GLFW_RELEASE) {
					MoveLeft = false;
				}
				break;
			}
			case (GLFW_KEY_D): {
				if (action == GLFW_PRESS) {
					MoveRight = true;
				}
				if (action == GLFW_RELEASE) {
					MoveRight = false;
				}
				break;
			}
			case (GLFW_KEY_SPACE): {
				if (action == GLFW_PRESS) {
					MoveUp = true;
				}
				if (action == GLFW_RELEASE) {
					MoveUp = false;
				}
				break;
			}
			case (GLFW_KEY_LEFT_CONTROL): {
				if (action == GLFW_PRESS) {
					MoveDown = true;
				}
				if (action == GLFW_RELEASE) {
					MoveDown = false;
				}
				break;
			}
		}
	}

	void FlyCamController::OnCursorPositionEvent(double xpos, double ypos) {
		glm::vec2 mousePosition{xpos, ypos};
		if (LastMousePosition.has_value()) {
			MousePositionDelta = (mousePosition - *LastMousePosition) * 0.0005f;
		}
		LastMousePosition = std::move(mousePosition);
	}

	void FlyCamController::OnMouseButtonEvent(int button, int action, int mods) {

	}

	void FlyCamController::Step() {
		auto &camera = Registry->get<Cameras::Camera>(CameraEntity);
		auto &cameraSpatial = Registry->get<Spatials::Spatial>(CameraEntity);
		auto up = camera.absoluteUp;
		auto front = camera.absoluteFront;
		auto side = GlmExtensions::cross(front, up);

		auto hasUpdate = false;

		if (MoveForward) {
			cameraSpatial.relativePosition += front * 0.01f;
			hasUpdate = true;
		}
		if (MoveBackward) {
			cameraSpatial.relativePosition -= front * 0.01f;
			hasUpdate = true;
		}
		if (MoveRight) {
			cameraSpatial.relativePosition += side * 0.005f;
			hasUpdate = true;
		}
		if (MoveLeft) {
			cameraSpatial.relativePosition -= side * 0.005f;
			hasUpdate = true;
		}
		if (MoveUp) {
			cameraSpatial.relativePosition += camera.relativeUp * 0.005f;
			hasUpdate = true;
		}
		if (MoveDown) {
			cameraSpatial.relativePosition -= camera.relativeUp * 0.005f;
			hasUpdate = true;
		}

		if (MousePositionDelta.x != 0 || MousePositionDelta.y != 0) {
			if (MousePositionDelta.x) {
				auto q = glm::angleAxis(-MousePositionDelta.x, glm::vec3(camera.relativeUp));
				cameraSpatial.relativeRotation = q * cameraSpatial.relativeRotation ;
				MousePositionDelta.x = 0;
			}
			if (MousePositionDelta.y) {
				cameraSpatial.relativeRotation = glm::rotate(
					cameraSpatial.relativeRotation,
					-MousePositionDelta.y,
					glm::vec3(GlmExtensions::cross(camera.relativeFront, camera.relativeUp))
				);
				MousePositionDelta.y = 0;
			}
			hasUpdate = true;
		}
		if (hasUpdate) {
			Registry->emplace_or_replace<Graphics::SynchronizationState<Spatials::Models::Spatial>>(CameraEntity, 2u);
			Registry->emplace_or_replace<Objects::Dirty<Spatials::Spatial>>(CameraEntity);
		}
	}
}
