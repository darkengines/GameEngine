#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS

#include "vulkan/vulkan.hpp"
#include "../Devices/DeviceContext.hpp"
#include "../Devices/Swapchain.hpp"

namespace drk::Graphics {
	class Graphics {
	public:
		static std::vector<const char *> RequiredInstanceExtensions;
		static std::vector<const char *> RequiredDeviceExtensions;

		Graphics(
			const Devices::DeviceContext *deviceContext,
			const vk::Extent2D extent
		);
		~Graphics();

		void SetExtent(const vk::Extent2D &extent);

		static vk::PipelineDepthStencilStateCreateInfo DefaultPipelineDepthStencilStateCreateInfo();
		static vk::PipelineColorBlendAttachmentState DefaultPipelineColorBlendAttachmentState();
		static vk::PipelineColorBlendStateCreateInfo DefaultPipelineColorBlendStateCreateInfo(vk::PipelineColorBlendAttachmentState& pipelineColorBlendAttachmentState);
		static vk::PipelineMultisampleStateCreateInfo DefaultPipelineMultisampleStateCreateInfo();
		static vk::PipelineRasterizationStateCreateInfo DefaultPipelineRasterizationStateCreateInfo();
		static vk::PipelineViewportStateCreateInfo DefaultPipelineViewportStateCreateInfo(const vk::Extent2D &extent, vk::Viewport& viewport, vk::Rect2D& scissor);
		static vk::PipelineInputAssemblyStateCreateInfo DefaultPipelineInputAssemblyStateCreateInfo();
		static vk::PipelineVertexInputStateCreateInfo DefaultPipelineVertexInputStateCreateInfo(
			std::vector<vk::VertexInputBindingDescription>& vertexInputBindingDescriptions,
			std::vector<vk::VertexInputAttributeDescription>& vertexInputAttributeDescriptions
		);

		vk::ShaderModule CreateShaderModule(const std::string &shaderPath) const;

	protected:
		const Devices::DeviceContext *DeviceContext;
		Devices::Swapchain Swapchain;
		vk::ShaderModule MainVertexShaderModule;
		vk::ShaderModule MainFragmentShaderModule;
		vk::RenderPass MainRenderPass;
		vk::PipelineLayout MainPipelineLayout;

		Devices::Texture MainFramebufferTexture;
		vk::ImageView MainFramebufferImageView;

		Devices::Texture MainFramebufferDepthTexture;
		vk::ImageView MainFramebufferDepthImageView;

		std::vector<vk::Framebuffer> MainFramebuffers;
		vk::Pipeline MainGraphicPipeline;

		void RecreateSwapchain(const vk::Extent2D extent);
		void CreateMainRenderPass();
		void CreateMainFramebufferResources();
		void CreateMainFramebuffers();
		void CreateMainPipelineLayout();
		void CreateMainGraphicPipeline();
	};
}