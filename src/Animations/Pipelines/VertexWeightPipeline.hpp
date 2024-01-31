#pragma once
#include "../../Devices/Device.hpp"
#include "../../Devices/DeviceContext.hpp"
#include "../../Engine/EngineState.hpp"
#include "../../Pipelines/ComputePipeline.hpp"
#include "../Resources/AnimationResourceManager.hpp"

namespace drk::Animations::Pipelines {
	class VertexWeightPipeline : public drk::Pipelines::ComputePipeline {
	public:
		VertexWeightPipeline(
			const Devices::DeviceContext& deviceContext,
			Engine::EngineState& engineState,
			Animations::Resources::AnimationResourceManager& animationResourceManager,
			Engine::DescriptorSetLayouts& descriptorSetLayouts
		);
		~VertexWeightPipeline();

		void configure(std::function<void(vk::ComputePipelineCreateInfo&)> configure) override;
		//Draws::Components::DrawVertexBufferInfo getBufferInfo(const entt::registry& registry, entt::entity drawEntity) const;
		void destroyPipeline() override;
		void bind(const vk::CommandBuffer& commandBuffer) override;

	protected:
		const Devices::DeviceContext& deviceContext;
		const Engine::EngineState& engineState;
		vk::ShaderModule skinningShaderModule;
		vk::ShaderModule mainFragmentShaderModule;
		vk::Pipeline pipeline;
		std::array<vk::DescriptorSetLayout, 4> descriptorSetLayouts;
		vk::PipelineLayout pipelineLayout;
		Animations::Resources::AnimationResourceManager& animationResourceManager;

		void createShaderModules();
		void destroyShaderModules();

		void createPipeline(const vk::ComputePipelineCreateInfo& graphicPipelineCreateInfo);

		static vk::PipelineLayout createPipelineLayout(
			const Devices::DeviceContext& deviceContext,
			const std::array<vk::DescriptorSetLayout, 4>& descriptorSetLayouts
		);
	};
}