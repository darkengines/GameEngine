#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "../Loaders/AssimpLoader.hpp"
#include "../Devices/DeviceContext.hpp"
#include "../Lights/Systems/LightSystem.hpp"
#include "../Lights/Systems/PointLightSystem.hpp"
#include "../Lights/Systems/SpotlightSystem.hpp"
#include "../Lights/Systems/DirectionalLightSystem.hpp"
#include "../Lights/Systems/LightPerspectiveSystem.hpp"
#include "../Graphics/Graphics.hpp"
#include "../Windows/Window.hpp"
#include "../Engine/EngineState.hpp"
#include "../Textures/Systems/TextureSystem.hpp"
#include "../Meshes/Systems/MeshSystem.hpp"
#include "../Materials/Systems/MaterialSystem.hpp"
#include "../Spatials/Systems/SpatialSystem.hpp"
#include "../Objects/Systems/ObjectSystem.hpp"
#include "../Cameras/Systems/CameraSystem.hpp"
#include "../Graphics/GlobalSystem.hpp"
#include "../Controllers/FlyCamController.hpp"
#include "../UserInterfaces/UserInterface.hpp"
#include "../Scenes/Renderers/SceneRenderer.hpp"
#include "../UserInterfaces/Renderers/UserInterfaceRenderer.hpp"
#include "../Scenes/Systems/SceneSystem.hpp"
#include "../Points/Systems/PointSystem.hpp"
#include "../Lines/Systems/LineSystem.hpp"
#include "../UserInterfaces/AssetExplorer.hpp"
#include <memory>

namespace drk::Applications {
	class Application {
	public:
		using boost_di_inject__ = boost::di::inject <
			const Windows::Window&,
			const Devices::DeviceContext&,
			Engine::EngineState&,
			Textures::Systems::TextureSystem&,
			Materials::Systems::MaterialSystem&,
			Meshes::Systems::MeshSystem&,
			Spatials::Systems::SpatialSystem&,
			Objects::Systems::ObjectSystem&,
			Cameras::Systems::CameraSystem&,
			Graphics::GlobalSystem&,
			const Loaders::AssimpLoader&,
			Graphics::Graphics&,
			Controllers::FlyCamController&,
			UserInterfaces::UserInterface&,
			entt::registry&,
			UserInterfaces::Renderers::UserInterfaceRenderer&,
			Scenes::Renderers::SceneRenderer&,
			Scenes::Systems::SceneSystem&,
			Points::Systems::PointSystem&,
			Lines::Systems::LineSystem&,
			Lights::Systems::LightSystem&,
			Lights::Systems::PointLightSystem&,
			Lights::Systems::DirectionalLightSystem&,
			Lights::Systems::SpotlightSystem&,
			Lights::Systems::LightPerspectiveSystem&,
			UserInterfaces::AssetExplorer&
		> ;

		Application(
			const Windows::Window& window,
			const Devices::DeviceContext& deviceContext,
			Engine::EngineState& engineState,
			Textures::Systems::TextureSystem& textureSystem,
			Materials::Systems::MaterialSystem& materialSystem,
			Meshes::Systems::MeshSystem& meshSystem,
			Spatials::Systems::SpatialSystem& spatialSystem,
			Objects::Systems::ObjectSystem& objectSystem,
			Cameras::Systems::CameraSystem& cameraSystem,
			Graphics::GlobalSystem& globalSystem,
			const Loaders::AssimpLoader& loader,
			Graphics::Graphics& graphics,
			Controllers::FlyCamController& flyCamController,
			UserInterfaces::UserInterface& userInterface,
			entt::registry& registry,
			UserInterfaces::Renderers::UserInterfaceRenderer& userInterfaceRenderer,
			Scenes::Renderers::SceneRenderer& sceneRenderer,
			Scenes::Systems::SceneSystem& sceneSystem,
			Points::Systems::PointSystem& pointSystem,
			Lines::Systems::LineSystem&,
			Lights::Systems::LightSystem& lightSystem,
			Lights::Systems::PointLightSystem& pointLightSystem,
			Lights::Systems::DirectionalLightSystem& directionalLightSystem,
			Lights::Systems::SpotlightSystem& spotlightSystem,
			Lights::Systems::LightPerspectiveSystem& lightPerspectiveSystem,
			UserInterfaces::AssetExplorer& assetExplorer
		);
		~Application();

		void Run();

	protected:
		const Windows::Window& window;
		const Devices::DeviceContext& deviceContext;
		Engine::EngineState& engineState;
		Textures::Systems::TextureSystem& textureSystem;
		Materials::Systems::MaterialSystem& materialSystem;
		Meshes::Systems::MeshSystem& meshSystem;
		Spatials::Systems::SpatialSystem& spatialSystem;
		Objects::Systems::ObjectSystem& objectSystem;
		Cameras::Systems::CameraSystem& cameraSystem;
		Graphics::GlobalSystem& globalSystem;
		const Loaders::AssimpLoader& loader;
		Graphics::Graphics& graphics;
		Controllers::FlyCamController& flyCamController;
		UserInterfaces::UserInterface& userInterface;
		entt::registry& registry;
		UserInterfaces::Renderers::UserInterfaceRenderer& userInterfaceRenderer;
		Scenes::Renderers::SceneRenderer& sceneRenderer;
		Scenes::Systems::SceneSystem& sceneSystem;
		Points::Systems::PointSystem& pointSystem;
		Lines::Systems::LineSystem& lineSystem;
		Lights::Systems::LightSystem& lightSystem;
		Lights::Systems::PointLightSystem& pointLightSystem;
		Lights::Systems::DirectionalLightSystem& directionalLightSystem;
		Lights::Systems::SpotlightSystem& spotlightSystem;
		Lights::Systems::LightPerspectiveSystem& lightPerspectiveSystem;
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