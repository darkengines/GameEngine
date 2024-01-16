#include <vulkan/vulkan.hpp>
#include <algorithm>
#include "ShadowMeshPipeline.hpp"
#include "../../Graphics/Graphics.hpp"
#include "../Components/ShadowMeshDraw.hpp"

namespace drk::Meshes::Pipelines {
	ShadowMeshPipeline::ShadowMeshPipeline(
		const Devices::DeviceContext& deviceContext,
		Engine::EngineState& engineState,
		const Engine::DescriptorSetLayouts& descriptorSetLayouts
	) : deviceContext(deviceContext),
		descriptorSetLayouts{
			descriptorSetLayouts.textureDescriptorSetLayout,
			descriptorSetLayouts.storeDescriptorSetLayout,
			descriptorSetLayouts.globalDescriptorSetLayout,
			descriptorSetLayouts.storeDescriptorSetLayout,
	},
	pipelineLayout(createPipelineLayout(deviceContext, this->descriptorSetLayouts)),
	engineState(engineState) {
		createShaderModules();
	}

	ShadowMeshPipeline::~ShadowMeshPipeline() {
		deviceContext.device.destroyPipeline(pipeline);
		destroyShaderModules();
		deviceContext.device.destroyPipelineLayout(pipelineLayout);
	}

	void ShadowMeshPipeline::destroyShaderModules() {
		deviceContext.device.destroyShaderModule(mainVertexShaderModule);
		deviceContext.device.destroyShaderModule(mainFragmentShaderModule);
	}

	void ShadowMeshPipeline::createPipeline(const vk::GraphicsPipelineCreateInfo& graphicPipelineCreateInfo) {

		auto result = deviceContext.device.createGraphicsPipeline(VK_NULL_HANDLE, graphicPipelineCreateInfo);
		if ((VkResult)result.result != VK_SUCCESS) {
			throw new std::runtime_error("Failed to create main graphic pipeline.");
		}
		pipeline = result.value;
	}

	Draws::Components::DrawVertexBufferInfo ShadowMeshPipeline::getBufferInfo(const entt::registry& registry, entt::entity drawEntity) const {
		const auto& meshDraw = registry.get<Components::ShadowMeshDraw>(drawEntity);
		Draws::Components::DrawVertexBufferInfo bufferInfo{
			static_cast<uint32_t>(meshDraw.meshResource->indices.size()),
			static_cast<uint32_t>(meshDraw.meshBufferView.IndexBufferView.byteOffset / sizeof(VertexIndex)),
			static_cast<int32_t>(meshDraw.meshBufferView.VertexBufferView.byteOffset / sizeof(Vertex))
		};
		return bufferInfo;
	}

	void ShadowMeshPipeline::configure(std::function<void(vk::GraphicsPipelineCreateInfo&)> configuration) {
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
		std::vector<vk::VertexInputBindingDescription> vertexInputBindingDescriptions = Meshes::Vertex::getBindingDescriptions();
		std::vector<vk::VertexInputAttributeDescription> vertexInputAttributeDescriptions = Meshes::Vertex::getAttributeDescriptions();
		vk::Viewport viewport;
		vk::Rect2D scissor;

		const auto& pipelineVertexInputStateCreateInfo = Graphics::Graphics::DefaultPipelineVertexInputStateCreateInfo(
			vertexInputBindingDescriptions,
			vertexInputAttributeDescriptions
		);
		const auto& pipelineInputAssemblyStateCreateInfo = Graphics::Graphics::DefaultPipelineInputAssemblyStateCreateInfo();
		const auto& pipelineViewportStateCreateInfo = Graphics::Graphics::DefaultPipelineViewportStateCreateInfo(
			{ 1024u, 768u },
			viewport,
			scissor
		);
		const auto& pipelineRasterizationStateCreateInfo = Graphics::Graphics::DefaultPipelineRasterizationStateCreateInfo();
		auto pipelineMultisampleStateCreateInfo = Graphics::Graphics::DefaultPipelineMultisampleStateCreateInfo();
		//TODO: Use configurable sample count
		pipelineMultisampleStateCreateInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;
		const auto& pipelineColorBlendStateCreateInfo = Graphics::Graphics::DefaultPipelineColorBlendStateCreateInfo(
			pipelineColorBlendAttachmentState
		);
		const auto& pipelineDepthStencilStateCreateInfo = Graphics::Graphics::DefaultPipelineDepthStencilStateCreateInfo();

		std::vector<vk::DynamicState> dynamicStates{ vk::DynamicState::eScissor, vk::DynamicState::eViewport };

		vk::PipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{
			.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
			.pDynamicStates = dynamicStates.data()
		};

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
			.pDynamicState = &pipelineDynamicStateCreateInfo,
			.layout = pipelineLayout,
		};

		configuration(graphicPipelineCreateInfo);
		createPipeline(graphicPipelineCreateInfo);
	}
	void ShadowMeshPipeline::destroyPipeline() {
		deviceContext.device.destroyPipeline(pipeline);
	}

	vk::PipelineLayout
		ShadowMeshPipeline::createPipelineLayout(
			const Devices::DeviceContext& deviceContext,
			const std::array<vk::DescriptorSetLayout, 4>& descriptorSetLayouts
		) {
		vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
			.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size()),
			.pSetLayouts = descriptorSetLayouts.data(),
		};
		return deviceContext.device.createPipelineLayout(pipelineLayoutCreateInfo);
	}
	void ShadowMeshPipeline::createShaderModules() {
		mainVertexShaderModule = deviceContext.CreateShaderModule("shaders/spv/ShadowMesh.vert.spv");
		mainFragmentShaderModule = deviceContext.CreateShaderModule("shaders/spv/ShadowMesh.frag.spv");
	}
	void ShadowMeshPipeline::bind(const vk::CommandBuffer& commandBuffer) {
		auto& frameState = engineState.getCurrentFrameState();
		const auto& drawDescriptorSet = frameState.getUniformStore<Models::ShadowMeshDraw>().descriptorSet;
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