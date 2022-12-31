#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include "MeshPipeline.hpp"
#include "../../Graphics/Graphics.hpp"
#include "../../Objects/Models/Object.hpp"
#include "../../Meshes/MeshGroup.hpp"
#include "../../Cameras/Camera.hpp"
#include "../../Meshes/Models/Mesh.hpp"

namespace drk::Meshes::Pipelines {
	MeshPipeline::MeshPipeline(
		const Devices::DeviceContext& deviceContext,
		drk::Engine::EngineState& engineState,
		entt::registry& registry,
		const Engine::DescriptorSetLayouts& descriptorSetLayouts
	)
		: registry(registry),
		  deviceContext(deviceContext),
		  engineState(engineState),
		  descriptorSetLayouts{
			  descriptorSetLayouts.textureDescriptorSetLayout,
			  descriptorSetLayouts.storeDescriptorSetLayout,
			  descriptorSetLayouts.globalDescriptorSetLayout,
			  descriptorSetLayouts.storeDescriptorSetLayout,
		  },
		  pipelineLayout(createPipelineLayout(deviceContext, this->descriptorSetLayouts)) {
		createShaderModules();
	}

	MeshPipeline::~MeshPipeline() {
		deviceContext.device.destroyPipeline(pipeline);

		destroyShaderModules();
		deviceContext.device.destroyPipelineLayout(pipelineLayout);
		destroyFramebuffer();
		deviceContext.device.destroyRenderPass(renderPass);

		destroyFramebufferResources();
	}

	void MeshPipeline::destroyShaderModules() {
		deviceContext.device.destroyShaderModule(mainFragmentShaderModule);
		deviceContext.device.destroyShaderModule(mainVertexShaderModule);
	}

	void MeshPipeline::destroyFramebuffer() {
		deviceContext.device.destroyFramebuffer(framebuffer);
	}

	void MeshPipeline::destroyFramebufferResources() {
		deviceContext.destroyTexture(depthTexture);
		deviceContext.destroyTexture(colorTexture);
	}

	void MeshPipeline::createRenderPass() {
		vk::AttachmentDescription colorAttachment = {
			.format = targetTexture->imageCreateInfo.format,
			//TODO: Use configurable sample count
			.samples = vk::SampleCountFlagBits::e8,
			.loadOp = vk::AttachmentLoadOp::eClear,
			.storeOp = vk::AttachmentStoreOp::eStore,
			.stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
			.stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
			.initialLayout = vk::ImageLayout::eUndefined,
			.finalLayout = vk::ImageLayout::eColorAttachmentOptimal,
		};

		vk::AttachmentReference colorAttachmentRef = {
			.attachment = 0,
			.layout = vk::ImageLayout::eColorAttachmentOptimal
		};

		vk::AttachmentDescription depthAttachment = {
			.format = deviceContext.DepthFormat,
			//TODO: Use configurable sample count
			.samples = vk::SampleCountFlagBits::e8,
			.loadOp = vk::AttachmentLoadOp::eClear,
			.storeOp = vk::AttachmentStoreOp::eStore,
			.stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
			.stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
			.initialLayout = vk::ImageLayout::eUndefined,
			.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal,
		};

		vk::AttachmentReference depthAttachmentRef = {
			.attachment = 1,
			.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal
		};

		vk::AttachmentDescription resolvedColorAttachment = {
			.format = targetTexture->imageViewCreateInfo.format,
			.samples = vk::SampleCountFlagBits::e1,
			.loadOp = vk::AttachmentLoadOp::eClear,
			.storeOp = vk::AttachmentStoreOp::eStore,
			.stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
			.stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
			.initialLayout = vk::ImageLayout::eUndefined,
			.finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
		};

		vk::AttachmentReference resolvedColorAttachmentRef = {
			.attachment = 2,
			.layout = vk::ImageLayout::eColorAttachmentOptimal
		};

		vk::SubpassDescription subpass = {
			.pipelineBindPoint = vk::PipelineBindPoint::eGraphics,
			.colorAttachmentCount = 1,
			.pColorAttachments = &colorAttachmentRef,
			.pResolveAttachments = &resolvedColorAttachmentRef,
			.pDepthStencilAttachment = &depthAttachmentRef,
		};

		vk::SubpassDependency dependency = {
			.srcSubpass = VK_SUBPASS_EXTERNAL,
			.dstSubpass = 0,
			.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput |
							vk::PipelineStageFlagBits::eEarlyFragmentTests,
			.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput |
							vk::PipelineStageFlagBits::eEarlyFragmentTests,
			.srcAccessMask = vk::AccessFlagBits::eNone,
			.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite |
							 vk::AccessFlagBits::eDepthStencilAttachmentWrite
		};

		std::vector<vk::AttachmentDescription> attachments{
			colorAttachment,
			depthAttachment,
			resolvedColorAttachment
		};
		vk::RenderPassCreateInfo renderPassCreationInfo = {
			.attachmentCount = static_cast<uint32_t>(attachments.size()),
			.pAttachments = attachments.data(),
			.subpassCount = 1,
			.pSubpasses = &subpass,
			.dependencyCount = 1,
			.pDependencies = &dependency,
		};

		renderPass = deviceContext.device.createRenderPass(renderPassCreationInfo);
	}

