#include "Application.hpp"
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

namespace drk::Applications {
	Application::Application()
		: Window(BuildWindow()),
		  DeviceContext(BuildDeviceContext(Window.get())),
		  EngineState(Application::BuildEngineState(DeviceContext.get())),
		  Graphics(BuildGraphics(Window.get(), DeviceContext.get(), EngineState.get())) {
		glfwSetWindowUserPointer(Window.get(), this);
		glfwSetWindowSizeCallback(
			Window.get(), [](GLFWwindow *window, int width, int height) {
				auto application = reinterpret_cast<Application *>(glfwGetWindowUserPointer(window));
				application->OnWindowSizeChanged(width, height);
			}
		);
		ImGui_ImplGlfw_InitForVulkan(Window.get(), true);
	}

	std::unique_ptr<GLFWwindow, void (*)(GLFWwindow *)> Application::BuildWindow() {
		const auto window = drk::Windows::Window::createWindow();
		return std::unique_ptr<GLFWwindow, void (*)(GLFWwindow *)>(window, glfwDestroyWindow);
	}

	std::unique_ptr<Devices::DeviceContext> Application::BuildDeviceContext(GLFWwindow *window) {

		const auto glfwExtensions = drk::Windows::Window::getVulkanInstanceExtension();

		std::vector<const char *> requiredInstanceExtensions(glfwExtensions.begin(), glfwExtensions.end());
		requiredInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		requiredInstanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

		const std::vector<const char *> requiredValidationLayers = {"VK_LAYER_KHRONOS_validation"};

		auto deviceContext = std::make_unique<Devices::DeviceContext>(
			requiredInstanceExtensions,
			drk::Graphics::Graphics::RequiredDeviceExtensions,
			requiredValidationLayers,
			[&window](const vk::Instance &instance) {
				vk::SurfaceKHR surface;
				auto result = glfwCreateWindowSurface((VkInstance) instance,
													  window,
													  nullptr,
													  (VkSurfaceKHR *) &surface
				);
				if (result != VK_SUCCESS) {
					throw "Failed to create surface.";
				}
				return surface;
			},
			true
		);
		return deviceContext;
	}

	std::unique_ptr<Graphics::Graphics>
	Application::BuildGraphics(
		GLFWwindow *window,
		const Devices::DeviceContext *deviceContext,
		Graphics::EngineState *engineState
	) {
		vk::Extent2D actualExtent;
		glfwGetWindowSize(window, (int *) &actualExtent.width, (int *) &actualExtent.height);

		auto graphics = std::make_unique<Graphics::Graphics>(
			deviceContext,
			engineState,
			actualExtent
		);

		return graphics;
	}

	void Application::Run() {
		while (!glfwWindowShouldClose(Window.get())) {
			glfwPollEvents();

			ImGui_ImplVulkan_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			ImGui::ShowDemoWindow();
			ImGui::EndFrame();

			Graphics->Render();
		}
	}

	std::unique_ptr<Graphics::EngineState> Application::BuildEngineState(const Devices::DeviceContext *deviceContext) {
		return std::make_unique<Graphics::EngineState>(deviceContext);
	}
}