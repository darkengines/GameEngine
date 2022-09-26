#include "implemantations.hpp"
#include "Devices/Device.hpp"
#include "Windows/Window.hpp"
#include "Graphics/Graphics.hpp"
#include <format>
#include <GLFW/glfw3.h>

int main(int argc, char **argv) {

	const auto window = drk::Windows::Window::createWindow();
	const auto glfwExtensions = drk::Windows::Window::getVulkanInstanceExtension();

	std::vector<const char *> requiredInstanceExtensions(glfwExtensions.begin(), glfwExtensions.end());
	requiredInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	requiredInstanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

	const std::vector<const char *> requiredValidationLayers = {"VK_LAYER_KHRONOS_validation"};

	drk::Devices::DeviceContext deviceContext(requiredInstanceExtensions, drk::Graphics::Graphics::RequiredDeviceExtensions, requiredValidationLayers, [&window](const vk::Instance& instance) {
		vk::SurfaceKHR surface;
		glfwCreateWindowSurface((VkInstance) instance, window, nullptr, (VkSurfaceKHR *) &surface);
		return surface;
	}, true);

	vk::Extent2D actualExtent;
	glfwGetWindowSize(window, (int *) &actualExtent.width, (int *) &actualExtent.height);

	drk::Graphics::Graphics graphics(
		deviceContext,
		actualExtent
	);

	return 0;
}