#include <vulkan/vulkan.hpp>
#include "../Models/VertexWeightPipelineOptions.hpp"
#include "../Models/SkinningInput.hpp"
#include <algorithm>

#include "VertexWeightPipeline.hpp"
#include "../../Graphics/Graphics.hpp"
#include "../../Objects/Models/Object.hpp"
#include "../../Cameras/Components/Camera.hpp"

namespace drk::Animations::Pipelines {
	VertexWeightPipeline::VertexWeightPipeline(
		const Devices::DeviceContext& deviceContext,
		Engine::EngineState& engineState,
		Animations::Resources::AnimationResourceManager& animationResourceManager,
		Engine::DescriptorSetLayouts& descriptorSetLayouts
	) : deviceContext(deviceContext),
		animationResourceManager(animationResourceManager),
		descriptorSetLayouts{
			descriptorSetLayouts.storeDescriptorSetLayout,
			descriptorSetLayouts.storeDescriptorSetLayout,
			animationResourceManager.skinnedMeshDescriptorSetLayout,
			animationResourceManager.vertexWeightDescriptorSetLayout
		},
		pipelineLayout(createPipelineLayout(deviceContext, this->descriptorSetLayouts)),
		engineState(engineState) {
		createShaderModules();
		configure([](auto& config) {});
	}

	VertexWeightPipeline::~VertexWeightPipeline() {
		deviceContext.device.destroyPipeline(pipeline);
		destroyShaderModules();
		deviceContext.device.destroyPipelineLayout(pipelineLayout);
	}

	void VertexWeightPipeline::destroyShaderModules() {
		deviceContext.device.destroyShaderModule(mainFragmentShaderModule);
		deviceContext.device.destroyShaderModule(skinningShaderModule);
	}

	void VertexWeightPipeline::createPipeline(const vk::ComputePipelineCreateInfo& computePipelineCreateInfo) {

		auto result = deviceContext.device.createComputePipeline(VK_NULL_HANDLE, computePipelineCreateInfo);
		if ((VkResult)result.result != VK_SUCCESS) {
			throw new std::runtime_error("Failed to create main graphic pipeline.");
		}
		pipeline = result.value;
	}

	void VertexWeightPipeline::configure(std::function<void(vk::ComputePipelineCreateInfo&)> configuration) {
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
	void VertexWeightPipeline::destroyPipeline() {
		deviceContext.device.waitIdle();
		deviceContext.device.destroyPipeline(pipeline);
	}

	vk::PipelineLayout
		VertexWeightPipeline::createPipelineLayout(
			const Devices::DeviceContext& deviceContext,
			const std::array<vk::DescriptorSetLayout, 4>& descriptorSetLayouts
		) {
		vk::PushConstantRange optionsPushConstant{
			.stageFlags = vk::ShaderStageFlagBits::eCompute,
			.offset = 0,
			.size = sizeof(Models::VertexWeightPipelineOptions)
		};
		
		vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
			.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size()),
			.pSetLayouts = descriptorSetLayouts.data(),
			.pushConstantRangeCount = 1,
			.pPushConstantRanges = &optionsPushConstant
		};

		return deviceContext.device.createPipelineLayout(pipelineLayoutCreateInfo);
	}

	void VertexWeightPipeline::createShaderModules() {
		skinningShaderModule = deviceContext.CreateShaderModule("shaders/spv/VertexWeight.comp.spv");
	}
	void VertexWeightPipeline::bind(const vk::CommandBuffer& commandBuffer) {
		auto& frameState = engineState.getCurrentFrameState();
		const auto& inputDescriptorSet = frameState.getUniformStore<Models::SkinningInput>().descriptorSet;
		std::array<vk::DescriptorSet, 4> descriptorSets{
			inputDescriptorSet,
			engineState.getCurrentFrameState().storeDescriptorSet,
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
}