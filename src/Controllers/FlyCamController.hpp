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
		std::optional<glm::vec2> LastMousePosition = std::nullopt;
		glm::vec2 MousePositionDelta = {0.0f, 0.0f};
		entt::entity CameraEntity = entt::null;
		entt::registry& Registry;

	public:
		FlyCamController(entt::registry& registry);
		void Attach(entt::entity cameraEntity);;
		void OnKeyboardEvent(int key, int scancode, int action, int mods);
		void OnCursorPositionEvent(double xpos, double ypos);
		void OnMouseButtonEvent(int button, int action, int mods);
		void Step();
	};
}