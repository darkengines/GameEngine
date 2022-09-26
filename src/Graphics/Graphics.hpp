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
			const Devices::DeviceContext &deviceContext,
			const vk::Extent2D extent
		);

		~Graphics();

		static vk::PipelineDepthStencilStateCreateInfo DefaultPipelineDepthStencilStateCreateInfo();

		static vk::PipelineColorBlendAttachmentState DefaultPipelineColorBlendAttachmentState();

		static vk::PipelineColorBlendStateCreateInfo DefaultPipelineColorBlendStateCreateInfo();

		static vk::PipelineMultisampleStateCreateInfo DefaultPipelineMultisampleStateCreateInfo();

		static vk::PipelineRasterizationStateCreateInfo DefaultPipelineRasterizationStateCreateInfo();

		static vk::PipelineViewportStateCreateInfo DefaultPipelineViewportStateCreateInfo(const vk::Extent2D &extent);

		static vk::PipelineInputAssemblyStateCreateInfo DefaultPipelineInputAssemblyStateCreateInfo();

		static vk::PipelineVertexInputStateCreateInfo DefaultPipelineVertexInputStateCreateInfo();

		vk::ShaderModule CreateShaderModule(const std::string &shaderPath) const;

	protected:
		const Devices::DeviceContext &DeviceContext;
		Devices::Swapchain Swapchain;
	};
}