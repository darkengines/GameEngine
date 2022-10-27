#pragma once

#include <GLFW/glfw3.h>
#include "../Windows/Window.hpp"

namespace drk::UserInterfaces {
	class UserInterface {
	protected:
		const Windows::Window& window;
		bool isVisible;

	public:
		UserInterface(const Windows::Window& window) : window(window), isVisible(false) {}
		void OnKeyboardEvent(int key, int scancode, int action, int mods) {
			switch (key) {
				case (GLFW_KEY_LEFT_ALT): {
					if (action == GLFW_PRESS) {
						isVisible = !isVisible;
						int cursorMode = isVisible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED;
						glfwSetInputMode(window.GetWindow(), GLFW_CURSOR, cursorMode);
					}
					break;
				}
			}
		}
		void OnCursorPositionEvent(double xpos, double ypos) {}
		void OnMouseButtonEvent(int button, int action, int mods) {}
		bool IsVisible() const { return isVisible; }
	};
}