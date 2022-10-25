#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include "Window.hpp"

namespace drk::Windows {
	const char *APPLICATION_NAME = "DarkEngines";
	const uint32_t SCREEN_WIDTH = 1920u;
	const uint32_t SCREEN_HEIGHT = 1080u;

	GLFWwindow *Window::createWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		const auto window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, APPLICATION_NAME, nullptr, nullptr);

		ImGui::CreateContext();
		auto& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		ImGui_ImplGlfw_InitForVulkan(window, true);
		return window;
	}

	std::vector<const char *> Window::getVulkanInstanceExtension() {
		uint32_t glfwExtensionCount = 0;
		auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		return std::vector<const char *>(glfwExtensions, glfwExtensions + glfwExtensionCount);
	}
}