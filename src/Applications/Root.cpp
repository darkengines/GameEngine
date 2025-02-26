#include "Root.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <algorithm>
#include <entt/entt.hpp>
#include <stack>
#include <taskflow/taskflow.hpp>

#include "../Common/Components/Name.hpp"
#include "../GlmExtensions.hpp"
#include "../Lights/Components/LightPerspectiveCollection.hpp"

#undef near
#undef far

namespace drk::Applications
{

  Root::Root(const Windows::Window &window,
      Engine::EngineState &engineState,
      const Devices::DeviceContext &deviceContext,
      Textures::Systems::TextureSystem &textureSystem,
      Materials::Systems::MaterialSystem &materialSystem,
      Meshes::Systems::MeshSystem &meshSystem,
      Meshes::Systems::MeshShadowSystem &meshShadowSystem,
      Spatials::Systems::SpatialSystem &spatialSystem,
      Spatials::Systems::RelativeSpatialSystem &relativeSpatialSystem,
      Nodes::Systems::NodeSystem &objectSystem,
      Cameras::Systems::CameraSystem &cameraSystem,
      Graphics::GlobalSystem &globalSystem,
      const Loaders::AssimpLoader &loader,
      Graphics::Graphics &graphics,
      Controllers::FlyCamController &flyCamController,
      UserInterfaces::UserInterface &userInterface,
      entt::registry &registry,
      UserInterfaces::Renderers::UserInterfaceRenderer &userInterfaceRenderer,
      Scenes::Renderers::SceneRenderer &sceneRenderer,
      Scenes::Systems::SceneSystem &sceneSystem,
      Points::Systems::PointSystem &pointSystem,
      BoundingVolumes::Systems::AxisAlignedBoundingBoxSystem &axisAlignedBoundingBoxSystem,
      Frustums::Systems::FrustumSystem &frustumSystem,
      Lines::Systems::LineSystem &lineSystem,
      Lights::Systems::LightSystem &lightSystem,
      Lights::Systems::PointLightSystem &pointLightSystem,
      Lights::Systems::DirectionalLightSystem &directionalLightSystem,
      Lights::Systems::SpotlightSystem &spotlightSystem,
      Lights::Systems::LightPerspectiveSystem &lightPerspectiveSystem,
      Animations::Systems::AnimationSystem &animationSystem,
      Animations::Systems::BoneMeshSystem &boneSystem,
      Animations::Systems::BoneSpatialSystem &boneSpatialSystem)
      : window(window),
        engineState(engineState),
        deviceContext(deviceContext),
        registry(registry),
        textureSystem(textureSystem),
        materialSystem(materialSystem),
        meshSystem(meshSystem),
        meshShadowSystem(meshShadowSystem),
        spatialSystem(spatialSystem),
        relativeSpatialSystem(relativeSpatialSystem),
        objectSystem(objectSystem),
        cameraSystem(cameraSystem),
        globalSystem(globalSystem),
        loader(loader),
        graphics(graphics),
        flyCamController(flyCamController),
        userInterface(userInterface),
        userInterfaceRenderer(userInterfaceRenderer),
        sceneRenderer(sceneRenderer),
        sceneSystem(sceneSystem),
        pointSystem(pointSystem),
        axisAlignedBoundingBoxSystem(axisAlignedBoundingBoxSystem),
        frustumSystem(frustumSystem),
        lineSystem(lineSystem),
        lightSystem(lightSystem),
        pointLightSystem(pointLightSystem),
        directionalLightSystem(directionalLightSystem),
        spotlightSystem(spotlightSystem),
        lightPerspectiveSystem(lightPerspectiveSystem),
        animationSystem(animationSystem),
        boneSystem(boneSystem),
        boneSpatialSystem(boneSpatialSystem)
  {
    const auto &glfwWindow = window.GetWindow();
    glfwSetCursorPosCallback(glfwWindow, Root::cursorPosCallback);
    glfwSetWindowUserPointer(glfwWindow, this);
    glfwSetWindowSizeCallback(glfwWindow,
        [](GLFWwindow *window, int width, int height)
        {
          auto application = reinterpret_cast<Root *>(glfwGetWindowUserPointer(window));
          application->onWindowSizeChanged(width, height);
        });
    ImGui_ImplGlfw_InitForVulkan(glfwWindow, true);
    auto io = ImGui::GetIO();
    glfwMakeContextCurrent(glfwWindow);
  }

