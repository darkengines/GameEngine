#include "implemantations.hpp"
#include "Devices/Device.hpp"
#include "Windows/Window.hpp"
#include <format>

int main(int argc, char **argv) {

	const auto window = drk::Windows::Window::createWindow();
	const auto glfwExtensions = drk::Windows::Window::getVulkanInstanceExtension();

	std::vector<const char *> requiredInstanceExtensions(glfwExtensions.begin(), glfwExtensions.end());
	requiredInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	requiredInstanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

	std::vector<const char *> requiredDeviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
		VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME
	};

	const std::vector<const char *> requiredValidationLayers = {"VK_LAYER_KHRONOS_validation"};

	auto device = drk::Devices::Device::createContext(
		window,
		requiredInstanceExtensions,
		requiredDeviceExtensions,
		requiredValidationLayers
	);

	return 0;
}