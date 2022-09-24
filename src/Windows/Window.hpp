#pragma once
#include <cstdint>
#include <GLFW/glfw3.h>
#include <vector>

namespace drk::Windows {
	class Window {
	public:
		static GLFWwindow* createWindow();
		static std::vector<const char*> getVulkanInstanceExtension();
	};
}