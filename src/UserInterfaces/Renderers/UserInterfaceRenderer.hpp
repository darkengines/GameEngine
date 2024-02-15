
#pragma once

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

		explicit UserInterfaceRenderer(
			Devices::DeviceContext& deviceContext,
			Engine::EngineState& engineState
		);
		~UserInterfaceRenderer();

		void render(uint32_t targetImageIndex, const vk::CommandBuffer& commandBuffer);
		void Present(uint32_t swapchainImageIndex);
		void SetTargetImageViews(Devices::ImageInfo targetImageInfo, std::vector<vk::ImageView> targetImageViews);

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

		bool isImGuiInitialized = false;

		void DestroyMainFramebufferResources();
		void DestroyMainFramebuffer();
		void CreateMainRenderPass();
		void CreateMainFramebufferResources();
		void CreateMainFramebuffers();
		void SetupImgui();
		void RecreateRenderPass();
		void CreateImguiResources();
	};
}