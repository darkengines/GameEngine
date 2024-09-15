#pragma once

#include <glm/vec2.hpp>
#include <optional>
#include <entt/entt.hpp>

namespace drk::Controllers {
	class FlyCamController {
	protected:
		float forward = 0.0f;
		float right = 0.0f;
		float up = 0.0f;
		glm::vec2 MousePositionDelta = {0.0f, 0.0f};
		entt::entity CameraEntity = entt::null;
		entt::registry& Registry;
		float pitch = 0.f;
		float yaw = 0.f;
		float roll = 0.f;

	public:
		std::optional<glm::vec2> LastMousePosition = std::nullopt;
		std::optional<glm::vec2> MousePosition = std::nullopt;
		FlyCamController(entt::registry& registry);
		void Attach(entt::entity cameraEntity);
		void HandleKeyboardEvents();
		void OnCursorPositionEvent(double xpos, double ypos);
		void OnMouseButtonEvent(int button, int action, int mods);
		void Step(float dt);
	};
}