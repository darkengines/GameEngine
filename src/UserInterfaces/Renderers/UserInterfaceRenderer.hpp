
#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS
#include "vulkan/vulkan.hpp"

#include "../../Renderers/Renderer.hpp"
#include "../../Devices/DeviceContext.hpp"
#include "../../Devices/Swapchain.hpp"
#include "../../Devices/Texture.hpp"
#include "../../Engine/EngineState.hpp"
#include "../../Devices/ImageInfo.hpp"

namespace drk::UserInterfaces::Renderers {
	class UserInterfaceRenderer : public drk::Renderers::Renderer {
	public:
		static std::vector<const char*> RequiredInstanceExtensions;
		static std::vector<const char*> RequiredDeviceExtensions;

		UserInterfaceRenderer(
			Devices::DeviceContext& deviceContext,
			Engine::EngineState& engineState,
			Windows::Window& windows
		);
		~UserInterfaceRenderer();

		void SetExtent(const vk::Extent2D& extent);
		void Render(const vk::CommandBuffer& commandBuffer, uint32_t swapchainImageIndex) const;
		void Present(uint32_t swapchainImageIndex);
		void SetTargetImageViews(Devices::ImageInfo targetImageInfo, std::vector<vk::ImageView> targetImageViews);
		std::optional<Devices::Texture> GetSceneRenderTargetTexture() const { return SceneRenderTargetTexture; }

	protected:
		Devices::DeviceContext& DeviceContext;
		Engine::EngineState& EngineState;

		vk::RenderPass MainRenderPass;
		vk::DescriptorPool ImGuiDescriptorPool;

		std::vector<vk::ImageView> targetImageViews;
		std::optional<Devices::ImageInfo> targetImageInfo;
		std::optional<Devices::Texture> SceneRenderTargetTexture;
		std::vector<vk::Framebuffer> MainFramebuffers;
		std::optional<Devices::Texture> MainFramebufferTexture;
		std::optional<Devices::Texture> MainFramebufferDepthTexture;

		void DestroyMainFramebufferResources();
		void DestroyMainFramebuffer();
		void CreateMainRenderPass(const vk::RenderPassCreateInfo& renderPassCreateInfo);
		void CreateMainFramebufferResources();
		void CreateMainFramebuffers();
		void SetupImgui();
		vk::RenderPassCreateInfo GetDefaultRenderPassCreateInfo();
		void RecreateRenderPass(const vk::RenderPassCreateInfo& renderPassCreateInfo);
	};
}