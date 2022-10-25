#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS

#include "vulkan/vulkan.hpp"
#include "../Devices/DeviceContext.hpp"
#include "../Devices/Swapchain.hpp"
#include "EngineState.hpp"
#include "DrawContext.hpp"

namespace drk::Graphics {
	class Graphics {
	public:
		static std::vector<const char *> RequiredInstanceExtensions;
		static std::vector<const char *> RequiredDeviceExtensions;

		Graphics(
			const Devices::DeviceContext *deviceContext,
			EngineState *engineState,
			const vk::Extent2D extent
		);
		~Graphics();

		void SetExtent(const vk::Extent2D &extent);

		void Render();
		static vk::PipelineDepthStencilStateCreateInfo DefaultPipelineDepthStencilStateCreateInfo();
		static vk::PipelineColorBlendAttachmentState DefaultPipelineColorBlendAttachmentState();
		static vk::PipelineColorBlendStateCreateInfo
		DefaultPipelineColorBlendStateCreateInfo(vk::PipelineColorBlendAttachmentState &pipelineColorBlendAttachmentState);
		static vk::PipelineMultisampleStateCreateInfo DefaultPipelineMultisampleStateCreateInfo();
		static vk::PipelineRasterizationStateCreateInfo DefaultPipelineRasterizationStateCreateInfo();
		static vk::PipelineViewportStateCreateInfo
		DefaultPipelineViewportStateCreateInfo(const vk::Extent2D &extent, vk::Viewport &viewport, vk::Rect2D &scissor);
		static vk::PipelineInputAssemblyStateCreateInfo DefaultPipelineInputAssemblyStateCreateInfo();
		static vk::PipelineVertexInputStateCreateInfo DefaultPipelineVertexInputStateCreateInfo(
			std::vector<vk::VertexInputBindingDescription> &vertexInputBindingDescriptions,
			std::vector<vk::VertexInputAttributeDescription> &vertexInputAttributeDescriptions
		);

	protected:
		const Devices::DeviceContext *DeviceContext;
		bool ExtentChanged = false;
		vk::Extent2D Extent;
		EngineState *EngineState;
		Devices::Swapchain Swapchain;
		vk::ShaderModule MainVertexShaderModule;
		vk::ShaderModule MainFragmentShaderModule;
		vk::RenderPass MainRenderPass;
		vk::PipelineLayout MainPipelineLayout;

		Devices::Texture MainFramebufferTexture;
		Devices::Texture MainFramebufferDepthTexture;

		std::vector<vk::Framebuffer> MainFramebuffers;
		vk::Pipeline MainGraphicPipeline;

		vk::DescriptorPool ImGuiDescriptorPool;

		void DestroyMainFramebufferResources();
		void DestroyShaderModules();
		void DestroyMainFramebuffer();
		void RecreateSwapchain(const vk::Extent2D extent);
		void DestroySwapchain();
		void CreateSwapchain(const vk::Extent2D &extent);
		void CreateMainRenderPass();
		void CreateMainFramebufferResources();
		void CreateMainFramebuffers();
		void CreateMainPipelineLayout();
		void CreateShaderModules();
		void CreateMainGraphicPipeline();
		void SetupImgui();
		DrawContext BuildMainRenderPass();

		void WaitFences();
		void ResetFences();
		void PopulateDrawContext(DrawContext &drawContext, const std::vector<Draw> &draws, uint32_t drawOffset);
	};
}