#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "../Loaders/AssimpLoader.hpp"
#include "../Devices/DeviceContext.hpp"
#include "../Lights/PointLightSystem.hpp"
#include "../Lights/SpotlightSystem.hpp"
#include "../Lights/DirectionalLightSystem.hpp"
#include "../Graphics/Graphics.hpp"
#include "../Windows/Window.hpp"
#include "../Engine/EngineState.hpp"
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
#include "../Scenes/Renderers/SceneRenderer.hpp"
#include "../UserInterfaces/Renderers/UserInterfaceRenderer.hpp"
#include "../Scenes/SceneSystem.hpp"
#include "../Points/PointSystem.hpp"
#include "../Lines/LineSystem.hpp"
#include "../UserInterfaces/AssetExplorer.hpp"
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
			UserInterfaces::Renderers::UserInterfaceRenderer&,
			Scenes::Renderers::SceneRenderer&,
			Scenes::SceneSystem&,
			Points::PointSystem&,
			Lines::LineSystem&,
			Lights::PointLightSystem&,
			Lights::DirectionalLightSystem&,
			Lights::SpotlightSystem&,
			UserInterfaces::AssetExplorer&
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
			UserInterfaces::Renderers::UserInterfaceRenderer& userInterfaceRenderer,
			Scenes::Renderers::SceneRenderer& sceneRenderer,
			Scenes::SceneSystem& sceneSystem,
			Points::PointSystem& pointSystem,
			Lines::LineSystem&,
			Lights::PointLightSystem& pointLightSystem,
			Lights::DirectionalLightSystem& directionalLightSystem,
			Lights::SpotlightSystem& spotlightSystem,
			UserInterfaces::AssetExplorer& assetExplorer
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
		Controllers::FlyCamController& flyCamController;
		UserInterfaces::UserInterface& userInterface;
		entt::registry& registry;
		UserInterfaces::Renderers::UserInterfaceRenderer& userInterfaceRenderer;
		Scenes::Renderers::SceneRenderer& sceneRenderer;
		Scenes::SceneSystem& sceneSystem;
		Points::PointSystem& pointSystem;
		Lines::LineSystem& lineSystem;
		Lights::PointLightSystem& pointLightSystem;
		Lights::DirectionalLightSystem& directionalLightSystem;
		Lights::SpotlightSystem& spotlightSystem;
		entt::entity selectedEntity = entt::null;
		UserInterfaces::AssetExplorer& assetExplorer;

		void OnWindowSizeChanged(uint32_t width, uint32_t height);
		void WaitFences();
		static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
		static void SetKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		void RenderEntityTree(entt::entity);
		void renderProperties(entt::entity entity);
		void renderStorageBuffers();
		bool shouldRecreateSwapchain = false;
		void RecreateSwapchain(vk::Extent2D windowExtent);
		vk::Extent2D windowExtent;
		void renderEntity(entt::entity entity);
		void renderEntities();
	};
}