  void Root::onWindowSizeChanged(uint32_t width, uint32_t height)
  {
    while (width == 0 || height == 0)
    {
      glfwGetFramebufferSize(window.GetWindow(), reinterpret_cast<int *>(&width), reinterpret_cast<int *>(&height));
      glfwWaitEvents();
    }
    if (windowExtent.width != width || windowExtent.height != height)
    {
      windowExtent.width = width;
      windowExtent.height = height;
      shouldRecreateSwapchain = true;
    }
  }

  void Root::cursorPosCallback(GLFWwindow *window, double xpos, double ypos)
  {
    auto application = reinterpret_cast<Root *>(glfwGetWindowUserPointer(window));
    if (application->userInterface.IsExplorationMode())
    {
      application->flyCamController.OnCursorPositionEvent(xpos, ypos);
    }
  }

  void Root::recreateSwapchain(vk::Extent2D windowExtent)
  {
    graphics.RecreateSwapchain(windowExtent);
    const auto swapchain = graphics.GetSwapchain();
    userInterfaceRenderer.SetTargetImageViews(
        { .extent = swapchain.extent, .format = swapchain.imageFormat }, swapchain.imageViews);
    shouldRecreateSwapchain = false;
  }

  void Root::renderGui(ApplicationState &applicationState)
  {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    auto mainViewport = ImGui::GetMainViewport();
    auto mainViewportSize = mainViewport->Size;
    ImGui::SetNextWindowSize(mainViewportSize, ImGuiCond_FirstUseEver);
    ImGui::NewFrame();

    auto io = ImGui::GetIO();
    io.MouseDrawCursor = false;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

    if (userInterface.IsExplorationMode())
    {
      io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    }
    if (true || userInterface.IsVisible())
    {
      auto open = true;

      ImGui::DockSpaceOverViewport();
      if (ImGui::BeginMainMenuBar())
      {
        if (ImGui::BeginMenu("File"))
        {
          if (ImGui::MenuItem("Open", "ctrl + o"))
          {
            applicationState.fileBrowser.Open();
          }
          if (ImGui::MenuItem("Save", "ctrl + s"))
          {
          }
          if (ImGui::MenuItem("Close", "alt + f4"))
          {
          }
          ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Add"))
        {
          if (ImGui::BeginMenu("Light"))
          {
            if (ImGui::MenuItem("Spot"))
            {
            }
            if (ImGui::MenuItem("Point"))
            {
              auto pointLightEntity = pointLightSystem.createPointLight();
              selectedEntity = pointLightEntity;
            }
            if (ImGui::MenuItem("Directional"))
            {
            }

            ImGui::EndMenu();
          }
          ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Render"))
        {
          if (ImGui::BeginMenu("Mesh"))
          {
            if (ImGui::MenuItem("Material"))
            {
              globalSystem.setRenderStyle(1 << 4);
            }
            if (ImGui::MenuItem("UVs"))
            {
              globalSystem.setRenderStyle(1 << 3);
            }
            ImGui::EndMenu();
          }
          if (ImGui::BeginMenu("Texture"))
          {
            if (ImGui::MenuItem("Normal"))
            {
              globalSystem.setRenderStyle(1 << 2);
            }
            if (ImGui::MenuItem("Albedo"))
            {
              globalSystem.setRenderStyle(1 << 1);
            }
            if (ImGui::MenuItem("PBR"))
            {
              globalSystem.setRenderStyle(0);
            }

            ImGui::EndMenu();
          }
          ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("About"))
        {
          applicationState.isDemoWindowOpen = ImGui::MenuItem("Show demo window", "ctrl + d");
          ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
      }
      if (applicationState.isDemoWindowOpen)
        ImGui::ShowDemoWindow(&applicationState.isDemoWindowOpen);
      auto windowExtent = window.GetExtent();
      ImGui::SetNextWindowSize(ImVec2(static_cast<float>(windowExtent.width),
                                   static_cast<float>(windowExtent.height) - ImGui::GetTextLineHeightWithSpacing()),
          ImGuiCond_FirstUseEver);
      ImGui::Begin("Hello World!", &open, ImGuiWindowFlags_MenuBar);
      auto contentRegionAvailable = ImGui::GetContentRegionAvail();
      applicationState.actualExtent.width = contentRegionAvailable.x;
      applicationState.actualExtent.height = contentRegionAvailable.y;
      if (applicationState.sceneTextureImageDescriptorSet.has_value())
      {
        ImGui::Image((ImTextureID)(void *)applicationState.sceneTextureImageDescriptorSet.value(),
            { static_cast<float>(applicationState.actualExtent.width),
                static_cast<float>(applicationState.actualExtent.height) },
            { 0.0f, 0.0f },
            { (float)applicationState.actualExtent.width / applicationState.sceneExtent.width,
                (float)applicationState.actualExtent.height / applicationState.sceneExtent.height });
      }

      ImGui::End();
      ImGui::Begin("Entities");
      renderEntities();
      ImGui::End();

      renderProperties(selectedEntity);
      renderStorageBuffers();
      renderAnimations();
      renderSystemInfos();
      // renderInspector();
      // assetExplorer.render(registry);

      applicationState.fileBrowser.Display();
      if (applicationState.fileBrowser.HasSelected())
      {
        auto loadResult = loader.Load(applicationState.fileBrowser.GetSelected(), registry);
        applicationState.loadResults.emplace_back(std::move(loadResult));
        applicationState.fileBrowser.ClearSelected();
      }
    }
    ImGui::EndFrame();
  }

  void Root::updateApplicationState(std::optional<Devices::Texture> &sceneTexture,
      vk::ResultValue<uint32_t> &swapchainImageAcquisitionResult,
      Engine::FrameState const *&frameStatePtr)
  {
    auto newTime = std::chrono::high_resolution_clock::now();
    applicationState.frameTime =
        std::chrono::duration<float, std::chrono::seconds::period>(newTime - applicationState.currentTime).count();
    applicationState.currentTime = newTime;
    userInterface.HandleKeyboardEvents();
    if (userInterface.IsExplorationMode())
    {
      flyCamController.HandleKeyboardEvents();
    }

    auto imGuiMousePosition = ImGui::GetMousePos();
    glm::vec2 mousePosition{ imGuiMousePosition.x, imGuiMousePosition.y };
    if (!userInterface.IsExplorationMode())
    {
      cursorPosCallback(window.GetWindow(), mousePosition.x, mousePosition.y);
    }

    frameStatePtr = &engineState.getCurrentFrameState();

    const auto &fence = frameStatePtr->fence;
    const auto &waitForFenceResult = deviceContext.device.waitForFences(1, &fence, VK_TRUE, UINT64_MAX);
    const auto &resetFenceResult = deviceContext.device.resetFences(1, &fence);
    if (!shouldRecreateSwapchain)
      swapchainImageAcquisitionResult = graphics.AcquireSwapchainImageIndex();

    if (shouldRecreateSwapchain || swapchainImageAcquisitionResult.result == vk::Result::eSuboptimalKHR ||
        swapchainImageAcquisitionResult.result == vk::Result::eErrorOutOfDateKHR)
    {
      recreateSwapchain(windowExtent);
      swapchainImageAcquisitionResult = graphics.AcquireSwapchainImageIndex();
    }

    if (applicationState.sceneExtent.width != applicationState.actualExtent.width ||
        applicationState.sceneExtent.height != applicationState.actualExtent.height)
    {
      if ((applicationState.sceneExtent.width < applicationState.actualExtent.width &&
              applicationState.actualExtent.width < INT_MAX) ||
          (applicationState.sceneExtent.height < applicationState.actualExtent.height &&
              applicationState.actualExtent.height < INT_MAX))
      {
        if (applicationState.sceneTextureImageDescriptorSet.has_value())
          ImGui_ImplVulkan_RemoveTexture(applicationState.sceneTextureImageDescriptorSet.value());
        if (sceneTexture.has_value())
          deviceContext.destroyTexture(sceneTexture.value());
        sceneTexture = Scenes::Renderers::SceneRenderer::BuildSceneRenderTargetTexture(
            deviceContext, { applicationState.actualExtent.width, applicationState.actualExtent.height, 1 });
        applicationState.sceneTextureImageDescriptorSet =
            (vk::DescriptorSet)ImGui_ImplVulkan_AddTexture((VkSampler)engineState.GetDefaultTextureSampler(),
                (VkImageView)sceneTexture->imageView,
                static_cast<VkImageLayout>(vk::ImageLayout::eShaderReadOnlyOptimal));

        sceneRenderer.setTargetImageViews(
            {
                applicationState.actualExtent,
                vk::Format::eR8G8B8A8Srgb,
            },
            { sceneTexture->imageView });
        applicationState.sceneExtent =
            vk::Extent3D{ applicationState.actualExtent.width, applicationState.actualExtent.height, 1 };
      }
      else if (sceneRenderer.getUserExtent() != applicationState.actualExtent)
      {
        sceneRenderer.setTargetExtent(applicationState.actualExtent);
      }
    }
  }

  void Root::run()
  {
    auto defaultCamera = cameraSystem.createCamera({ 0.0f, 0.0f, 0.0f, 1.0f },
        glm::vec4{ 0.0f, 0.0f, -1.0f, 0.0f },
        glm::vec4{ 0.0f, 1.0f, 0.0f, 0.0f },
        glm::radians(65.0f),
        16.0f / 9.0f,
        0.1f,
        1000.0f);

    flyCamController.Attach(defaultCamera);
    globalSystem.setCamera(defaultCamera);

    const auto swapchain = graphics.GetSwapchain();
    userInterfaceRenderer.SetTargetImageViews(
        { .extent = { swapchain.extent.width, swapchain.extent.height, 1 }, .format = swapchain.imageFormat },
        swapchain.imageViews);

    std::optional<Devices::Texture> sceneTexture;
    glm::vec2 previousMousePosition{ 0, 0 };

    auto windowExtent = window.GetExtent();

    applicationState.currentTime = std::chrono::high_resolution_clock::now();

    tf::Taskflow rootTaskflow("drk");
    vk::ResultValue<uint32_t> swapchainImageAcquisitionResult(vk::Result::eSuccess, 0);
    Engine::FrameState const *frameStatePtr = nullptr;

    /*auto updateApplicationStateTaskflow =
        rootTaskflow.emplace([&]() { updateApplicationState(sceneTexture, swapchainImageAcquisitionResult, frameStatePtr); })
            .name("Application state update");*/

    // const auto &resetSynchronizationState =
    //     rootTaskflow.emplace([&]() { globalSystem.GlobalSynchronizationState.Reset(); }).name("Synchronization state
    //     reset").succeed(updateApplicationStateTaskflow);
    const auto &textureUpload = rootTaskflow.emplace([&]() { textureSystem.UploadTextures(); }).name("Texture upload");
    const auto &meshUpload =
        rootTaskflow.emplace([&]() { meshSystem.uploadMeshes(); }).succeed(textureUpload).name("Mesh upload");
    const auto &skinnedMeshUpload =
        rootTaskflow.emplace([&]() { animationSystem.storeMeshes(); }).name("Skinned mesh upload").succeed(meshUpload);

    auto storeTaskflow = rootTaskflow
                             .emplace(
                                 [&]()
                                 {
                                   for (const auto &storageSystem : drk::Systems::IStorageSystem::storageSystems)
                                   {
                                     storageSystem->store();
                                   }
                                 })
                             .name("Store allocation")
                             .succeed(skinnedMeshUpload)
                             .succeed(textureUpload);
    auto globalSystemStoreUpdate =
        rootTaskflow.emplace([&]() { globalSystem.update(); }).succeed(storeTaskflow).name("Global system update");
    const auto &spatialSystemPropagation =
        rootTaskflow.emplace([&]() { spatialSystem.PropagateChanges(); }).name("Spatial systempass").succeed(storeTaskflow);
    const auto &cameraPass = rootTaskflow.emplace([&]() { cameraSystem.processDirtyItems(); })
                                 .name("Camera dirty processing")
                                 .succeed(spatialSystemPropagation);
    const auto &emitMeshDrawsTask = rootTaskflow.emplace([&]() { meshSystem.emitDraws(); })
                                        .name("Mesh draw")
                                        .succeed(spatialSystemPropagation)
                                        .succeed(cameraPass);
    auto dirtyDrawTaskflow = rootTaskflow.emplace([&]() { meshSystem.processDirtyDraws(); })
                                 .name("Mesh dirty draw processing")
                                 .succeed(emitMeshDrawsTask);

    const auto &storeUpdateTask = rootTaskflow
                                      .emplace(
                                          [&](tf::Subflow &storeUpdateSubFlow)
                                          {
                                            for (const auto &storageSystem : drk::Systems::IStorageSystem::storageSystems)
                                            {
                                              storeUpdateSubFlow.emplace([&]() { storageSystem->updateStore(); })
                                                  .name(fmt::format("{0} store update", typeid(storageSystem).name()));
                                            }
                                          })
                                      .succeed(storeTaskflow)
                                      .name("Store update");

    const auto &updateDraws = rootTaskflow.emplace([&]() { sceneSystem.updateDraws(); })
                                  .precede(storeUpdateTask)
                                  .succeed(dirtyDrawTaskflow)
                                  .name("Update scene draws");

    /*

    const auto &animationUpdate = rootTaskflow.emplace([&]() { animationSystem.updateAnimations(); })
                                      .name("Animation system pass")
                                      .precede(spatialSystemPropagation)
                                      .succeed(resetSynchronizationState);

    rootTaskflow.emplace([&]() { boneSpatialSystem.propagateChanges(); })
        .name("Bone spatial propagation")
        .precede(updateStoreTaskflow)
        .succeed(spatialSystemPropagation);


    const auto &lightPerspectiveUpdate = rootTaskflow.emplace([&]() { lightPerspectiveSystem.processDirtyItems(); })
                                             .name("Light perspective processing")
                                             .precede(updateStoreTaskflow)
                                             .succeed(spatialSystemPropagation);

    rootTaskflow.emplace([&]() { directionalLightSystem.processDirtyItems(); })
        .name("Directional light processing")
        .succeed(spatialSystemPropagation)
        .precede(lightPerspectiveUpdate);

    rootTaskflow.emplace([&]() { pointLightSystem.processDirtyItems(); })
        .name("Point light processing")
        .succeed(spatialSystemPropagation)
        .precede(lightPerspectiveUpdate);

    rootTaskflow.emplace([&]() { spotlightSystem.processDirtyItems(); })
        .name("Spotlight processing")
        .succeed(spatialSystemPropagation)
        .precede(lightPerspectiveUpdate);

    const auto &axisAlignedBoundingBoxPass = rootTaskflow.emplace([&]() { axisAlignedBoundingBoxSystem.processDirty(); })
                                                 .name("AABB processing")
                                                 .succeed(spatialSystemPropagation)
                                                 .precede(lightPerspectiveUpdate);

    auto controllerTaskflow = rootTaskflow.emplace([&]() { flyCamController.Step(applicationState.frameTime); })
                                  .name("Flycam controller update")
                                  .precede(spatialSystemPropagation)
                                  .succeed(resetSynchronizationState);

*/
    /*auto resetCommandBufferTaskflow = rootTaskflow.emplace(
        [&]()
        {
          frameStatePtr->commandBuffer.reset();
          vk::CommandBufferBeginInfo commandBufferBeginInfo = {};
          const auto &result = frameStatePtr->commandBuffer.begin(&commandBufferBeginInfo);
        });
    resetCommandBufferTaskflow.succeed(updateApplicationStateTaskflow).name("Command buffer reset");*/

    /*
    const auto &updateShadowDrawsTaskflow =
        rootTaskflow.emplace([&]() { sceneSystem.updateShadowDraws(); }).name("Update shadow
    draws").succeed(updateStoreTaskflow);


     auto renderSceneTaskflow = rootTaskflow.emplace([&]() { sceneRenderer.render(0, frameStatePtr->commandBuffer); })
                                   .succeed(updateDraws)
                                   .succeed(updateShadowDrawsTaskflow)
                                   .succeed(resetCommandBufferTaskflow)
                                   .name("Render scene");

    auto updateSkinTaskflow = rootTaskflow.emplace([&]() { animationSystem.updateSkins(frameStatePtr->commandBuffer); })
                                  .name("Skin update")
                                  .succeed(skinnedMeshUpload)
                                  .succeed(resetCommandBufferTaskflow)
                                    .precede(renderSceneTaskflow);

    rootTaskflow.emplace([&]() { meshShadowSystem.emitDraws(); }).name("Mesh shadowd
    raw").succeed(spatialSystemPropagation).precede(updateShadowDrawsTaskflow); rootTaskflow.emplace([&]() {
    pointSystem.emitDraws(); }).name("Point draw").succeed(spatialSystemPropagation).precede(updateDraws);
    rootTaskflow.emplace([&]() { lineSystem.emitDraws(); }).name("Line
    draw").succeed(spatialSystemPropagation).precede(updateDraws); rootTaskflow.emplace([&]() {
    axisAlignedBoundingBoxSystem.emitDraws(); }) .name("AABB draw") .succeed(axisAlignedBoundingBoxPass)
        .succeed(spatialSystemPropagation)
        .precede(updateDraws);
    rootTaskflow.emplace([&]() { frustumSystem.emitDraws(); }).name("Frustum
    draw").succeed(cameraPass).succeed(spatialSystemPropagation) .precede(updateDraws);

    auto createSkinnedMeshInstanceTaskflow =
        rootTaskflow.emplace([&]() { animationSystem.createSkinnedMeshInstanceResources(engineState.getFrameIndex()); })
            .name("Create skinned mesh instance")
            .succeed(resetCommandBufferTaskflow)
            .succeed(meshUpload)
            .succeed(skinnedMeshUpload);

    auto resetTaskflow = rootTaskflow
                             .emplace(
                                 [&](tf::Subflow &resetSubflow)
                                 {
                                   resetSubflow
                                       .emplace(
                                           [&]()
                                           {
                                             registry.clear<Common::Components::Dirty<Spatials::Components::Spatial<Spatials::Components::Relative>>>();
                                           })
                                       .name("Reset relative spatial dirty flags");

                                   resetSubflow
                                       .emplace(
                                           [&]()
                                           {
                                             registry.clear<Common::Components::Dirty<Spatials::Components::Spatial<Spatials::Components::Absolute>>>();
                                           })
                                       .name("Reset absolute spatial dirty flags");
                                 })
                             .name("Dirty item cleanup")
                             .succeed(updateStoreTaskflow);
*/
    // auto drawGuiTaskflow = rootTaskflow.emplace([&]() { renderGui(applicationState);
    // }).name("DrawGUI").succeed(updateApplicationStateTaskflow);

    /* auto finalTask = rootTaskflow
                          .emplace(
                              [&]()
                              {
                                frameStatePtr->commandBuffer.end();

                                vk::PipelineStageFlags submissionWaitDstStageMask =
       vk::PipelineStageFlagBits::eColorAttachmentOutput; vk::SubmitInfo submitInfo = { .waitSemaphoreCount = 1,
       .pWaitSemaphores = &frameStatePtr->imageReadySemaphore, .pWaitDstStageMask = &submissionWaitDstStageMask,
       .commandBufferCount = 1, .pCommandBuffers = &frameStatePtr->commandBuffer, .signalSemaphoreCount = 1,
       .pSignalSemaphores = &frameStatePtr->imageRenderedSemaphore,
                                };

                                deviceContext.GraphicQueue.submit({ submitInfo }, frameStatePtr->fence);

                                vk::Result presentResult;
                                try
                                {
                                  presentResult = graphics.Present(swapchainImageAcquisitionResult.value);
                                }
                                catch (const vk::OutOfDateKHRError &e)
                                {
                                  shouldRecreateSwapchain = true;
                                }
                                workflowToMain.release();
                                mainToWorkflow.acquire();
                              })
                          .name("Final rendering task");*/

    /*auto renderGuiTaskflow =
        rootTaskflow
            .emplace(
                [&]() { userInterfaceRenderer.render(swapchainImageAcquisitionResult.value, frameStatePtr->commandBuffer); })
            .name("Render GUI")
            .succeed(drawGuiTaskflow)
            .succeed(sceneTextureTransitionTask);*/

    //.succeed(renderGuiTaskflow);

    rootTaskflow.dump(std::cout);
    tf::Executor executor;
    bool running = true;
    while (running = !glfwWindowShouldClose(window.GetWindow()))
    {
      globalSystem.GlobalSynchronizationState.Reset();
      glfwPollEvents();
      flyCamController.Step(applicationState.frameTime);
      updateApplicationState(sceneTexture, swapchainImageAcquisitionResult, frameStatePtr);

      frameStatePtr->commandBuffer.reset();
      vk::CommandBufferBeginInfo commandBufferBeginInfo = {};
      const auto &result = frameStatePtr->commandBuffer.begin(&commandBufferBeginInfo);

      executor.run(rootTaskflow).wait();

      registry.clear<Common::Components::Dirty<Spatials::Components::Spatial<Spatials::Components::Relative>>>();
      registry.clear<Common::Components::Dirty<Spatials::Components::Spatial<Spatials::Components::Absolute>>>();

      sceneRenderer.render(0, frameStatePtr->commandBuffer);
      renderGui(applicationState);

      if (sceneTexture.has_value())
      {
        Devices::Device::transitionLayout(frameStatePtr->commandBuffer,
            sceneTexture->image.image,
            sceneTexture->imageCreateInfo.format,
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eShaderReadOnlyOptimal,
            1);
      }

      userInterfaceRenderer.render(swapchainImageAcquisitionResult.value, frameStatePtr->commandBuffer);

      frameStatePtr->commandBuffer.end();

      vk::PipelineStageFlags submissionWaitDstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
      vk::SubmitInfo submitInfo = {
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &frameStatePtr->imageReadySemaphore,
        .pWaitDstStageMask = &submissionWaitDstStageMask,
        .commandBufferCount = 1,
        .pCommandBuffers = &frameStatePtr->commandBuffer,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &frameStatePtr->imageRenderedSemaphore,
      };

      deviceContext.GraphicQueue.submit({ submitInfo }, frameStatePtr->fence);

      vk::Result presentResult;
      try
      {
        presentResult = graphics.Present(swapchainImageAcquisitionResult.value);
      }
      catch (const vk::OutOfDateKHRError &e)
      {
        shouldRecreateSwapchain = true;
      }

      if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
      {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
      }
      engineState.incrementFrameIndex();
    }
    // while (!glfwWindowShouldClose(window.GetWindow()))
    //{
    //    updateApplicationState(sceneTexture, swapchainImageAcquisitionResult, frameStatePtr);
    //  }

    //{
    //  globalSystem.GlobalSynchronizationState.Reset();
    //}

    //{
    //  textureSystem.UploadTextures();
    //  meshSystem.uploadMeshes();
    //  animationSystem.storeMeshes();
    //}

    //{
    //  flyCamController.Step(applicationState.frameTime);
    //}

    //{
    //  animationSystem.updateAnimations();
    //  spatialSystem.PropagateChanges();
    //  boneSpatialSystem.propagateChanges();
    //}
    //{
    //  cameraSystem.processDirtyItems();
    //  lightPerspectiveSystem.processDirtyItems();
    //  directionalLightSystem.processDirtyItems();
    //  pointLightSystem.processDirtyItems();
    //  spotlightSystem.processDirtyItems();
    //  axisAlignedBoundingBoxSystem.processDirty();
    //}
    //// frustumSystem.processDirty();

    //// ??
    //{
    //  meshSystem.processDirtyDraws();
    //}

    //{
    //  // Resources to GPU
    //  materialSystem.store();
    //  meshSystem.store();
    //  pointSystem.store();
    //  lineSystem.store();
    //  spatialSystem.store();
    //  relativeSpatialSystem.store();
    //  objectSystem.store();
    //  cameraSystem.store();
    //  lightSystem.store();
    //  lightPerspectiveSystem.store();
    //  pointLightSystem.store();
    //  directionalLightSystem.store();
    //  spotlightSystem.store();
    //  axisAlignedBoundingBoxSystem.store();
    //  frustumSystem.store();
    //  boneSystem.store();
    //  boneSpatialSystem.store();
    //}

    //{
    //  // Store updates to GPU
    //  materialSystem.updateStore();
    //  meshSystem.updateStore();
    //  pointSystem.updateStore();
    //  lineSystem.updateStore();
    //  spatialSystem.updateStore();
    //  relativeSpatialSystem.updateStore();
    //  objectSystem.updateStore();
    //  cameraSystem.updateStore();
    //  lightSystem.updateStore();
    //  lightPerspectiveSystem.updateStore();
    //  pointLightSystem.updateStore();
    //  directionalLightSystem.updateStore();
    //  spotlightSystem.updateStore();
    //  axisAlignedBoundingBoxSystem.updateStore();
    //  boneSystem.updateStore();
    //  frustumSystem.updateStore();
    //  globalSystem.update();
    //  boneSpatialSystem.updateStore();
    //}

    //{
    //  animationSystem.createSkinnedMeshInstanceResources(engineState.getFrameIndex());
    //}

    //{
    //  frameStatePtr->commandBuffer.reset();
    //  vk::CommandBufferBeginInfo commandBufferBeginInfo = {};
    //  const auto &result = frameStatePtr->commandBuffer.begin(&commandBufferBeginInfo);
    //}

    //{
    //  animationSystem.updateSkins(frameStatePtr->commandBuffer);
    //}
    //// auto draws = registry.view<Scenes::Draws::SceneDraw>();
    //// registry.destroy(draws.begin(), draws.end());

    //{
    //  // Emit draws
    //  meshSystem.emitDraws();
    //  meshShadowSystem.emitDraws();
    //  pointSystem.emitDraws();
    //  lineSystem.emitDraws();
    //  axisAlignedBoundingBoxSystem.emitDraws();
    //  frustumSystem.emitDraws();
    //}

    //{
    //  // Stores draws to GPU
    //  sceneSystem.updateShadowDraws();
    //  sceneSystem.updateDraws();
    //}

    //{
    //  // Clear frame
    //  registry.clear<Common::Components::Dirty<Spatials::Components::Spatial<Spatials::Components::Relative>>>();
    //  registry.clear<Common::Components::Dirty<Spatials::Components::Spatial<Spatials::Components::Absolute>>>();
    //}

    //{
    //  // Renders
    //  sceneRenderer.render(0, frameStatePtr->commandBuffer);
    //}
    //{
    //  renderGui(applicationState);
    //}

    //{
    //  if (sceneTexture.has_value())
    //  {
    //    Devices::Device::
    //        transitionLayout(frameStatePtr->commandBuffer, sceneTexture->image.image, sceneTexture->imageCreateInfo.format,
    //        vk::ImageLayout::eUndefined, vk::ImageLayout::eShaderReadOnlyOptimal, 1);
    //  }
    //  {
    //    userInterfaceRenderer.render(swapchainImageAcquisitionResult.value, frameStatePtr->commandBuffer);
    //  }
    //}

    //{
    //  frameStatePtr->commandBuffer.end();

    //  vk::PipelineStageFlags submissionWaitDstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    //  vk::SubmitInfo submitInfo = {
    //    .waitSemaphoreCount = 1,
    //    .pWaitSemaphores = &frameStatePtr->imageReadySemaphore,
    //    .pWaitDstStageMask = &submissionWaitDstStageMask,
    //    .commandBufferCount = 1,
    //    .pCommandBuffers = &frameStatePtr->commandBuffer,
    //    .signalSemaphoreCount = 1,
    //    .pSignalSemaphores = &frameStatePtr->imageRenderedSemaphore,
    //  };

    //  deviceContext.GraphicQueue.submit({ submitInfo }, frameStatePtr->fence);

    //  vk::Result presentResult;
    //  bool outOfDate = false;
    //  try
    //  {
    //    presentResult = graphics.Present(swapchainImageAcquisitionResult.value);
    //  }
    //  catch (const vk::OutOfDateKHRError &e)
    //  {
    //    outOfDate = true;
    //  }

    //  if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    //  {
    //    ImGui::UpdatePlatformWindows();
    //    ImGui::RenderPlatformWindowsDefault();
    //  }
    //}
    //}
    engineState.incrementFrameIndex();

    deviceContext.device.waitIdle();
    if (sceneTexture.has_value())
      deviceContext.destroyTexture(sceneTexture.value());
  }
}  // namespace drk::Applications
