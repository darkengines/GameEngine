#pragma once
#include <GLFW/glfw3.h>

#include <boost/di.hpp>
#include <memory>
#include <taskflow/taskflow.hpp>

#include "../Animations/Systems/AnimationSystem.hpp"
#include "../Animations/Systems/BoneMeshSystem.hpp"
#include "../Animations/Systems/BoneSpatialSystem.hpp"
#include "../BoundingVolumes/Systems/AxisAlignedBoundingBoxSystem.hpp"
#include "../Cameras/Systems/CameraSystem.hpp"
#include "../Controllers/FlyCamController.hpp"
#include "../Devices/DeviceContext.hpp"
#include "../Engine/EngineState.hpp"
#include "../Frustums/Systems/FrustumSystem.hpp"
#include "../Graphics/GlobalSystem.hpp"
#include "../Graphics/Graphics.hpp"
#include "../Lights/Systems/DirectionalLightSystem.hpp"
#include "../Lights/Systems/LightPerspectiveSystem.hpp"
#include "../Lights/Systems/LightSystem.hpp"
#include "../Lights/Systems/PointLightSystem.hpp"
#include "../Lights/Systems/SpotlightSystem.hpp"
#include "../Lines/Systems/LineSystem.hpp"
#include "../Loaders/AssimpLoader.hpp"
#include "../Materials/Systems/MaterialSystem.hpp"
#include "../Meshes/Systems/MeshShadowSystem.hpp"
#include "../Meshes/Systems/MeshSystem.hpp"
#include "../Nodes/Systems/NodeSystem.hpp"
#include "../Points/Systems/PointSystem.hpp"
#include "../Scenes/Renderers/SceneRenderer.hpp"
#include "../Scenes/Systems/SceneSystem.hpp"
#include "../Spatials/Systems/RelativeSpatialSystem.hpp"
#include "../Spatials/Systems/SpatialSystem.hpp"
#include "../Systems/IStorageSystem.hpp"
#include "../Textures/Systems/TextureSystem.hpp"
#include "../UserInterfaces/AssetExplorer.hpp"
#include "../UserInterfaces/Renderers/UserInterfaceRenderer.hpp"
#include "../UserInterfaces/UserInterface.hpp"
#include "../Windows/Window.hpp"
#include "ApplicationState.hpp"

namespace drk::Applications {

class Application {
public:
	using boost_di_inject__ = boost::di::inject<
		const Windows::Window&,
		Engine::EngineState&,
		const Devices::DeviceContext&,
		Textures::Systems::TextureSystem&,
		Materials::Systems::MaterialSystem&,
		Meshes::Systems::MeshSystem&,
		Meshes::Systems::MeshShadowSystem&,
		Spatials::Systems::SpatialSystem&,
		Spatials::Systems::RelativeSpatialSystem&,
		Nodes::Systems::NodeSystem&,
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
		BoundingVolumes::Systems::AxisAlignedBoundingBoxSystem&,
		Frustums::Systems::FrustumSystem&,
		Lines::Systems::LineSystem&,
		Lights::Systems::LightSystem&,
		Lights::Systems::PointLightSystem&,
		Lights::Systems::DirectionalLightSystem&,
		Lights::Systems::SpotlightSystem&,
		Lights::Systems::LightPerspectiveSystem&,
		Animations::Systems::AnimationSystem&,
		Animations::Systems::BoneMeshSystem&,
		Animations::Systems::BoneSpatialSystem&,
		UserInterfaces::AssetExplorer&>;

	Application(
		const Windows::Window& window,
		Engine::EngineState& engineState,
		const Devices::DeviceContext& deviceContext,
		Textures::Systems::TextureSystem& textureSystem,
		Materials::Systems::MaterialSystem& materialSystem,
		Meshes::Systems::MeshSystem& meshSystem,
		Meshes::Systems::MeshShadowSystem& meshShadowSystem,
		Spatials::Systems::SpatialSystem& spatialSystem,
		Spatials::Systems::RelativeSpatialSystem& relativeSpatialSystem,
		Nodes::Systems::NodeSystem& objectSystem,
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
		BoundingVolumes::Systems::AxisAlignedBoundingBoxSystem& axisAlignedBoundingBoxSystem,
		Frustums::Systems::FrustumSystem& frustumSystem,
		Lines::Systems::LineSystem&,
		Lights::Systems::LightSystem& lightSystem,
		Lights::Systems::PointLightSystem& pointLightSystem,
		Lights::Systems::DirectionalLightSystem& directionalLightSystem,
		Lights::Systems::SpotlightSystem& spotlightSystem,
		Lights::Systems::LightPerspectiveSystem& lightPerspectiveSystem,
		Animations::Systems::AnimationSystem& animationSystem,
		Animations::Systems::BoneMeshSystem& boneSystem,
		Animations::Systems::BoneSpatialSystem& boneSpatialSystem,
		UserInterfaces::AssetExplorer& assetExplorer
	);

	void run();
	ApplicationState applicationState;
	static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);

protected:
	const Windows::Window& window;
	Engine::EngineState& engineState;
	const Devices::DeviceContext& deviceContext;
	Textures::Systems::TextureSystem& textureSystem;
	Materials::Systems::MaterialSystem& materialSystem;
	Meshes::Systems::MeshSystem& meshSystem;
	Meshes::Systems::MeshShadowSystem& meshShadowSystem;
	Spatials::Systems::SpatialSystem& spatialSystem;
	Spatials::Systems::RelativeSpatialSystem& relativeSpatialSystem;
	Nodes::Systems::NodeSystem& objectSystem;
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
	BoundingVolumes::Systems::AxisAlignedBoundingBoxSystem& axisAlignedBoundingBoxSystem;
	Frustums::Systems::FrustumSystem& frustumSystem;
	Lines::Systems::LineSystem& lineSystem;
	Lights::Systems::LightSystem& lightSystem;
	Lights::Systems::PointLightSystem& pointLightSystem;
	Lights::Systems::DirectionalLightSystem& directionalLightSystem;
	Lights::Systems::SpotlightSystem& spotlightSystem;
	Lights::Systems::LightPerspectiveSystem& lightPerspectiveSystem;
	Animations::Systems::AnimationSystem& animationSystem;
	Animations::Systems::BoneMeshSystem& boneSystem;
	Animations::Systems::BoneSpatialSystem& boneSpatialSystem;
	entt::entity selectedEntity = entt::null;
	UserInterfaces::AssetExplorer& assetExplorer;

	void OnWindowSizeChanged(uint32_t width, uint32_t height);
	void renderProperties(entt::entity entity);
	void renderStorageBuffers();
	bool shouldRecreateSwapchain = false;
	void RecreateSwapchain(vk::Extent2D windowExtent);
	vk::Extent2D windowExtent;
	void renderEntity(const entt::entity entity);
	void renderEntities();
	void renderAnimations();
	void renderSystemInfos();
	void renderGui(ApplicationState& applicationState);
	void taskflow();
};
}  // namespace drk::Applications
