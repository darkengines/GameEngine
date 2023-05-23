#pragma once

#include <glm/vec2.hpp>
#include <optional>
#include <entt/entt.hpp>

namespace drk::Controllers {
	class FlyCamController {
	protected:
		bool MoveForward = false;
		bool MoveRight = false;
		bool MoveBackward = false;
		bool MoveLeft = false;
		bool MoveUp = false;
		bool MoveDown = false;
		glm::vec2 MousePositionDelta = {0.0f, 0.0f};
		entt::entity CameraEntity = entt::null;
		entt::registry& Registry;

	public:
		std::optional<glm::vec2> LastMousePosition = std::nullopt;
		FlyCamController(entt::registry& registry);
		void Attach(entt::entity cameraEntity);
		void HandleKeyboardEvents();
		void OnCursorPositionEvent(double xpos, double ypos);
		void OnMouseButtonEvent(int button, int action, int mods);
		void Step();
	};
}