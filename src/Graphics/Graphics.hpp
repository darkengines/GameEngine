#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS

#include "vulkan/vulkan.hpp"
#include "../Devices/DeviceContext.hpp"
#include "../Devices/Swapchain.hpp"
#include "../Engine/EngineState.hpp"
#include "DrawContext.hpp"

namespace drk::Graphics {
	class Graphics {
	public:
		static std::vector<const char*> RequiredInstanceExtensions;
		static std::vector<const char*> RequiredDeviceExtensions;

		Graphics(
			Devices::DeviceContext& deviceContext,
			Engine::EngineState& engineState,
			Windows::Window& windows
		);
		~Graphics();

		void SetExtent(const vk::Extent2D& extent);

		uint32_t AcuireSwapchainImageIndex();
		void Render(const vk::CommandBuffer& commandBuffer, uint32_t swapchainImageIndex) const;
		void Present(uint32_t swapchainImageIndex);
		static vk::PipelineDepthStencilStateCreateInfo DefaultPipelineDepthStencilStateCreateInfo();
		static vk::PipelineColorBlendAttachmentState DefaultPipelineColorBlendAttachmentState();
		static vk::PipelineColorBlendStateCreateInfo
		DefaultPipelineColorBlendStateCreateInfo(vk::PipelineColorBlendAttachmentState& pipelineColorBlendAttachmentState);
		static vk::PipelineMultisampleStateCreateInfo DefaultPipelineMultisampleStateCreateInfo();
		static vk::PipelineRasterizationStateCreateInfo DefaultPipelineRasterizationStateCreateInfo();
		static vk::PipelineViewportStateCreateInfo
		DefaultPipelineViewportStateCreateInfo(const vk::Extent2D& extent, vk::Viewport& viewport, vk::Rect2D& scissor);
		static vk::PipelineInputAssemblyStateCreateInfo DefaultPipelineInputAssemblyStateCreateInfo();
		static vk::PipelineVertexInputStateCreateInfo DefaultPipelineVertexInputStateCreateInfo(
			const std::vector<vk::VertexInputBindingDescription>& vertexInputBindingDescriptions,
			const std::vector<vk::VertexInputAttributeDescription>& vertexInputAttributeDescriptions
		);
		static Devices::Texture
		BuildSceneRenderTargetTexture(const Devices::DeviceContext& deviceContext, const Devices::Swapchain& swapchain);
		Devices::Texture GetSceneRenderTargetTexture() const { return SceneRenderTargetTexture; }
		const Devices::Swapchain& GetSwapchain() const;
	protected:
		Devices::DeviceContext& DeviceContext;
		Engine::EngineState& EngineState;
		vk::Extent2D Extent;
		bool ExtentChanged = false;
		Devices::Swapchain Swapchain;
		Devices::Texture SceneRenderTargetTexture;

		vk::RenderPass MainRenderPass;

		Devices::Texture MainFramebufferTexture;
		Devices::Texture MainFramebufferDepthTexture;

		std::vector<vk::Framebuffer> MainFramebuffers;

		vk::DescriptorPool ImGuiDescriptorPool;

		void DestroyMainFramebufferResources();
		void DestroyMainFramebuffer();
		void RecreateSwapchain(vk::Extent2D extent);
		void DestroySwapchain();
		void CreateSwapchain(vk::Extent2D& extent);
		void CreateMainRenderPass();
		void CreateMainFramebufferResources();
		void createMainFramebuffers();
		void SetupImgui();
	};
}