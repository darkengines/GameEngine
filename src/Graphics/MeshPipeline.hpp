#pragma once
#include "../Devices/Device.hpp"
#include "../Devices/DeviceContext.hpp"
#include "../Engine/EngineState.hpp"
#include "DrawContext.hpp"

namespace drk::Graphics {
	class MeshPipeline {
	public:
		MeshPipeline(
			const Devices::DeviceContext& deviceContext,
			Engine::EngineState& engineState,
			entt::registry& registry,
			const Engine::DescriptorSetLayouts& descriptorSetLayouts
		);
		~MeshPipeline();

		void setTarget(const Devices::Texture& texture);
		void render(const vk::CommandBuffer& commandBuffer) const;

	protected:
		entt::registry& registry;
		const Devices::DeviceContext& deviceContext;
		bool ExtentChanged = false;
		Engine::EngineState& engineState;
		std::optional<Devices::Texture> targetTexture;
		vk::ShaderModule mainVertexShaderModule;
		vk::ShaderModule mainFragmentShaderModule;
		vk::RenderPass renderPass;

		Devices::Texture colorTexture;
		Devices::Texture depthTexture;

		vk::Framebuffer framebuffer;
		vk::Pipeline pipeline;
		std::array<vk::DescriptorSetLayout, 4> descriptorSetLayouts;
		vk::PipelineLayout pipelineLayout;

		void createFramebufferResources();
		void destroyFramebufferResources();

		void createShaderModules();
		void destroyShaderModules();

		void createFramebuffer();
		void destroyFramebuffer();

		void createRenderPass();
		void createPipeline();

		void recreatePipeLine();

		static vk::PipelineLayout createPipelineLayout(
			const Devices::DeviceContext& deviceContext,
			const std::array<vk::DescriptorSetLayout, 4>& descriptorSetLayouts
		);

		drk::Graphics::DrawContext BuildMainRenderPass() const;
		void
		PopulateDrawContext(DrawContext& drawContext, const std::vector<Draw>& draws, uint32_t drawOffset) const;
	};
}