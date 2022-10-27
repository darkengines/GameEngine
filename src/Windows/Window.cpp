#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include "Window.hpp"
#include "../Configuration/Extensions.hpp"
#include "WindowConfiguration.hpp"

namespace drk::Windows {

	const char* APPLICATION_NAME = "DarkEngines";
	const uint32_t SCREEN_WIDTH = 1920u;
	const uint32_t SCREEN_HEIGHT = 1080u;

	GLFWwindow* Window::createWindow(const WindowConfiguration& configuration) {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		const auto window = glfwCreateWindow(
			configuration.Width,
			configuration.Height,
			configuration.Title.c_str(),
			nullptr,
			nullptr
		);

		ImGui::CreateContext();
		auto& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		ImGui_ImplGlfw_InitForVulkan(window, true);
		return window;
	}

	std::vector<const char*> Window::getVulkanInstanceExtension() {
		uint32_t glfwExtensionCount = 0;
		auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		return std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);
	}
	void to_json(nlohmann::json& j, const WindowConfiguration& p) {
		j = nlohmann::json{{NAMEOF(p.Title).c_str(), p.Title}};
		j = nlohmann::json{{NAMEOF(p.Width).c_str(), p.Width}};
		j = nlohmann::json{{NAMEOF(p.Height).c_str(), p.Height}};
	}
	void from_json(const nlohmann::json& j, WindowConfiguration& p) {
		j.at(NAMEOF(p.Title).c_str()).get_to(p.Title);
		j.at(NAMEOF(p.Width).c_str()).get_to(p.Width);
		j.at(NAMEOF(p.Height).c_str()).get_to(p.Height);
	}
}