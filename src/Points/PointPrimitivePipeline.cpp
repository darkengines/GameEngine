#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
#include <algorithm>

#include "PointPrimitivePipeline.hpp"
#include "../Graphics/Graphics.hpp"
#include "../Objects/Models/Object.hpp"
#include "../Meshes/MeshGroup.hpp"
#include "../Cameras/Components/Camera.hpp"
#include "Models/PointVertex.hpp"
#include "Models/PointDraw.hpp"

namespace drk::Points {

	PointPrimitivePipeline::PointPrimitivePipeline(
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

	PointPrimitivePipeline::~PointPrimitivePipeline() {
		deviceContext.device.destroyPipeline(pipeline);
		destroyShaderModules();
		deviceContext.device.destroyPipelineLayout(pipelineLayout);
	}

	void PointPrimitivePipeline::destroyShaderModules() {
		deviceContext.device.destroyShaderModule(mainFragmentShaderModule);
		deviceContext.device.destroyShaderModule(mainVertexShaderModule);
	}

	void PointPrimitivePipeline::createPipeline(const vk::GraphicsPipelineCreateInfo& graphicPipelineCreateInfo) {

		auto result = deviceContext.device.createGraphicsPipeline(VK_NULL_HANDLE, graphicPipelineCreateInfo);
		if ((VkResult)result.result != VK_SUCCESS) {
			throw new std::runtime_error("Failed to create main graphic pipeline.");
		}
		pipeline = result.value;
	}

	void PointPrimitivePipeline::configure(std::function<void(vk::GraphicsPipelineCreateInfo&)> configuration) {
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
		std::vector<vk::VertexInputBindingDescription> vertexInputBindingDescriptions = Models::PointVertex::getBindingDescriptions();
		std::vector<vk::VertexInputAttributeDescription> vertexInputAttributeDescriptions = Models::PointVertex::getAttributeDescriptions();
		vk::Viewport viewport;
		vk::Rect2D scissor;

		const auto& pipelineVertexInputStateCreateInfo = Graphics::Graphics::DefaultPipelineVertexInputStateCreateInfo(
			vertexInputBindingDescriptions,
			vertexInputAttributeDescriptions
		);
		auto pipelineInputAssemblyStateCreateInfo = Graphics::Graphics::DefaultPipelineInputAssemblyStateCreateInfo();
		pipelineInputAssemblyStateCreateInfo.topology = vk::PrimitiveTopology::ePointList;

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
			.pVertexInputState = &pipelineVertexInputStateCreateInfo,
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
	void PointPrimitivePipeline::destroyPipeline() {
		deviceContext.device.waitIdle();
		deviceContext.device.destroyPipeline(pipeline);
	}

	vk::PipelineLayout
		PointPrimitivePipeline::createPipelineLayout(
			const Devices::DeviceContext& deviceContext,
			const std::array<vk::DescriptorSetLayout, 4>& descriptorSetLayouts
		) {
		vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
			.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size()),
			.pSetLayouts = descriptorSetLayouts.data(),
		};
		return deviceContext.device.createPipelineLayout(pipelineLayoutCreateInfo);
	}

