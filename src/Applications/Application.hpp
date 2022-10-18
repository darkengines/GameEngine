#pragma once
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include "../Devices/DeviceContext.hpp"
#include "../Graphics/Graphics.hpp"
#include "../Windows/Window.hpp"
#include "../Graphics/EngineState.hpp"
#include "../Loaders/AssimpLoader.hpp"
#include "../Textures/TextureSystem.hpp"
#include "../Meshes/MeshSystem.hpp"
#include "../Materials/MaterialSystem.hpp"
#include "../Spatials/SpatialSystem.hpp"
#include "../Objects/ObjectSystem.hpp"
#include "../Cameras/CameraSystem.hpp"
#include "../Graphics/GlobalSystem.hpp"
#include <memory>

namespace drk::Applications {
	class Application {
	public:
		Application();

		void Run();

	protected:
		std::unique_ptr<GLFWwindow, void (*)(GLFWwindow *)> Window;
		const std::unique_ptr<Devices::DeviceContext> DeviceContext;
		const std::unique_ptr<Graphics::EngineState> EngineState;
		const std::unique_ptr<Textures::TextureSystem> TextureSystem;
		const std::unique_ptr<Materials::MaterialSystem> MaterialSystem;
		const std::unique_ptr<Meshes::MeshSystem> MeshSystem;
		const std::unique_ptr<Spatials::SpatialSystem> SpatialSystem;
		const std::unique_ptr<Objects::ObjectSystem> ObjectSystem;
		const std::unique_ptr<Cameras::CameraSystem> CameraSystem;
		const std::unique_ptr<Graphics::GlobalSystem> GlobalSystem;
		const std::unique_ptr<Loaders::AssimpLoader> Loader;
		const std::unique_ptr<Graphics::Graphics> Graphics;

		static std::unique_ptr<GLFWwindow, void (*)(GLFWwindow *)> BuildWindow();
		static std::unique_ptr<Devices::DeviceContext> BuildDeviceContext(GLFWwindow *window);
		static std::unique_ptr<Graphics::Graphics>
		BuildGraphics(
			GLFWwindow *window,
			const Devices::DeviceContext *deviceContext,
			Graphics::EngineState *engineState
		);
		static std::unique_ptr<Graphics::EngineState>
		BuildEngineState(const Devices::DeviceContext *deviceContext);

		void OnWindowSizeChanged(uint32_t width, uint32_t height);
		void WaitFences();
	};
}