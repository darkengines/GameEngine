
#pragma once

#include <imgui.h>

#include <vulkan/vulkan.hpp>

#include "../../Devices/DeviceContext.hpp"
#include "../../Devices/ImageInfo.hpp"
#include "../../Devices/Swapchain.hpp"
#include "../../Devices/Texture.hpp"
#include "../../Engine/EngineState.hpp"
#include "../../Renderers/Renderer.hpp"

namespace drk::UserInterfaces::Renderers
{
  class UserInterfaceRenderer : public drk::Renderers::Renderer
  {
   public:
    static std::vector<const char*> RequiredInstanceExtensions;
    static std::vector<const char*> RequiredDeviceExtensions;
    ImGuiContext* imGuiContext;

    explicit UserInterfaceRenderer(Devices::DeviceContext& deviceContext,
        Engine::EngineState& engineState,
        const Windows::Window& window);
    ~UserInterfaceRenderer();

    void render(uint32_t targetImageIndex, const vk::CommandBuffer& commandBuffer);
    void SetTargetImageViews(Devices::ImageInfo targetImageInfo, std::vector<vk::ImageView> targetImageViews);

   protected:
    void initializeImGuiContext();
    Devices::DeviceContext& deviceContext;
    Engine::EngineState& engineState;
    const Windows::Window& window;
    bool imGuiInitialized;

    vk::RenderPass mainRenderPass;

    std::vector<vk::ImageView> targetImageViews;
    std::optional<Devices::ImageInfo> targetImageInfo;
    std::optional<Devices::Texture> SceneRenderTargetTexture;
    std::vector<vk::Framebuffer> MainFramebuffers;
    std::optional<Devices::Texture> MainFramebufferTexture;
    std::optional<Devices::Texture> MainFramebufferDepthTexture;

    void DestroyMainFramebufferResources();
    void DestroyMainFramebuffer();
    void CreateMainRenderPass();
    void CreateMainFramebufferResources();
    void CreateMainFramebuffers();
    void RecreateRenderPass();
  };
}  // namespace drk::UserInterfaces::Renderers
