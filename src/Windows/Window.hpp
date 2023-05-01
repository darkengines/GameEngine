#pragma once
#include <cstdint>
#include <GLFW/glfw3.h>
#include <vector>
#include "WindowConfiguration.hpp"
#include "../Configuration/Extensions.hpp"
#include <nlohmann/json.hpp>
#include <nameof.hpp>
#include <vulkan/vulkan.hpp>

namespace drk::Windows {

	void to_json(nlohmann::json& j, const WindowConfiguration& p);

	void from_json(const nlohmann::json& j, WindowConfiguration& p);

	class Window {
	public:
		Window(const Configuration::Configuration& configuration)
			: windowConfiguration(
			configuration.jsonConfiguration[std::string(nameof::nameof_short_type<Window>()).c_str()].
				get<Windows::WindowConfiguration>()
		),
			  window(createWindow(windowConfiguration)) {}
		~Window() {
			glfwDestroyWindow(window);
		}
		GLFWwindow* GetWindow() const { return window; };
		vk::Extent2D GetExtent() const {
			vk::Extent2D actualExtent;
			glfwGetFramebufferSize(window, (int*) &actualExtent.width, (int*) &actualExtent.height);
			return actualExtent;
		};

		static GLFWwindow* createWindow(const Windows::WindowConfiguration& configuration);
		static std::vector<const char*> getVulkanInstanceExtension();
	protected:
		Windows::WindowConfiguration windowConfiguration;
		GLFWwindow* window;
	};
}