#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "../Devices/DeviceContext.hpp"
#include "../Graphics/Graphics.hpp"
#include "../Windows/Window.hpp"
#include "../Engine/EngineState.hpp"
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
#include "../Graphics/MeshPipeline.hpp"
#include <memory>

namespace drk::Applications {
	class Application {
	public:
		using boost_di_inject__ = boost::di::inject<
			const Windows::Window&,
			const Devices::DeviceContext&,
			Engine::EngineState&,
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
			UserInterfaces::UserInterface&,
			entt::registry&,
			Graphics::MeshPipeline&
		>;

		Application(
			const Windows::Window& window,
			const Devices::DeviceContext& deviceContext,
			Engine::EngineState& engineState,
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
			UserInterfaces::UserInterface& userInterface,
			entt::registry& registry,
			Graphics::MeshPipeline& mainRenderContext
		);
		~Application();

		void Run();

	protected:
		const Windows::Window& window;
		const Devices::DeviceContext& deviceContext;
		Engine::EngineState& engineState;
		Textures::TextureSystem& textureSystem;
		Materials::MaterialSystem& materialSystem;
		Meshes::MeshSystem& meshSystem;
		Spatials::SpatialSystem& spatialSystem;
		Objects::ObjectSystem& objectSystem;
		Cameras::CameraSystem& cameraSystem;
		Graphics::GlobalSystem& globalSystem;
		const Loaders::AssimpLoader& loader;
		Graphics::Graphics& graphics;
		Graphics::MeshPipeline mainRenderContext;
		Controllers::FlyCamController& flyCamController;
		UserInterfaces::UserInterface& userInterface;
		entt::registry& registry;

		void OnWindowSizeChanged(uint32_t width, uint32_t height);
		void WaitFences();
		static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
		static void SetKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		void RenderEntityTree(entt::entity);
		void renderProperties(entt::entity entity);
	};
}