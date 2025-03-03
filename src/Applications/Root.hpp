#pragma once
#include <GLFW/glfw3.h>

#include <boost/di.hpp>
#include <memory>
#include <semaphore>
#include <taskflow/taskflow.hpp>

#include "../Animations/Systems/AnimationSystem.hpp"
#include "../Animations/Systems/BoneMeshSystem.hpp"
#include "../Animations/Systems/BoneSpatialSystem.hpp"
#include "../BoundingVolumes/Systems/AxisAlignedBoundingBoxSystem.hpp"
#include "../Cameras/Systems/CameraSystem.hpp"
#include "../Controllers/FlyCamController.hpp"
#include "../Devices/DeviceContext.hpp"
#include "../Engine/EngineState.hpp"
#include "../Editors/Components/Selected.hpp"
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
#include <imgui_internal.h>

namespace drk::Applications
{
  class Root
  {
   public:
    Root(const Windows::Window& window,
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
        Lines::Systems::LineSystem& lineSystem,
        Lights::Systems::LightSystem& lightSystem,
        Lights::Systems::PointLightSystem& pointLightSystem,
        Lights::Systems::DirectionalLightSystem& directionalLightSystem,
        Lights::Systems::SpotlightSystem& spotlightSystem,
        Lights::Systems::LightPerspectiveSystem& lightPerspectiveSystem,
        Animations::Systems::AnimationSystem& animationSystem,
        Animations::Systems::BoneMeshSystem& boneSystem,
        Animations::Systems::BoneSpatialSystem& boneSpatialSystem
        // UserInterfaces::AssetExplorer& assetExplorer
    );

   public:
    void onWindowSizeChanged(uint32_t width, uint32_t height);
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    void recreateSwapchain(vk::Extent2D windowExtent);
    //void renderSceneGui(std::optional<Devices::Texture> sceneTexture, const std::optional<ImVec2>& sceneCursorPosition);
    void renderGui(ApplicationState& applicationState, std::optional<ImVec2>& sceneCursorPosition);
    void updateApplicationState(std::optional<Devices::Texture>& sceneTexture,
        vk::ResultValue<uint32_t>& swapchainImageAcquisitionResult,
        Engine::FrameState const*& frameStatePtr);
    void renderEntities();
    void renderEntity(const entt::entity entity);
    void renderProperties(entt::entity entity);
    void renderAnimations();
    void renderSystemInfos();
    void renderStorageBuffers();
    void run();
    ApplicationState applicationState;

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
    std::binary_semaphore mainToWorkflow{ 0 };
    std::binary_semaphore workflowToMain{ 0 };
    std::binary_semaphore glfwTaskflowToMainSemaphore{ 0 };
    std::binary_semaphore glfwmainToTaskflowSemaphore{ 0 };
    // UserInterfaces::AssetExplorer& assetExplorer;

    entt::entity selectedEntity = entt::null;
    bool shouldRecreateSwapchain = false;
    vk::Extent2D windowExtent;
  };
}  // namespace drk::Applications
