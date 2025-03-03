#include "UserInterfaceRenderer.hpp"

#include <functional>

#include "imgui_impl_vulkan.h"
#include "../../ImGui/imgui_impl_glfw.h"

namespace drk::UserInterfaces::Renderers
{
  void UserInterfaceRenderer::initializeImGuiContext()
  {
    IMGUI_CHECKVERSION();
    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui_ImplVulkan_InitInfo infos{
      .Instance = deviceContext.Instance,
      .PhysicalDevice = deviceContext.PhysicalDevice,
      .Device = deviceContext.device,
      .QueueFamily = 0,
      .Queue = deviceContext.GraphicQueue,
      .DescriptorPool = engineState.imGuiDescriptorPool,
      .RenderPass = static_cast<VkRenderPass>(mainRenderPass),
      .MinImageCount = 2,
      .ImageCount = 2,
      // TODO: Use configurable sample count
      .MSAASamples = VK_SAMPLE_COUNT_8_BIT,
    };

    auto glfwWindow = window.GetWindow();
    ImGui_ImplGlfw_InitForVulkan(glfwWindow, true);
    ImGui_ImplVulkan_Init(&infos);
  }
  UserInterfaceRenderer::UserInterfaceRenderer(Devices::DeviceContext& deviceContext,
      Engine::EngineState& engineState,
      const Windows::Window& window)
      : deviceContext(deviceContext),
        engineState(engineState),
        imGuiContext(ImGui::CreateContext()),
        window(window),
        imGuiInitialized(false)
  {}
  UserInterfaceRenderer::~UserInterfaceRenderer()
  {
    ImGui_ImplVulkan_Shutdown();
    DestroyMainFramebuffer();
    deviceContext.device.destroyRenderPass(mainRenderPass);
    if (SceneRenderTargetTexture.has_value())
      deviceContext.destroyTexture(SceneRenderTargetTexture.value());
    DestroyMainFramebufferResources();
  }
  void UserInterfaceRenderer::DestroyMainFramebuffer()
  {
    for (const auto& framebuffer : MainFramebuffers)
    {
      deviceContext.device.destroyFramebuffer(framebuffer);
    }
    MainFramebuffers.clear();
  }

  void UserInterfaceRenderer::DestroyMainFramebufferResources()
  {
    if (MainFramebufferDepthTexture.has_value())
      deviceContext.destroyTexture(MainFramebufferDepthTexture.value());
    if (MainFramebufferTexture.has_value())
      deviceContext.destroyTexture(MainFramebufferTexture.value());
  }
  void UserInterfaceRenderer::SetTargetImageViews(Devices::ImageInfo targetImageInfo,
      std::vector<vk::ImageView> targetImageViews)
  {
    this->targetImageInfo = targetImageInfo;
    this->targetImageViews = targetImageViews;
    RecreateRenderPass();
  }
  void UserInterfaceRenderer::CreateMainRenderPass()
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
      .loadOp = vk::AttachmentLoadOp::eDontCare,
      .storeOp = vk::AttachmentStoreOp::eStore,
      .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
      .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
      .initialLayout = vk::ImageLayout::eUndefined,
      .finalLayout = vk::ImageLayout::ePresentSrcKHR,
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
    vk::RenderPassCreateInfo renderPassCreateInfo = {
      .attachmentCount = static_cast<uint32_t>(attachments.size()),
      .pAttachments = attachments.data(),
      .subpassCount = 1,
      .pSubpasses = &subpass,
      .dependencyCount = 1,
      .pDependencies = &dependency,
    };
    auto renderPass = deviceContext.device.createRenderPass(renderPassCreateInfo);
    mainRenderPass = renderPass;
  }

  void UserInterfaceRenderer::RecreateRenderPass()
  {
    deviceContext.device.waitIdle();
    DestroyMainFramebuffer();
    if (mainRenderPass)
      deviceContext.device.destroyRenderPass(mainRenderPass);
    if (SceneRenderTargetTexture.has_value())
      deviceContext.destroyTexture(SceneRenderTargetTexture.value());
    DestroyMainFramebufferResources();

    CreateMainFramebufferResources();
    CreateMainRenderPass();
    CreateMainFramebuffers();

    if (!imGuiInitialized)
    {
      initializeImGuiContext();
      imGuiInitialized = true;
    }
  }

  void UserInterfaceRenderer::CreateMainFramebufferResources()
  {
    assert(this->targetImageInfo.has_value());
    vk::ImageCreateInfo imageCreateInfo{
      .imageType = vk::ImageType::e2D,
      .format = targetImageInfo->format,
      .extent = { targetImageInfo->extent.width, targetImageInfo->extent.height, 1 },
      .mipLevels = 1,
      .arrayLayers = 1,
      .samples = vk::SampleCountFlagBits::e8,
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
      .samples = vk::SampleCountFlagBits::e8,
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
    MainFramebufferTexture = { .image = mainFramebufferImage, .imageView = mainFramebufferImageView };
    MainFramebufferDepthTexture = { .image = mainFramebufferDepthImage, .imageView = mainFramebufferDepthImageView };
  }

  void UserInterfaceRenderer::CreateMainFramebuffers()
  {
    for (const auto& swapChainImageView : targetImageViews)
    {
      std::array<vk::ImageView, 3> attachments{
        MainFramebufferTexture->imageView, MainFramebufferDepthTexture->imageView, swapChainImageView
      };
      vk::FramebufferCreateInfo framebufferCreateInfo = { .renderPass = mainRenderPass,
        .attachmentCount = (uint32_t)attachments.size(),
        .pAttachments = attachments.data(),
        .width = targetImageInfo->extent.width,
        .height = targetImageInfo->extent.height,
        .layers = 1 };
      auto framebuffer = deviceContext.device.createFramebuffer(framebufferCreateInfo);
      MainFramebuffers.push_back(framebuffer);
    }
  }

  void UserInterfaceRenderer::render(uint32_t targetImageIndex, const vk::CommandBuffer& commandBuffer)
  {
    ImGui::Render();

    const auto& frameState = engineState.getCurrentFrameState();
    const auto& fence = frameState.fence;
    const auto& imageReadySemaphore = frameState.imageReadySemaphore;
    const auto& imageRenderedSemaphore = frameState.imageRenderedSemaphore;

    vk::ClearValue colorClearValue = { .color = { std::array<float, 4>{ 0.0f, 0.0f, 0.0f, 1.0f } } };
    vk::ClearValue depthClearValue{ .depthStencil = { 1.0f, 0 } };
    std::array<vk::ClearValue, 3> clearValues{ colorClearValue, depthClearValue, colorClearValue };

    vk::RenderPassBeginInfo mainRenderPassBeginInfo = {
      .renderPass = mainRenderPass,
      .framebuffer = MainFramebuffers[targetImageIndex],
      .renderArea = { 0, 0, { targetImageInfo->extent.width, targetImageInfo->extent.height } },
      .clearValueCount = (uint32_t)clearValues.size(),
      .pClearValues = clearValues.data(),
    };
    commandBuffer.beginRenderPass(mainRenderPassBeginInfo, vk::SubpassContents::eInline);
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
    commandBuffer.endRenderPass();
  }
}  // namespace drk::UserInterfaces::Renderers
