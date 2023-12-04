#pragma once
#include "../Devices/Device.hpp"
#include "../Devices/DeviceContext.hpp"
#include "../Engine/EngineState.hpp"
#include "../Pipelines/Pipeline.hpp"

namespace drk::Lines {
	class LinePipeline : public drk::Pipelines::Pipeline {
	public:
		LinePipeline(
			const Devices::DeviceContext& deviceContext,
			Engine::EngineState& engineState,
			const Engine::DescriptorSetLayouts& descriptorSetLayouts
		);
		~LinePipeline();

		void bind(const vk::CommandBuffer& commandBuffer);
		void configure(std::function<void(vk::GraphicsPipelineCreateInfo&)> configure);
		void destroyPipeline();

	protected:
		const Devices::DeviceContext& deviceContext;
		const Engine::EngineState& engineState;
		vk::ShaderModule mainVertexShaderModule;
		vk::ShaderModule mainFragmentShaderModule;
		vk::Pipeline pipeline;
		std::array<vk::DescriptorSetLayout, 4> descriptorSetLayouts;
		vk::PipelineLayout pipelineLayout;

		void createShaderModules();
		void destroyShaderModules();

		void createPipeline(const vk::GraphicsPipelineCreateInfo& graphicPipelineCreateInfo);

		static vk::PipelineLayout createPipelineLayout(
			const Devices::DeviceContext& deviceContext,
			const std::array<vk::DescriptorSetLayout, 4>& descriptorSetLayouts
		);
		vk::GraphicsPipelineCreateInfo getDefaultGraphicPipelineCreateInfo();
	};
}