	void MeshPipeline::createFramebufferResources() {
		vk::ImageCreateInfo imageCreateInfo{
			.imageType = vk::ImageType::e2D,
			.format = targetTexture->imageViewCreateInfo.format,
			.extent = {targetTexture->imageCreateInfo.extent.width, targetTexture->imageCreateInfo.extent.height, 1},
			.mipLevels = 1,
			.arrayLayers = 1,
			.samples = deviceContext.MaxSampleCount,
			.usage = vk::ImageUsageFlagBits::eColorAttachment,
			.sharingMode = vk::SharingMode::eExclusive,
		};
		auto mainFramebufferImage = deviceContext.createImage(
			imageCreateInfo,
			vk::MemoryPropertyFlagBits::eDeviceLocal
		);

		vk::ImageSubresourceRange subresourceRange = {
			.aspectMask = vk::ImageAspectFlagBits::eColor,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		};

		vk::ImageViewCreateInfo imageViewCreateInfo = {
			.image = mainFramebufferImage.image,
			.viewType = vk::ImageViewType::e2D,
			.format = targetTexture->imageViewCreateInfo.format,
			.subresourceRange = subresourceRange
		};

		auto mainFramebufferImageView = deviceContext.device.createImageView(imageViewCreateInfo);

		vk::ImageCreateInfo depthImageCreateInfo{
			.imageType = vk::ImageType::e2D,
			.format = deviceContext.DepthFormat,
			.extent = {targetTexture->imageCreateInfo.extent.width, targetTexture->imageCreateInfo.extent.height, 1},
			.mipLevels = 1,
			.arrayLayers = 1,
			.samples = deviceContext.MaxSampleCount,
			.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment,
			.sharingMode = vk::SharingMode::eExclusive,
		};
		auto mainFramebufferDepthImage = deviceContext.createImage(
			depthImageCreateInfo,
			vk::MemoryPropertyFlagBits::eDeviceLocal
		);

		vk::ImageSubresourceRange depthSubresourceRange = {
			.aspectMask = vk::ImageAspectFlagBits::eDepth,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		};

		vk::ImageViewCreateInfo depthImageViewCreateInfo = {
			.image = mainFramebufferDepthImage.image,
			.viewType = vk::ImageViewType::e2D,
			.format = deviceContext.DepthFormat,
			.subresourceRange = depthSubresourceRange
		};

		auto mainFramebufferDepthImageView = deviceContext.device.createImageView(depthImageViewCreateInfo);
		colorTexture = {
			.image = mainFramebufferImage,
			.imageView = mainFramebufferImageView
		};
		depthTexture = {
			.image = mainFramebufferDepthImage,
			.imageView = mainFramebufferDepthImageView
		};

	}

