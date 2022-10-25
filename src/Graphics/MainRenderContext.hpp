#pragma once
#include "../Devices/Device.hpp"
#include "../Devices/DeviceContext.hpp"
#include "EngineState.hpp"

namespace drk::Graphics {
	class MainRenderContext {
		MainRenderContext(
			const Devices::DeviceContext* deviceContext,
			EngineState* engineState,
			const vk::Extent2D extent
		);
		~MainRenderContext();

		void SetExtent(const vk::Extent2D& extent);

	protected:
		const Devices::DeviceContext* DeviceContext;
		bool ExtentChanged = false;
		vk::Extent2D Extent;
		EngineState* EngineState;
		Devices::Swapchain Swapchain;
		vk::ShaderModule MainVertexShaderModule;
		vk::ShaderModule MainFragmentShaderModule;
		vk::RenderPass MainRenderPass;
		vk::PipelineLayout MainPipelineLayout;

		Devices::Texture MainFramebufferTexture;
		Devices::Texture MainFramebufferDepthTexture;

		std::vector<vk::Framebuffer> MainFramebuffers;
		vk::Pipeline MainGraphicPipeline;

		void DestroyMainFramebufferResources();
		void DestroyShaderModules();
		void DestroyMainFramebuffer();
		void RecreateSwapchain(const vk::Extent2D extent);
		void DestroySwapchain();
		void CreateSwapchain(const vk::Extent2D& extent);
		void CreateMainRenderPass();
		void CreateMainFramebufferResources();
		void CreateMainFramebuffers();
		void CreateMainPipelineLayout();
		void CreateShaderModules();
		void CreateMainGraphicPipeline();
	};
}