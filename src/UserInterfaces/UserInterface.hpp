#pragma once
#include <GLFW/glfw3.h>
#include "../Windows/Window.hpp"
#include "../Controllers/FlyCamController.hpp"
#include <glm/glm.hpp>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>

namespace drk::UserInterfaces {
	class UserInterface {
	protected:
		const Windows::Window& window;
		bool isVisible;
		bool isExplorationMode = false;
		Controllers::FlyCamController& flyCamController;

	public:
		UserInterface(Windows::Window& window, Controllers::FlyCamController& flyCamController)
			: window(window), isVisible(false), flyCamController(flyCamController) {}
		void HandleKeyboardEvents() {
			if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_LeftAlt)) {
				int cursorMode;
				isExplorationMode = !isExplorationMode;
				if (isExplorationMode) {
					isVisible = false;
					//cursorMode = ImGuiMouseCursor_None;
					cursorMode = GLFW_CURSOR_DISABLED;
					glm::vec<2, double> cursorPosition;
					glfwGetCursorPos(window.GetWindow(), &cursorPosition.x, &cursorPosition.y);
					flyCamController.LastMousePosition = cursorPosition;
					flyCamController.MousePosition = cursorPosition;
				} else {
					isVisible = true;
					//cursorMode = ImGuiMouseCursor_Arrow;
					cursorMode = GLFW_CURSOR_NORMAL;
					flyCamController.LastMousePosition = std::nullopt;
					flyCamController.MousePosition = std::nullopt;
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