	void MeshPipeline::createFramebuffer() {
		std::array<vk::ImageView, 3> attachments{
			colorTexture.imageView,
			depthTexture.imageView,
			targetTexture->imageView
		};
		vk::FramebufferCreateInfo framebufferCreateInfo = {
			.renderPass = renderPass,
			.attachmentCount = (uint32_t) attachments.size(),
			.pAttachments = attachments.data(),
			.width = targetTexture->imageCreateInfo.extent.width,
			.height = targetTexture->imageCreateInfo.extent.height,
			.layers = 1
		};
		framebuffer = deviceContext.device.createFramebuffer(framebufferCreateInfo);
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

	void MeshPipeline::render(const vk::CommandBuffer& commandBuffer) const {
		auto& frameState = engineState.getCurrentFrameState();

		const auto drawContext = BuildMainRenderPass();

		if (!drawContext.drawSets.empty()) {

			const auto& drawDescriptorSet = frameState.getUniformStore<Models::Draw>().descriptorSet;
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

			vk::ClearValue colorClearValue = {
				.color = {std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}}
			};
			vk::ClearValue depthClearValue{
				.depthStencil = {1.0f, 0}
			};
			std::array<vk::ClearValue, 3> clearValues{colorClearValue, depthClearValue, colorClearValue};

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
				.clearValueCount = (uint32_t) clearValues.size(),
				.pClearValues = clearValues.data(),
			};
			commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
			commandBuffer.beginRenderPass(mainRenderPassBeginInfo, vk::SubpassContents::eInline);
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

	std::vector<Draws::Draw<Models::Draw>> MeshPipeline::getDraws() const {
		auto objectEntities = registry.view<Stores::StoreItem<Objects::Models::Object>, Meshes::MeshGroup, Spatials::Spatial>();
		std::vector<Draws::Draw<Models::Draw>> draws;
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
				for (const auto& meshEntity : meshGroup.meshEntities) {
					Meshes::MeshInfo* meshInfo = registry.get<Meshes::MeshInfo*>(meshEntity);
					const Meshes::Mesh mesh = registry.get<Meshes::Mesh>(meshEntity);
					const Stores::StoreItem<Meshes::Models::Mesh> meshStoreItem = registry.get<Stores::StoreItem<Meshes::Models::Mesh>>(
						meshEntity
					);
					const auto& meshStoreItemLocation = meshStoreItem.frameStoreItems[engineState.getFrameIndex()];
					Draws::Draw<Models::Draw> draw = {
						.drawModel = {.meshStoreItem = meshStoreItemLocation, .objectLocation = objectStoreItemLocation},
						.pipeline = pipeline,
						.indexBufferView = mesh.IndexBufferView,
						.vertexBufferView = mesh.VertexBufferView,
						.hasTransparency = meshInfo->pMaterial->hasTransparency,
						.depth = glm::distance(camera.absolutePosition, spatial.absolutePosition)
					};
					draws.push_back(draw);
				}
			}
		);
		return draws;
	}

	void MeshPipeline::recreatePipeLine() {
		if ((VkPipeline) pipeline != VK_NULL_HANDLE) {
			deviceContext.device.destroyPipeline(pipeline);
			deviceContext.device.destroyRenderPass(renderPass);
			destroyFramebuffer();
			destroyFramebufferResources();
		}

		createRenderPass();
		createFramebufferResources();
		createFramebuffer();
		createPipeline();
	}
	void MeshPipeline::setTarget(const Devices::Texture& texture) {
		targetTexture = texture;
		deviceContext.device.waitIdle();
		recreatePipeLine();
		deviceContext.device.waitIdle();
	}
}