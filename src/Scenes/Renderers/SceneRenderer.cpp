#define GLM_ENABLE_EXPERIMENTAL
#include "SceneRenderer.hpp"

#include <imgui_internal.h>

#include <functional>
#include <glm/gtx/matrix_decompose.hpp>

#include "../../Cameras/Components/Camera.hpp"
#include "../../Editors/Components/Selected.hpp"
#include "../../Graphics/Graphics.hpp"
#include "../../Spatials/Components/Spatial.hpp"
#include "../../Spatials/Systems/SpatialSystem.hpp"
#include "ImGuizmo.h"
#include "imgui_impl_vulkan.h"

namespace drk::Scenes::Renderers
{
  void SceneRenderer::initializeImGuiContext()
  {
    IMGUI_CHECKVERSION();
    auto parentContext = ImGui::GetCurrentContext();
    ImGui::SetCurrentContext(imGuiContext);

    ImGui_ImplVulkan_InitInfo infos{
      .Instance = deviceContext.Instance,
      .PhysicalDevice = deviceContext.PhysicalDevice,
      .Device = deviceContext.device,
      .QueueFamily = 0,
      .Queue = deviceContext.GraphicQueue,
      .DescriptorPool = engineState.imGuiDescriptorPool,
      .RenderPass = static_cast<VkRenderPass>(renderPass),
      .MinImageCount = 2,
      .ImageCount = 2,
      .MSAASamples = VK_SAMPLE_COUNT_8_BIT,
      .PipelineRenderingCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
        .colorAttachmentCount = 1,
        .pColorAttachmentFormats = (VkFormat*)&targetImageInfo->format,
        .depthAttachmentFormat = (VkFormat)deviceContext.DepthFormat,
      },
      // TODO: Use configurable sample count
    };