	void PointPrimitivePipeline::createShaderModules() {
		mainVertexShaderModule = deviceContext.CreateShaderModule("shaders/spv/Point.vert.spv");
		mainFragmentShaderModule = deviceContext.CreateShaderModule("shaders/spv/Point.frag.spv");
	}
	void PointPrimitivePipeline::bind(const vk::CommandBuffer& commandBuffer) {
		auto& frameState = engineState.getCurrentFrameState();
		const auto& drawDescriptorSet = frameState.getUniformStore<Models::PointDraw>().descriptorSet;
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

	//PointPrimitivePipeline::PointPrimitivePipeline(
	//	const Devices::DeviceContext& deviceContext,
	//	const vk::PipelineLayout& mainPipelineLayout,
	//	const entt::registry& registry,
	//	const Geometries::Primitives& primitives,
	//	const Engine::EngineState& engineState
	//) :
	//	deviceContext(deviceContext),
	//	mainPipelineLayout(mainPipelineLayout),
	//	registry(registry),
	//	engineState(engineState),
	//	primitives(primitives),
	//	vertexShaderModule(deviceContext.CreateShaderModule("shaders/spv/PointPrimitive.vert.spv")),
	//	fragmentShaderModule(deviceContext.CreateShaderModule("shaders/spv/PointPrimitive.frag.spv")) {}
	//const vk::ShaderModule& PointPrimitivePipeline::getVertexShaderModule() const {
	//	return vertexShaderModule;
	//}
	//const vk::ShaderModule& PointPrimitivePipeline::getFragmentShaderModule() const {
	//	return fragmentShaderModule;
	//}
	//vk::Pipeline
	//	PointPrimitivePipeline::BuildGraphicsPipeline(const vk::RenderPass& renderPass, const vk::Extent2D& extent) {
	//	vk::PipelineShaderStageCreateInfo vertexPipelineShaderStageCreateInfo = {
	//		.stage = vk::ShaderStageFlagBits::eVertex,
	//		.module = vertexShaderModule,
	//		.pName = "main"
	//	};
	//	vk::PipelineShaderStageCreateInfo fragmentPipelineShaderStageCreateInfo = {
	//		.stage = vk::ShaderStageFlagBits::eFragment,
	//		.module = fragmentShaderModule,
	//		.pName = "main"
	//	};

	//	std::vector<vk::PipelineShaderStageCreateInfo> pipelineShaderStageCreateInfos = {
	//		vertexPipelineShaderStageCreateInfo,
	//		fragmentPipelineShaderStageCreateInfo
	//	};

	//	vk::PipelineColorBlendAttachmentState pipelineColorBlendAttachmentState;
	//	std::vector<vk::VertexInputBindingDescription> vertexInputBindingDescriptions = Models::PointVertex::getBindingDescriptions();
	//	std::vector<vk::VertexInputAttributeDescription> vertexInputAttributeDescriptions = Models::PointVertex::getAttributeDescriptions();
	//	vk::Viewport viewport;
	//	vk::Rect2D scissor;

	//	const auto& pipelineVertexInputStateCreateInfo = Graphics::Graphics::DefaultPipelineVertexInputStateCreateInfo(
	//		vertexInputBindingDescriptions,
	//		vertexInputAttributeDescriptions
	//	);
	//	auto pipelineInputAssemblyStateCreateInfo = Graphics::Graphics::DefaultPipelineInputAssemblyStateCreateInfo();
	//	pipelineInputAssemblyStateCreateInfo.topology = vk::PrimitiveTopology::ePointList;

	//	const auto& pipelineViewportStateCreateInfo = Graphics::Graphics::DefaultPipelineViewportStateCreateInfo(
	//		extent,
	//		viewport,
	//		scissor
	//	);
	//	const auto& pipelineRasterizationStateCreateInfo = Graphics::Graphics::DefaultPipelineRasterizationStateCreateInfo();
	//	auto pipelineMultisampleStateCreateInfo = Graphics::Graphics::DefaultPipelineMultisampleStateCreateInfo();
	//	//TODO: Use configurable sample count
	//	pipelineMultisampleStateCreateInfo.rasterizationSamples = vk::SampleCountFlagBits::e8;
	//	const auto& pipelineColorBlendStateCreateInfo = Graphics::Graphics::DefaultPipelineColorBlendStateCreateInfo(
	//		pipelineColorBlendAttachmentState
	//	);
	//	const auto& pipelineDepthStencilStateCreateInfo = Graphics::Graphics::DefaultPipelineDepthStencilStateCreateInfo();

	//	vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {
	//		.stageCount = static_cast<uint32_t>(pipelineShaderStageCreateInfos.size()),
	//		.pStages = pipelineShaderStageCreateInfos.data(),
	//		.pVertexInputState = &pipelineVertexInputStateCreateInfo,
	//		.pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo,
	//		.pViewportState = &pipelineViewportStateCreateInfo,
	//		.pRasterizationState = &pipelineRasterizationStateCreateInfo,
	//		.pMultisampleState = &pipelineMultisampleStateCreateInfo,
	//		.pDepthStencilState = &pipelineDepthStencilStateCreateInfo,
	//		.pColorBlendState = &pipelineColorBlendStateCreateInfo,
	//		.layout = mainPipelineLayout,
	//		.renderPass = renderPass,
	//	};

	//	auto result = deviceContext.device.createGraphicsPipeline(VK_NULL_HANDLE, graphicsPipelineCreateInfo);
	//	if ((VkResult)result.result != VK_SUCCESS) {
	//		throw new std::runtime_error("Failed to create main graphic pipeline.");
	//	}
	//	return result.value;
	//}
}