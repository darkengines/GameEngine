#pragma once

#include <vulkan/vulkan.hpp>
#include "../Devices/DeviceContext.hpp"
#include "../Devices/Swapchain.hpp"
#include "../Engine/EngineState.hpp"

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

		vk::ResultValue<uint32_t> AcquireSwapchainImageIndex();
		vk::Result Present(uint32_t swapchainImageIndex);
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
		[[nodiscard]] const Devices::Swapchain& GetSwapchain() const;
		void RecreateSwapchain(vk::Extent2D extent);
	protected:
		Devices::DeviceContext& DeviceContext;
		Engine::EngineState& EngineState;
		Devices::Swapchain Swapchain;

		vk::DescriptorPool ImGuiDescriptorPool;

		void DestroySwapchain();
		void CreateSwapchain(const vk::Extent2D& extent);
	};
}