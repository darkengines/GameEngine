#include <vulkan/vulkan.hpp>
#include <algorithm>

#include "SkinningPipeline.hpp"
#include "../../Graphics/Graphics.hpp"
#include "../../Objects/Models/Object.hpp"
#include "../../Cameras/Components/Camera.hpp"

namespace drk::Skinnings::Pipelines {

	SkinningPipeline::SkinningPipeline(
		const Devices::DeviceContext& deviceContext,
		Engine::EngineState& engineState,
		const Engine::DescriptorSetLayouts& descriptorSetLayouts,
		Animations::Resources::AnimationResourceManager& animationResourceManager
	) : deviceContext(deviceContext),
		animationResourceManager(animationResourceManager),
		descriptorSetLayouts{
			animationResourceManager.skinnedMeshDescriptorSetLayout,
			animationResourceManager.vertexWeightDescriptorSetLayout
		},
		pipelineLayout(createPipelineLayout(deviceContext, this->descriptorSetLayouts)),
		engineState(engineState) {
		createShaderModules();
	}

	SkinningPipeline::~SkinningPipeline() {
		deviceContext.device.destroyPipeline(pipeline);
		destroyShaderModules();
		deviceContext.device.destroyPipelineLayout(pipelineLayout);
	}

	void SkinningPipeline::destroyShaderModules() {
		deviceContext.device.destroyShaderModule(mainFragmentShaderModule);
		deviceContext.device.destroyShaderModule(skinningShaderModule);
	}

	void SkinningPipeline::createPipeline(const vk::ComputePipelineCreateInfo& computePipelineCreateInfo) {

		auto result = deviceContext.device.createComputePipeline(VK_NULL_HANDLE, computePipelineCreateInfo);
		if ((VkResult)result.result != VK_SUCCESS) {
			throw new std::runtime_error("Failed to create main graphic pipeline.");
		}
		pipeline = result.value;
	}

	void SkinningPipeline::configure(std::function<void(vk::ComputePipelineCreateInfo&)> configuration) {
		vk::PipelineShaderStageCreateInfo skinningPipelineShaderStageCreateInfo = {
			.stage = vk::ShaderStageFlagBits::eCompute,
			.module = skinningShaderModule,
			.pName = "main"
		};

		vk::ComputePipelineCreateInfo graphicPipelineCreateInfo = {
			.stage = skinningPipelineShaderStageCreateInfo,
			.layout = pipelineLayout
		};

		configuration(graphicPipelineCreateInfo);
		createPipeline(graphicPipelineCreateInfo);
	}
	void SkinningPipeline::destroyPipeline() {
		deviceContext.device.waitIdle();
		deviceContext.device.destroyPipeline(pipeline);
	}

	vk::PipelineLayout
		SkinningPipeline::createPipelineLayout(
			const Devices::DeviceContext& deviceContext,
			const std::array<vk::DescriptorSetLayout, 4>& descriptorSetLayouts
		) {
		vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
			.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size()),
			.pSetLayouts = descriptorSetLayouts.data(),
		};
		return deviceContext.device.createPipelineLayout(pipelineLayoutCreateInfo);
	}

	void SkinningPipeline::createShaderModules() {
		skinningShaderModule = deviceContext.CreateShaderModule("shaders/spv/Skinning.cs.spv");
	}
	void SkinningPipeline::bind(const vk::CommandBuffer& commandBuffer) {
		auto& frameState = engineState.getCurrentFrameState();
		std::array<vk::DescriptorSet, 2> descriptorSets{
			animationResourceManager.skinnedMeshDescriptorSet,
			animationResourceManager.vertexWeightDescriptorSet
		};
		frameState.commandBuffer.bindDescriptorSets(
			vk::PipelineBindPoint::eCompute,
			pipelineLayout,
			0,
			static_cast<uint32_t>(descriptorSets.size()),
			descriptorSets.data(),
			0,
			nullptr
		);
		commandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, pipeline);
	}

	void SkinningPipeline::bind(vk::CommandBuffer commandBuffer) {

		commandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, pipeline);
	}
}