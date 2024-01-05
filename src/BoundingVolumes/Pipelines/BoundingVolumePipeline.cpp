#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
#include <algorithm>

#include "BoundingVolumePipeline.hpp"
#include "../../Graphics/Graphics.hpp"
#include "../../Objects/Models/Object.hpp"
#include "../../Cameras/Components/Camera.hpp"
#include "../Models/Vertex.hpp"
#include "../Models/BoundingVolumeDraw.hpp"
#include "../../Draws/Models/Draw.hpp"

namespace drk::BoundingVolumes::Pipelines {

	BoundingVolumePipeline::BoundingVolumePipeline(
		const Devices::DeviceContext& deviceContext,
		Engine::EngineState& engineState,
		const Engine::DescriptorSetLayouts& descriptorSetLayouts
	) : deviceContext(deviceContext),
		descriptorSetLayouts{
			descriptorSetLayouts.textureDescriptorSetLayout,
			descriptorSetLayouts.storeDescriptorSetLayout,
			descriptorSetLayouts.globalDescriptorSetLayout,
			descriptorSetLayouts.storeDescriptorSetLayout
	},
		pipelineLayout(createPipelineLayout(deviceContext, this->descriptorSetLayouts)),
		engineState(engineState) {
		createShaderModules();
	}

	BoundingVolumePipeline::~BoundingVolumePipeline() {
		deviceContext.device.destroyPipeline(pipeline);
		destroyShaderModules();
		deviceContext.device.destroyPipelineLayout(pipelineLayout);
	}

	void BoundingVolumePipeline::destroyShaderModules() {
		deviceContext.device.destroyShaderModule(mainFragmentShaderModule);
		deviceContext.device.destroyShaderModule(mainVertexShaderModule);
	}

	void BoundingVolumePipeline::createPipeline(const vk::GraphicsPipelineCreateInfo& graphicPipelineCreateInfo) {

		auto result = deviceContext.device.createGraphicsPipeline(VK_NULL_HANDLE, graphicPipelineCreateInfo);
		if ((VkResult)result.result != VK_SUCCESS) {
			throw new std::runtime_error("Failed to create main graphic pipeline.");
		}
		pipeline = result.value;
	}

