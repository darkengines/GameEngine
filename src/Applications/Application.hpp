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
#include "../Controllers/FlyCamController.hpp"
#include "../UserInterfaces/UserInterface.hpp"
#include "../Graphics/MainRenderContext.hpp"
#include <memory>

namespace drk::Applications {
	class Application {
	public:
		using boost_di_inject__ = boost::di::inject<
			const Windows::Window&,
			const Devices::DeviceContext&,
			const Graphics::EngineState&,
			Textures::TextureSystem&,
			Materials::MaterialSystem&,
			Meshes::MeshSystem&,
			Spatials::SpatialSystem&,
			Objects::ObjectSystem&,
			Cameras::CameraSystem&,
			Graphics::GlobalSystem&,
			const Loaders::AssimpLoader&,
			Graphics::Graphics&,
			Controllers::FlyCamController&,
			UserInterfaces::UserInterface&
		>;

		Application(
			const Windows::Window& window,
			const Devices::DeviceContext& deviceContext,
			const Graphics::EngineState& engineState,
			Textures::TextureSystem& textureSystem,
			Materials::MaterialSystem& materialSystem,
			Meshes::MeshSystem& meshSystem,
			Spatials::SpatialSystem& spatialSystem,
			Objects::ObjectSystem& objectSystem,
			Cameras::CameraSystem& cameraSystem,
			Graphics::GlobalSystem& globalSystem,
			const Loaders::AssimpLoader& loader,
			Graphics::Graphics& graphics,
			Controllers::FlyCamController& flyCamController,
			UserInterfaces::UserInterface& userInterface
		);
		~Application();

		void Run();

	protected:
		const Windows::Window& Window;
		const Devices::DeviceContext& DeviceContext;
		const Graphics::EngineState& EngineState;
		Textures::TextureSystem& TextureSystem;
		Materials::MaterialSystem& MaterialSystem;
		Meshes::MeshSystem& MeshSystem;
		Spatials::SpatialSystem& SpatialSystem;
		Objects::ObjectSystem& ObjectSystem;
		Cameras::CameraSystem& CameraSystem;
		Graphics::GlobalSystem& GlobalSystem;
		const Loaders::AssimpLoader& Loader;
		Graphics::Graphics& Graphics;
		Graphics::MainRenderContext MainRenderContext;
		Controllers::FlyCamController& FlyCamController;
		UserInterfaces::UserInterface& UserInterface;

		void OnWindowSizeChanged(uint32_t width, uint32_t height);
		void WaitFences();
		static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
		static void SetKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		void RenderEntityTree(entt::entity);
	};
}