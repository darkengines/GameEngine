#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
#include <algorithm>

#include "MeshPipeline.hpp"
#include "../../Graphics/Graphics.hpp"
#include "../../Objects/Models/Object.hpp"
#include "../../Meshes/MeshGroup.hpp"
#include "../../Cameras/Camera.hpp"
#include "../../Meshes/Models/Mesh.hpp"

namespace drk::Meshes::Pipelines {
	MeshPipeline::MeshPipeline(
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

	MeshPipeline::~MeshPipeline() {
		deviceContext.device.destroyPipeline(pipeline);
		destroyShaderModules();
		deviceContext.device.destroyPipelineLayout(pipelineLayout);
	}

	void MeshPipeline::destroyShaderModules() {
		deviceContext.device.destroyShaderModule(mainFragmentShaderModule);
		deviceContext.device.destroyShaderModule(mainVertexShaderModule);
	}

	void MeshPipeline::createPipeline() {
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
			{targetTexture->imageCreateInfo.extent.width, targetTexture->imageCreateInfo.extent.height},
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
			.layout = pipelineLayout,
			.renderPass = renderPass,
		};

		auto result = deviceContext.device.createGraphicsPipeline(VK_NULL_HANDLE, graphicsPipelineCreateInfo);
		if ((VkResult) result.result != VK_SUCCESS) {
			throw new std::runtime_error("Failed to create main graphic pipeline.");
		}
		pipeline = result.value;
	}

	vk::PipelineLayout
	MeshPipeline::createPipelineLayout(
		const Devices::DeviceContext& deviceContext,
		const std::array<vk::DescriptorSetLayout, 4>& descriptorSetLayouts
	) {
		vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
			.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size()),
			.pSetLayouts = descriptorSetLayouts.data(),
		};
		return deviceContext.device.createPipelineLayout(pipelineLayoutCreateInfo);
	}

	void MeshPipeline::createShaderModules() {
		mainVertexShaderModule = deviceContext.CreateShaderModule("shaders/spv/main.vert.spv");
		mainFragmentShaderModule = deviceContext.CreateShaderModule("shaders/spv/main.frag.spv");
	}
	void MeshPipeline::bind(const vk::CommandBuffer& commandBuffer) const {
		auto& frameState = engineState.getCurrentFrameState();
		const auto& drawDescriptorSet = frameState.getUniformStore<Models::MeshDraw>().descriptorSet;
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
			descriptorSets.size(),
			descriptorSets.data(),
			0,
			nullptr
		);
		commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
	}
	void MeshPipeline::render(
		const vk::CommandBuffer& commandBuffer,
		std::vector<std::unique_ptr<Draws::Draw<Models::MeshDraw>>>& draws
	) const {
		auto& frameState = engineState.getCurrentFrameState();
		vk::RenderPassBeginInfo mainRenderPassBeginInfo = {
			.renderPass = renderPass,
			.framebuffer = framebuffer,
			.renderArea = {
				0,
				0,
				{
					targetTexture->imageCreateInfo.extent.width,
					targetTexture->imageCreateInfo.extent.height
				}},
			.clearValueCount = 0u,
			.pClearValues = nullptr,
		};
		commandBuffer.beginRenderPass(mainRenderPassBeginInfo, vk::SubpassContents::eInline);

		if (!draws.empty()) {
			std::sort(
				draws.begin(),
				draws.end(),
				[](
					const std::unique_ptr<Draws::Draw<Models::MeshDraw>>& left,
					const std::unique_ptr<Draws::Draw<Models::MeshDraw>>& right
				) {
					return left->indexBufferView.buffer.buffer == right->indexBufferView.buffer.buffer;
				}
			);
			for (auto drawSetIndex = 0u; drawSetIndex < drawContext.drawSets.size(); drawSetIndex++) {
				const auto& drawSet = drawContext.drawSets[drawSetIndex];

				vk::DeviceSize offset = 0u;
				commandBuffer.bindIndexBuffer(drawSet.indexBuffer.buffer, 0, vk::IndexType::eUint32);
				commandBuffer.bindVertexBuffers(0, 1, &drawSet.vertexBuffer.buffer, &offset);
				for (auto drawIndex = 0u; drawIndex < drawSet.drawCommands.size(); drawIndex++) {
					const auto& drawCommand = drawSet.drawCommands[drawIndex];
					commandBuffer.drawIndexed(
						drawCommand.indexCount,
						drawCommand.instanceCount,
						drawCommand.firstIndex,
						drawCommand.vertexOffset,
						drawCommand.firstInstance
					);
				}
			}
			commandBuffer.endRenderPass();
		}
	}

	std::vector<Draws::DrawSet> MeshPipeline::PrepareDraws() {
		auto objectEntities = registry.view<Stores::StoreItem<Objects::Models::Object>, Meshes::MeshGroup, Spatials::Spatial>();
		std::vector<std::unique_ptr<Draws::GenericDraw>> draws;
		auto cameraEntity = engineState.CameraEntity;
		auto camera = registry.get<Cameras::Camera>(cameraEntity);
		objectEntities.each(
			[&](
				entt::entity objectEntity,
				auto& objectStoreItem,
				auto& meshGroup,
				auto& spatial
			) {
				const auto& objectStoreItemLocation = objectStoreItem.frameStoreItems[engineState.getFrameIndex()];
				for (const auto& meshEntity: meshGroup.meshEntities) {
					Meshes::MeshInfo* meshInfo = registry.get<Meshes::MeshInfo*>(meshEntity);
					const Meshes::Mesh mesh = registry.get<Meshes::Mesh>(meshEntity);
					const Stores::StoreItem<Meshes::Models::Mesh> meshStoreItem = registry.get<Stores::StoreItem<Meshes::Models::Mesh>>(
						meshEntity
					);
					const auto& meshStoreItemLocation = meshStoreItem.frameStoreItems[engineState.getFrameIndex()];
					Draws::Draw<Models::MeshDraw> draw = {
						.drawModel = {.meshStoreItem = meshStoreItemLocation, .objectLocation = objectStoreItemLocation},
						.pipeline = pipeline,
						.indexBufferView = mesh.IndexBufferView,
						.vertexBufferView = mesh.VertexBufferView,
						.hasTransparency = meshInfo->pMaterial->hasTransparency,
						.depth = glm::distance(camera.absolutePosition, spatial.absolutePosition)
					};
					draws.emplace_back(std::make_unique(draw));
				}
			}
		);
		return draws;
	}

	void MeshPipeline::recreatePipeLine() {
		if ((VkPipeline) pipeline != VK_NULL_HANDLE) {
			deviceContext.device.destroyPipeline(pipeline);
		}
		createPipeline();
	}
}