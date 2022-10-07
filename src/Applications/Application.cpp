#include "Application.hpp"
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <iostream>

namespace drk::Applications {
	Application::Application()
		: Window(BuildWindow()),
		  DeviceContext(BuildDeviceContext(Window.get())),
		  EngineState(Application::BuildEngineState(DeviceContext.get())),
		  TextureSystem(std::make_unique<Textures::TextureSystem>(DeviceContext.get(), EngineState.get())),
		  MaterialSystem(std::make_unique<Materials::MaterialSystem>(DeviceContext.get(), EngineState.get())),
		  MeshSystem(std::make_unique<Meshes::MeshSystem>(DeviceContext.get(), EngineState.get())),
		  Loader(std::make_unique<Loaders::AssimpLoader>(EngineState.get())),
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
				auto result = glfwCreateWindowSurface(
					(VkInstance) instance,
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
		std::cout << "Load" << std::endl;
		auto data = Loader->Load("H:/models/hilbre-island/source/hilbre.glb");
		std::cout << "Loaded" << std::endl;
		TextureSystem->UploadTextures();
		MaterialSystem->UpdateMaterials();
		MeshSystem->UploadMeshes();

		while (!glfwWindowShouldClose(Window.get())) {
			glfwPollEvents();

			ImGui_ImplVulkan_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			ImGui::ShowDemoWindow();
			ImGui::EndFrame();

			Graphics->Render();
		}

		WaitFences();
	}

	std::unique_ptr<Graphics::EngineState> Application::BuildEngineState(const Devices::DeviceContext *deviceContext) {
		return std::make_unique<Graphics::EngineState>(deviceContext);
	}

	void Application::OnWindowSizeChanged(uint32_t width, uint32_t height) {
		WaitFences();
		Graphics->SetExtent({width, height});
	}

	void Application::WaitFences() {
		std::vector<vk::Fence> fences;
		for (const auto &frameState: EngineState->FrameStates) {
			fences.push_back(frameState.Fence);
		}
		for (const auto &frameState: EngineState->FrameStates) {
			fences.push_back(frameState.Fence);
		}
		auto waitFenceResult = DeviceContext->Device.waitForFences(
			fences,
			VK_TRUE,
			UINT64_MAX
		);
	}
}