	void BoundingVolumePipeline::configure(std::function<void(vk::GraphicsPipelineCreateInfo&)> configuration) {
		vk::PipelineShaderStageCreateInfo vertexPipelineShaderStageCreateInfo = {
			.stage = vk::ShaderStageFlagBits::eVertex,
			.module = mainVertexShaderModule,
			.pName = "main"
		};
		vk::PipelineShaderStageCreateInfo fragmentPipelineShaderStageCreateInfo = {
			.stage = vk::ShaderStageFlagBits::eFragment,
			.module = mainFragmentShaderModule,
			.pName = "main"
		};

		std::vector<vk::PipelineShaderStageCreateInfo> pipelineShaderStageCreateInfos = {
			vertexPipelineShaderStageCreateInfo,
			fragmentPipelineShaderStageCreateInfo
		};

		vk::PipelineColorBlendAttachmentState pipelineColorBlendAttachmentState;
		std::vector<vk::VertexInputBindingDescription> vertexInputBindingDescriptions = Models::Vertex::getBindingDescriptions();
		std::vector<vk::VertexInputAttributeDescription> vertexInputAttributeDescriptions = Models::Vertex::getAttributeDescriptions();
		vk::Viewport viewport;
		vk::Rect2D scissor;

		const auto& pipeVertexInputStateCreateInfo = Graphics::Graphics::DefaultPipelineVertexInputStateCreateInfo(
			vertexInputBindingDescriptions,
			vertexInputAttributeDescriptions
		);
		auto pipelineInputAssemblyStateCreateInfo = Graphics::Graphics::DefaultPipelineInputAssemblyStateCreateInfo();
		pipelineInputAssemblyStateCreateInfo.topology = vk::PrimitiveTopology::eLineList;

		const auto& pipelineViewportStateCreateInfo = Graphics::Graphics::DefaultPipelineViewportStateCreateInfo(
			{ 1024u, 768u },
			viewport,
			scissor
		);
		const auto& pipelineRasterizationStateCreateInfo = Graphics::Graphics::DefaultPipelineRasterizationStateCreateInfo();
		auto pipelineMultisampleStateCreateInfo = Graphics::Graphics::DefaultPipelineMultisampleStateCreateInfo();
		//TODO: Use configurable sample count
		pipelineMultisampleStateCreateInfo.rasterizationSamples = vk::SampleCountFlagBits::e8;
		const auto& pipelineColorBlendStateCreateInfo = Graphics::Graphics::DefaultPipelineColorBlendStateCreateInfo(
			pipelineColorBlendAttachmentState
		);
		const auto& pipelineDepthStencilStateCreateInfo = Graphics::Graphics::DefaultPipelineDepthStencilStateCreateInfo();

		vk::GraphicsPipelineCreateInfo graphicPipelineCreateInfo = {
			.stageCount = static_cast<uint32_t>(pipelineShaderStageCreateInfos.size()),
			.pStages = pipelineShaderStageCreateInfos.data(),
			.pVertexInputState = &pipeVertexInputStateCreateInfo,
			.pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo,
			.pViewportState = &pipelineViewportStateCreateInfo,
			.pRasterizationState = &pipelineRasterizationStateCreateInfo,
			.pMultisampleState = &pipelineMultisampleStateCreateInfo,
			.pDepthStencilState = &pipelineDepthStencilStateCreateInfo,
			.pColorBlendState = &pipelineColorBlendStateCreateInfo,
			.layout = pipelineLayout,
		};

		configuration(graphicPipelineCreateInfo);
		createPipeline(graphicPipelineCreateInfo);
	}
	void BoundingVolumePipeline::destroyPipeline() {
		deviceContext.device.waitIdle();
		deviceContext.device.destroyPipeline(pipeline);
	}
	Draws::Components::DrawVertexBufferInfo BoundingVolumePipeline::getBufferInfo(const entt::registry& registry, entt::entity drawEntity) const {
		return Draws::Components::DrawVertexBufferInfo{ 24, 0, 0 };
	}
	vk::PipelineLayout
		BoundingVolumePipeline::createPipelineLayout(
			const Devices::DeviceContext& deviceContext,
			const std::array<vk::DescriptorSetLayout, 4>& descriptorSetLayouts
		) {
		vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
			.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size()),
			.pSetLayouts = descriptorSetLayouts.data(),
		};
		return deviceContext.device.createPipelineLayout(pipelineLayoutCreateInfo);
	}

	void BoundingVolumePipeline::createShaderModules() {
		mainVertexShaderModule = deviceContext.CreateShaderModule("shaders/spv/BoundingVolume.vert.spv");
		mainFragmentShaderModule = deviceContext.CreateShaderModule("shaders/spv/BoundingVolume.frag.spv");
	}
	void BoundingVolumePipeline::bind(const vk::CommandBuffer& commandBuffer) {
		auto& frameState = engineState.getCurrentFrameState();
		const auto& drawDescriptorSet = frameState.getUniformStore<BoundingVolumes::Models::BoundingVolumeDraw>().descriptorSet;
		std::array<vk::DescriptorSet, 4> descriptorSets{
			engineState.textureDescriptorSet,
			drawDescriptorSet,
			frameState.globalDescriptorSet,
			frameState.storeDescriptorSet
		};
		frameState.commandBuffer.bindDescriptorSets(
			vk::PipelineBindPoint::eGraphics,
			pipelineLayout,
			0,
			static_cast<uint32_t>(descriptorSets.size()),
			descriptorSets.data(),
			0,
			nullptr
		);
		commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
	}
}