#pragma once

#include <GLFW/glfw3.h>
#include "../Windows/Window.hpp"
#include <imgui.h>

namespace drk::UserInterfaces {
	class UserInterface {
	protected:
		const Windows::Window& window;
		bool isVisible;
		bool isExplorationMode = false;

	public:
		UserInterface(const Windows::Window& window) : window(window), isVisible(false) {}
		void HandleKeyboardEvents() {
			if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_LeftAlt)) {
				int cursorMode;
				isExplorationMode = !isExplorationMode;
				if (isExplorationMode) {
					isVisible = false;
					//cursorMode = ImGuiMouseCursor_None;
					cursorMode = GLFW_CURSOR_DISABLED;
				} else {
					isVisible = true;
					//cursorMode = ImGuiMouseCursor_Arrow;
					cursorMode = GLFW_CURSOR_NORMAL;
				}
				//ImGui::SetMouseCursor(cursorMode);
				glfwFocusWindow(window.GetWindow());
				glfwSetInputMode(window.GetWindow(), GLFW_CURSOR, cursorMode);
			}
		}

		void OnCursorPositionEvent(double xpos, double ypos) {}
		void OnMouseButtonEvent(int button, int action, int mods) {}
		bool IsVisible() const { return isVisible; }
		bool IsExplorationMode() const { return isExplorationMode; }
	};
}