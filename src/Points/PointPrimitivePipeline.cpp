#include "PointPrimitivePipeline.hpp"

namespace drk::Points {

	PointPrimitivePipeline::PointPrimitivePipeline(
		const Devices::DeviceContext& deviceContext,
		const vk::PipelineLayout& mainPipelineLayout,
		const entt::registry& registry,
		const Geometries::Primitives& primitives,
		const Engine::EngineState& engineState
	) :
		deviceContext(deviceContext),
		mainPipelineLayout(mainPipelineLayout),
		registry(registry),
		engineState(engineState),
		primitives(primitives),
		vertexShaderModule(deviceContext.CreateShaderModule("shaders/spv/PointPrimitive.vert.spv")),
		fragmentShaderModule(deviceContext.CreateShaderModule("shaders/spv/PointPrimitive.frag.spv")) {}
	const vk::ShaderModule& PointPrimitivePipeline::getVertexShaderModule() const {
		return vertexShaderModule;
	}
	const vk::ShaderModule& PointPrimitivePipeline::getFragmentShaderModule() const {
		return fragmentShaderModule;
	}
	vk::Pipeline
	PointPrimitivePipeline::BuildGraphicsPipeline(const vk::RenderPass& renderPass, const vk::Extent2D& extent) {
		vk::PipelineShaderStageCreateInfo vertexPipelineShaderStageCreateInfo = {
			.stage = vk::ShaderStageFlagBits::eVertex,
			.module = vertexShaderModule,
			.pName = "main"
		};
		vk::PipelineShaderStageCreateInfo fragmentPipelineShaderStageCreateInfo = {
			.stage = vk::ShaderStageFlagBits::eFragment,
			.module = fragmentShaderModule,
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
			extent,
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

		vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {
			.stageCount = static_cast<uint32_t>(pipelineShaderStageCreateInfos.size()),
			.pStages = pipelineShaderStageCreateInfos.data(),
			.pVertexInputState = &pipelineVertexInputStateCreateInfo,
			.pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo,
			.pViewportState = &pipelineViewportStateCreateInfo,
			.pRasterizationState = &pipelineRasterizationStateCreateInfo,
			.pMultisampleState = &pipelineMultisampleStateCreateInfo,
			.pDepthStencilState = &pipelineDepthStencilStateCreateInfo,
			.pColorBlendState = &pipelineColorBlendStateCreateInfo,
			.layout = mainPipelineLayout,
			.renderPass = renderPass,
		};

		auto result = deviceContext.device.createGraphicsPipeline(VK_NULL_HANDLE, graphicsPipelineCreateInfo);
		if ((VkResult) result.result != VK_SUCCESS) {
			throw new std::runtime_error("Failed to create main graphic pipeline.");
		}
		return result.value;
	}
}