    const auto& glfwWindow = window.GetWindow();
    nestedWindow = { .handle = glfwWindow, .parentContext = parentContext };
    ImGui_ImplNested_InitForVulkan(&nestedWindow, true);
    ImGui_ImplVulkan_Init(&infos);
  }

  SceneRenderer::SceneRenderer(Engine::EngineState& engineState,
      const Devices::DeviceContext& deviceContext,
      entt::registry& registry,
      std::function<std::unique_ptr<Meshes::Pipelines::MeshPipeline>()> meshPipelineFactory,
      std::function<std::unique_ptr<Points::Pipelines::PointPrimitivePipeline>()> pointPrimitivePipelineFactory,
      std::function<std::unique_ptr<Lines::Pipelines::LinePipeline>()> linePipelineFactory,
      std::function<std::unique_ptr<BoundingVolumes::Pipelines::BoundingVolumePipeline>()> boundingVolumePipelineFactory,
      std::function<std::unique_ptr<Frustums::Pipelines::FrustumPipeline>()> frustumPipelineFactory,
      std::function<std::unique_ptr<Renderers::ShadowSceneRenderer>()> shadowSceneRendererFactory,
      Lights::Systems::ShadowMappingSystem& shadowMappingSystem,
      const Windows::Window& window)
      : engineState(engineState),
        deviceContext(deviceContext),
        registry(registry),
        meshPipeline(std::move(meshPipelineFactory())),
        pointPrimitivePipeline(std::move(pointPrimitivePipelineFactory())),
        linePipeline(std::move(linePipelineFactory())),
        boundingVolumePipeline(std::move(boundingVolumePipelineFactory())),
        frustumPipeline(std::move(frustumPipelineFactory())),
        shadowSceneRenderer(std::move(shadowSceneRendererFactory())),
        shadowMappingSystem(shadowMappingSystem),
        pipelines{ { std::type_index(typeid(Meshes::Pipelines::MeshPipeline)), this->meshPipeline.get() },
          { std::type_index(typeid(Points::Pipelines::PointPrimitivePipeline)), this->pointPrimitivePipeline.get() },
          { std::type_index(typeid(Lines::Pipelines::LinePipeline)), this->linePipeline.get() },
          { std::type_index(typeid(BoundingVolumes::Pipelines::BoundingVolumePipeline)),
              this->boundingVolumePipeline.get() },
          { std::type_index(typeid(Frustums::Pipelines::FrustumPipeline)), this->frustumPipeline.get() } },
        imGuiContext(ImGui::CreateContext()),
        window(window),
        imGuiInitialized(false)
  {
    this->shadowSceneRenderer->setTargetImageViews(
        { .extent = shadowMappingSystem.shadowMappingTexture->imageCreateInfo.extent,
            .format = shadowMappingSystem.shadowMappingTexture->imageCreateInfo.format },
        { shadowMappingSystem.shadowMappingTexture->imageView });
  }

  SceneRenderer::~SceneRenderer()
  {
    ImGui::SetCurrentContext(imGuiContext);
    ImGui_ImplVulkan_Shutdown();
    destroyFramebuffers();
    destroyRenderPass();
    destroyFramebufferResources();
  }
  vk::Extent3D SceneRenderer::getUserExtent()
  {
    return userExtent;
  }
  Pipelines::GraphicsPipeline* SceneRenderer::getPipeline(std::type_index pipelineTypeIndex)
  {
    auto keyValuePair = pipelines.find(pipelineTypeIndex);
    if (keyValuePair == pipelines.end())
      throw std::runtime_error(fmt::format("Unsupported pipeline type index {0}.", pipelineTypeIndex.name()));
    return keyValuePair->second;
  }
  void SceneRenderer::destroyFramebuffers()
  {
    for (const auto& framebuffer : framebuffers)
    {
      deviceContext.device.destroyFramebuffer(framebuffer);
    }
    framebuffers.clear();
  }

  void SceneRenderer::destroyFramebufferResources()
  {
    if (depthTexture.has_value())
      deviceContext.destroyTexture(depthTexture.value());
    if (colorTexture.has_value())
      deviceContext.destroyTexture(colorTexture.value());
  }
  void SceneRenderer::createFramebufferResources()
  {
    vk::ImageCreateInfo imageCreateInfo{
      .imageType = vk::ImageType::e2D,
      .format = targetImageInfo->format,
      .extent = { targetImageInfo->extent.width, targetImageInfo->extent.height, 1 },
      .mipLevels = 1,
      .arrayLayers = 1,
      .samples = deviceContext.MaxSampleCount,
      .usage = vk::ImageUsageFlagBits::eColorAttachment,
      .sharingMode = vk::SharingMode::eExclusive,
    };
    auto mainFramebufferImage = deviceContext.createImage(imageCreateInfo, vk::MemoryPropertyFlagBits::eDeviceLocal);

    vk::ImageSubresourceRange subresourceRange = {
      .aspectMask = vk::ImageAspectFlagBits::eColor, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1
    };

    vk::ImageViewCreateInfo imageViewCreateInfo = { .image = mainFramebufferImage.image,
      .viewType = vk::ImageViewType::e2D,
      .format = targetImageInfo->format,
      .subresourceRange = subresourceRange };

    auto mainFramebufferImageView = deviceContext.device.createImageView(imageViewCreateInfo);

    vk::ImageCreateInfo depthImageCreateInfo{
      .imageType = vk::ImageType::e2D,
      .format = deviceContext.DepthFormat,
      .extent = { targetImageInfo->extent.width, targetImageInfo->extent.height, 1 },
      .mipLevels = 1,
      .arrayLayers = 1,
      .samples = deviceContext.MaxSampleCount,
      .usage = vk::ImageUsageFlagBits::eDepthStencilAttachment,
      .sharingMode = vk::SharingMode::eExclusive,
    };
    auto mainFramebufferDepthImage =
        deviceContext.createImage(depthImageCreateInfo, vk::MemoryPropertyFlagBits::eDeviceLocal);

    vk::ImageSubresourceRange depthSubresourceRange = {
      .aspectMask = vk::ImageAspectFlagBits::eDepth, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1
    };

    vk::ImageViewCreateInfo depthImageViewCreateInfo = { .image = mainFramebufferDepthImage.image,
      .viewType = vk::ImageViewType::e2D,
      .format = deviceContext.DepthFormat,
      .subresourceRange = depthSubresourceRange };

    auto mainFramebufferDepthImageView = deviceContext.device.createImageView(depthImageViewCreateInfo);
    colorTexture = { .image = mainFramebufferImage, .imageView = mainFramebufferImageView };
    depthTexture = { .image = mainFramebufferDepthImage, .imageView = mainFramebufferDepthImageView };
  }

  void SceneRenderer::createFramebuffers()
  {
    for (const auto& swapChainImageView : targetImageViews)
    {
      std::array<vk::ImageView, 3> attachments{ colorTexture->imageView, depthTexture->imageView, swapChainImageView };
      vk::FramebufferCreateInfo framebufferCreateInfo = { .renderPass = renderPass,
        .attachmentCount = (uint32_t)attachments.size(),
        .pAttachments = attachments.data(),
        .width = targetImageInfo->extent.width,
        .height = targetImageInfo->extent.height,
        .layers = 1 };
      auto framebuffer = deviceContext.device.createFramebuffer(framebufferCreateInfo);
      framebuffers.push_back(framebuffer);
    }
  }

  void SceneRenderer::createRenderPass()
  {
    vk::AttachmentDescription colorAttachment = {
      .format = targetImageInfo->format,
      // TODO: Use configurable sample count
      .samples = vk::SampleCountFlagBits::e8,
      .loadOp = vk::AttachmentLoadOp::eClear,
      .storeOp = vk::AttachmentStoreOp::eStore,
      .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
      .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
      .initialLayout = vk::ImageLayout::eUndefined,
      .finalLayout = vk::ImageLayout::eColorAttachmentOptimal,
    };

    vk::AttachmentReference colorAttachmentRef = { .attachment = 0, .layout = vk::ImageLayout::eColorAttachmentOptimal };

    vk::AttachmentDescription depthAttachment = {
      .format = deviceContext.DepthFormat,
      // TODO: Use configurable sample count
      .samples = vk::SampleCountFlagBits::e8,
      .loadOp = vk::AttachmentLoadOp::eClear,
      .storeOp = vk::AttachmentStoreOp::eStore,
      .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
      .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
      .initialLayout = vk::ImageLayout::eUndefined,
      .finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal,
    };

    vk::AttachmentReference depthAttachmentRef = { .attachment = 1,
      .layout = vk::ImageLayout::eDepthStencilAttachmentOptimal };

    vk::AttachmentDescription resolvedColorAttachment = {
      .format = targetImageInfo->format,
      .samples = vk::SampleCountFlagBits::e1,
      .loadOp = vk::AttachmentLoadOp::eClear,
      .storeOp = vk::AttachmentStoreOp::eStore,
      .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
      .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
      .initialLayout = vk::ImageLayout::eUndefined,
      .finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
    };

    vk::AttachmentReference resolvedColorAttachmentRef = { .attachment = 2,
      .layout = vk::ImageLayout::eColorAttachmentOptimal };

    vk::SubpassDescription subpass = {
      .pipelineBindPoint = vk::PipelineBindPoint::eGraphics,
      .colorAttachmentCount = 1,
      .pColorAttachments = &colorAttachmentRef,
      .pResolveAttachments = &resolvedColorAttachmentRef,
      .pDepthStencilAttachment = &depthAttachmentRef,
    };

    vk::SubpassDependency dependency = { .srcSubpass = VK_SUBPASS_EXTERNAL,
      .dstSubpass = 0,
      .srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
      .dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
      .srcAccessMask = vk::AccessFlagBits::eNone,
      .dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite };

    std::vector<vk::AttachmentDescription> attachments{ colorAttachment, depthAttachment, resolvedColorAttachment };
    vk::RenderPassCreateInfo renderPassCreationInfo = {
      .attachmentCount = static_cast<uint32_t>(attachments.size()),
      .pAttachments = attachments.data(),
      .subpassCount = 1,
      .pSubpasses = &subpass,
      .dependencyCount = 1,
      .pDependencies = &dependency,
    };

    renderPass = deviceContext.device.createRenderPass(renderPassCreationInfo);
  }
  void SceneRenderer::setTargetImageViews(Devices::ImageInfo targetImageInfo, std::vector<vk::ImageView> targetImageViews)
  {
    this->targetImageInfo = targetImageInfo;
    this->targetImageViews = targetImageViews;
    for (const auto& pipeline : pipelines)
      pipeline.second->destroyPipeline();
    destroyFramebuffers();
    destroyFramebufferResources();
    destroyRenderPass();
    createRenderPass();
    if (!imGuiInitialized)
    {
      initializeImGuiContext();
      imGuiInitialized = true;
    }

    vk::Viewport viewport;
    vk::Rect2D scissor;

    userExtent = targetImageInfo.extent;
    const auto& pipelineViewportStateCreateInfo = Graphics::Graphics::DefaultPipelineViewportStateCreateInfo(
        { userExtent.width, userExtent.height }, viewport, scissor);

    for (const auto& pipeline : pipelines)
    {
      pipeline.second->configure(
          [&](vk::GraphicsPipelineCreateInfo& graphicsPipelineCreateInfo)
          {
            graphicsPipelineCreateInfo.renderPass = renderPass;
            graphicsPipelineCreateInfo.pViewportState = &pipelineViewportStateCreateInfo;
          });
    }

    createFramebufferResources();
    createFramebuffers();
  }
  void SceneRenderer::render(uint32_t targetImageIndex, const vk::CommandBuffer& commandBuffer)
  {
    if (!targetImageInfo.has_value())
      return;
    shadowSceneRenderer->render(0, commandBuffer);
    vk::ClearValue colorClearValue = { .color = { std::array<float, 4>{ 0.0f, 0.0f, 0.0f, 1.0f } } };
    vk::ClearValue depthClearValue{ .depthStencil = { 1.0f, 0 } };
    std::array<vk::ClearValue, 3> clearValues{ colorClearValue, depthClearValue, colorClearValue };
    const auto& extent = targetImageInfo.value().extent;
    vk::RenderPassBeginInfo mainRenderPassBeginInfo = {
      .renderPass = renderPass,
      .framebuffer = framebuffers[targetImageIndex],
      .renderArea = { 0, 0, { extent.width, extent.height } },
      .clearValueCount = static_cast<uint32_t>(clearValues.size()),
      .pClearValues = clearValues.data(),
    };
    commandBuffer.beginRenderPass(mainRenderPassBeginInfo, vk::SubpassContents::eInline);
    const auto& draws = registry.view<Draws::SceneDraw>();
    const Draws::SceneDraw* previousSceneDraw = nullptr;
    entt::entity previousDrawEntity = entt::null;

    uint32_t instanceCount = 0u;

    std::map<std::type_index, int> pipelineDrawIndices;
    for (const auto& pipeline : pipelines)
    {
      pipelineDrawIndices[pipeline.first] = 0;
    }
    bool isFirst = true;
    Pipelines::GraphicsPipeline const* pCurrentPipeline;

    auto previousContext = ImGui::GetCurrentContext();
    ImGui::SetCurrentContext(imGuiContext);

    ImGui_ImplNested_NewFrame();
    ImGui_ImplVulkan_NewFrame();

    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
    ImGuizmo::SetRect(0, 0, userExtent.width, userExtent.height);

    renderGridGui();
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
    //ImGui::EndFrame();
    draws.each(
        [&](entt::entity drawEntity, const Draws::SceneDraw& sceneDraw)
        {
          auto operations = drk::Renderers::RenderOperation::None;
          if (isFirst || previousSceneDraw->pipelineTypeIndex != sceneDraw.pipelineTypeIndex)
          {
            isFirst = false;
            operations |= drk::Renderers::RenderOperation::BindPipeline;
          }
          if (previousDrawEntity == entt::null ||
              (previousSceneDraw->indexBufferView.buffer.buffer != sceneDraw.indexBufferView.buffer.buffer) ||
              (previousSceneDraw->vertexBufferView.buffer.buffer != sceneDraw.vertexBufferView.buffer.buffer))
          {
            operations |=
                drk::Renderers::RenderOperation::BindIndexBuffer | drk::Renderers::RenderOperation::BindVertexBuffer;
          }
          if (previousDrawEntity != entt::null &&
              (previousSceneDraw->indexBufferView.byteOffset != sceneDraw.indexBufferView.byteOffset ||
                  previousSceneDraw->vertexBufferView.byteOffset != sceneDraw.vertexBufferView.byteOffset))
          {
            operations |= drk::Renderers::RenderOperation::Draw;
          }
          if (previousDrawEntity == entt::null)
          {
            doOperations(commandBuffer, operations, sceneDraw, &pCurrentPipeline);
          }
          else
          {
            if (operations != drk::Renderers::RenderOperation::None)
            {
              draw(previousDrawEntity,
                  *previousSceneDraw,
                  commandBuffer,
                  instanceCount,
                  pipelineDrawIndices[previousSceneDraw->pipelineTypeIndex],
                  pCurrentPipeline);
              pipelineDrawIndices[previousSceneDraw->pipelineTypeIndex] += instanceCount;
              instanceCount = 0u;
            }
            doOperations(commandBuffer, operations, sceneDraw, &pCurrentPipeline);
          }
          previousSceneDraw = &sceneDraw;
          previousDrawEntity = drawEntity;
          instanceCount++;
        });
    if (previousDrawEntity != entt::null)
    {
      this->draw(previousDrawEntity,
          *previousSceneDraw,
          commandBuffer,
          instanceCount,
          pipelineDrawIndices[previousSceneDraw->pipelineTypeIndex],
          pCurrentPipeline);
    }
    /*ImGui::NewFrame();
    ImGuizmo::BeginFrame();*/
    renderGizmoGui();
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
    ImGui::EndFrame();
    ImGui::SetCurrentContext(previousContext);
    commandBuffer.endRenderPass();
  }
  void SceneRenderer::draw(entt::entity previousDrawEntity,
      const Draws::SceneDraw& previousSceneDraw,
      const vk::CommandBuffer& commandBuffer,
      int instanceCount,
      int firstInstance,
      Pipelines::GraphicsPipeline const* pPipeline)
  {
    auto bufferInfo = pPipeline->getBufferInfo(registry, previousDrawEntity);
    commandBuffer.drawIndexed(
        bufferInfo.indexCount, instanceCount, bufferInfo.firstIndex, bufferInfo.vertexOffset, firstInstance);
  }
  void SceneRenderer::doOperations(const vk::CommandBuffer& commandBuffer,
      drk::Renderers::RenderOperation sceneRenderOperation,
      const Draws::SceneDraw& sceneDraw,
      Pipelines::GraphicsPipeline const** ppPipeline)
  {
    if ((sceneRenderOperation & drk::Renderers::RenderOperation::BindPipeline) ==
        drk::Renderers::RenderOperation::BindPipeline)
    {
      const auto& pipeline = getPipeline(sceneDraw.pipelineTypeIndex);
      *ppPipeline = pipeline;
      pipeline->bind(commandBuffer);
    }
    if ((sceneRenderOperation & drk::Renderers::RenderOperation::BindIndexBuffer) ==
        drk::Renderers::RenderOperation::BindIndexBuffer)
    {
      commandBuffer.bindIndexBuffer(sceneDraw.indexBufferView.buffer.buffer, 0, vk::IndexType::eUint32);
    }
    if ((sceneRenderOperation & drk::Renderers::RenderOperation::BindVertexBuffer) ==
        drk::Renderers::RenderOperation::BindVertexBuffer)
    {
      vk::DeviceSize offset = 0u;
      commandBuffer.bindVertexBuffers(0, 1, &sceneDraw.vertexBufferView.buffer.buffer, &offset);
    }
  }
  void SceneRenderer::destroyRenderPass()
  {
    deviceContext.device.destroyRenderPass(renderPass);
  }
  Devices::Texture SceneRenderer::BuildSceneRenderTargetTexture(const Devices::DeviceContext& deviceContext,
      vk::Extent3D extent)
  {
    vk::ImageCreateInfo imageCreateInfo{
      .imageType = vk::ImageType::e2D,
      .format = vk::Format::eR8G8B8A8Srgb,
      .extent = extent,
      .mipLevels = 1,
      .arrayLayers = 1,
      .samples = vk::SampleCountFlagBits::e1,
      .usage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
      .sharingMode = vk::SharingMode::eExclusive,
    };
    auto memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;
    auto mainFramebufferImage = deviceContext.createImage(imageCreateInfo, memoryProperties);

    vk::ImageSubresourceRange subresourceRange = {
      .aspectMask = vk::ImageAspectFlagBits::eColor, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1
    };

    vk::ImageViewCreateInfo imageViewCreateInfo = { .image = mainFramebufferImage.image,
      .viewType = vk::ImageViewType::e2D,
      .format = vk::Format::eR8G8B8A8Srgb,
      .subresourceRange = subresourceRange };

    auto mainFramebufferImageView = deviceContext.device.createImageView(imageViewCreateInfo);

    Devices::Texture target = { .image = mainFramebufferImage,
      .imageView = mainFramebufferImageView,
      .imageCreateInfo = imageCreateInfo,
      .imageViewCreateInfo = imageViewCreateInfo,
      .memoryProperties = memoryProperties };

    return target;
  }
  void SceneRenderer::setTargetExtent(vk::Extent3D extent)
  {
    userExtent = extent;
    deviceContext.device.waitIdle();
    for (const auto& pipeline : pipelines)
    {
      pipeline.second->destroyPipeline();
    }

    vk::Viewport viewport;
    vk::Rect2D scissor;

    const auto& pipelineViewportStateCreateInfo = Graphics::Graphics::DefaultPipelineViewportStateCreateInfo(
        { userExtent.width, userExtent.height }, viewport, scissor);
    for (const auto& pipeline : pipelines)
    {
      pipeline.second->configure(
          [&](vk::GraphicsPipelineCreateInfo& graphicsPipelineCreateInfo)
          {
            graphicsPipelineCreateInfo.renderPass = renderPass;
            graphicsPipelineCreateInfo.pViewportState = &pipelineViewportStateCreateInfo;
          });
    }
  }
  void SceneRenderer::renderGridGui()
  {
    const auto& camera = registry.get<Cameras::Components::Camera>(engineState.cameraEntity);

    auto perspective = camera.perspective;
    perspective[1][1] *= -1.0f;
    ImGuizmo::DrawGrid(
        glm::value_ptr(camera.view), glm::value_ptr(perspective), glm::value_ptr(glm::identity<glm::mat4>()), 100.f);

  }
  void SceneRenderer::renderGizmoGui()
  {
    
    const auto& camera = registry.get<Cameras::Components::Camera>(engineState.cameraEntity);
    const auto& selectedEntities =
        registry.view<Spatials::Components::Spatial<Spatials::Components::Relative>, drk::Editors::Components::Selected>();
    auto perspective = camera.perspective;
    perspective[1][1] *= -1.0f;
    selectedEntities.each(
        [&](entt::entity selectedEntity, Spatials::Components::Spatial<Spatials::Components::Relative>& relativeSpatial)
        {
          if (ImGuizmo::Manipulate(glm::value_ptr(camera.view),
                  glm::value_ptr(perspective),
                  ImGuizmo::OPERATION::UNIVERSAL,
                  ImGuizmo::WORLD,
                  glm::value_ptr(relativeSpatial.model),
                  nullptr))
          {
            glm::vec3 scale, translation, skew;
            glm::vec4 perspective;
            glm::quat rotation;
            glm::decompose(relativeSpatial.model, scale, rotation, translation, skew, perspective);
            relativeSpatial.position = glm::vec4(translation, 1);
            relativeSpatial.rotation = rotation;
            relativeSpatial.scale = glm::vec4(scale, 0);
            Spatials::Systems::SpatialSystem::makeDirty(registry, selectedEntity);
          }
        });
  }
}  // namespace drk::Scenes::Renderers
