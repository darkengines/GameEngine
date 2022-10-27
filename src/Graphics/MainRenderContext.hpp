#pragma once
#include "../Devices/Device.hpp"
#include "../Devices/DeviceContext.hpp"
#include "EngineState.hpp"
#include "DrawContext.hpp"

namespace drk::Graphics {
	class MainRenderContext {
	public:
		MainRenderContext(
			const Devices::DeviceContext& deviceContext,
			const EngineState& engineState,
			Devices::Texture target
		);
		~MainRenderContext();

		void SetExtent(const vk::Extent2D& extent);
		void Render(const vk::CommandBuffer& commandBuffer) const;

	protected:
		const Devices::DeviceContext& DeviceContext;
		bool ExtentChanged = false;
		const EngineState& EngineState;
		Devices::Texture TargetTexture;
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
		void CreateMainRenderPass();
		void CreateMainFramebufferResources();
		void CreateMainFramebuffers();
		void CreateMainPipelineLayout();
		void CreateShaderModules();
		void CreateMainGraphicPipeline();
		drk::Graphics::DrawContext BuildMainRenderPass() const;
		void PopulateDrawContext(DrawContext& drawContext, const std::vector<Draw>& draws, uint32_t drawOffset) const;
	};
}