#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include "MeshPipeline.hpp"
#include "Graphics.hpp"
#include "../Objects/Models/Object.hpp"
#include "../Meshes/MeshGroup.hpp"
#include "../Cameras/Components/Camera.hpp"
#include "../Meshes/Models/Mesh.hpp"

namespace drk::Graphics {
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

		const auto& pipelineVertexInputStateCreateInfo = Graphics::DefaultPipelineVertexInputStateCreateInfo(
			vertexInputBindingDescriptions,
			vertexInputAttributeDescriptions
		);
		const auto& pipelineInputAssemblyStateCreateInfo = Graphics::DefaultPipelineInputAssemblyStateCreateInfo();
		const auto& pipelineViewportStateCreateInfo = Graphics::DefaultPipelineViewportStateCreateInfo(
			{targetTexture->imageCreateInfo.extent.width, targetTexture->imageCreateInfo.extent.height},
			viewport,
			scissor
		);
		const auto& pipelineRasterizationStateCreateInfo = Graphics::DefaultPipelineRasterizationStateCreateInfo();
		auto pipelineMultisampleStateCreateInfo = Graphics::DefaultPipelineMultisampleStateCreateInfo();
		//TODO: Use configurable sample count
		pipelineMultisampleStateCreateInfo.rasterizationSamples = vk::SampleCountFlagBits::e8;
		const auto& pipelineColorBlendStateCreateInfo = Graphics::DefaultPipelineColorBlendStateCreateInfo(
			pipelineColorBlendAttachmentState
		);
		const auto& pipelineDepthStencilStateCreateInfo = Graphics::DefaultPipelineDepthStencilStateCreateInfo();

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
		mainVertexShaderModule = deviceContext.CreateShaderModule("shaders/spv/Mesh.vert.spv");
		mainFragmentShaderModule = deviceContext.CreateShaderModule("shaders/spv/Mesh.frag.spv");
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

	DrawContext MeshPipeline::BuildMainRenderPass() const {
		auto objectEntities = registry.view<Stores::StoreItem<Objects::Models::Object>, Meshes::MeshGroup, Spatials::Components::Spatial>();
		std::vector<Draw> draws;
		std::vector<Draw> transparencyDraws;
		objectEntities.each(
			[&](
				entt::entity objectEntity,
				auto& objectStoreItem,
				auto& meshGroup,
				auto& spatial
			) {
				const auto& objectStoreItemLocation = objectStoreItem.frameStoreItems[engineState.getFrameIndex()];
				for (const auto& meshEntity : meshGroup.meshEntities) {
					std::shared_ptr<Meshes::MeshInfo> meshInfo = registry.get<std::shared_ptr<Meshes::MeshInfo>>(meshEntity);
					const Meshes::Components::Mesh mesh = registry.get<Meshes::Components::Mesh>(meshEntity);
					const Stores::StoreItem<Meshes::Models::Mesh> meshStoreItem = registry.get<Stores::StoreItem<Meshes::Models::Mesh>>(
						meshEntity
					);
					const auto& meshStoreItemLocation = meshStoreItem.frameStoreItems[engineState.getFrameIndex()];
					Draw draw = {
						.meshInfo = meshInfo,
						.mesh = mesh,
						.meshStoreItem = {
							.storeIndex = meshStoreItemLocation.pStore->descriptorArrayElement,
							.itemIndex = meshStoreItemLocation.index
						},
						.objectLocation = {
							.storeIndex = objectStoreItemLocation.pStore->descriptorArrayElement,
							.itemIndex = objectStoreItemLocation.index
						},
						.spatial = spatial,
						.hasTransparency = meshInfo->pMaterial->hasTransparency
					};
					if (draw.hasTransparency) {
						transparencyDraws.push_back(draw);
					} else {
						draws.push_back(draw);
					}
				}
			}
		);

		std::sort(
			draws.begin(), draws.end(), [](const Draw& leftDraw, const Draw& rightDraw) {
				return leftDraw.meshInfo < rightDraw.meshInfo;
			}
		);

		std::stable_sort(
			draws.begin(), draws.end(), [](const Draw& leftDraw, const Draw& rightDraw) {
				return leftDraw.mesh.IndexBufferView.buffer.buffer < rightDraw.mesh.IndexBufferView.buffer.buffer;
			}
		);

		std::sort(
			transparencyDraws.begin(), transparencyDraws.end(), [](const Draw& leftDraw, const Draw& rightDraw) {
				return leftDraw.meshInfo < rightDraw.meshInfo;
			}
		);

		std::stable_sort(
			transparencyDraws.begin(), transparencyDraws.end(), [](const Draw& leftDraw, const Draw& rightDraw) {
				return leftDraw.mesh.IndexBufferView.buffer.buffer < rightDraw.mesh.IndexBufferView.buffer.buffer;
			}
		);

		auto cameraEntity = engineState.CameraEntity;
		auto camera = registry.get<Cameras::Components::Camera>(cameraEntity);

		std::stable_sort(
			transparencyDraws.begin(), transparencyDraws.end(), [&camera](const Draw& leftDraw, const Draw& rightDraw) {
				auto leftDistance = glm::distance(camera.absolutePosition, leftDraw.spatial.absolutePosition);
				auto rightDistance = glm::distance(camera.absolutePosition, rightDraw.spatial.absolutePosition);
				return leftDistance > rightDistance;
			}
		);

		DrawContext drawContext;

		PopulateDrawContext(drawContext, draws, 0u);
		PopulateDrawContext(drawContext, transparencyDraws, draws.size());

		return drawContext;
	}

	void MeshPipeline::PopulateDrawContext(
		DrawContext& drawContext,
		const std::vector<Draw>& draws,
		uint32_t drawOffset
	) const {
		auto& frameState = engineState.getCurrentFrameState();
		auto& drawStore = frameState.getUniformStore<Models::Draw>();
		const Draw* previousDraw = nullptr;
		if (!drawContext.drawSets.empty() && !drawContext.drawSets.back().draws.empty())
			previousDraw = &drawContext.drawSets.back().draws.back();

		for (auto drawIndex = 0u; drawIndex < draws.size(); drawIndex++) {
			auto& draw = draws[drawIndex];

			if (previousDraw != nullptr && previousDraw->meshInfo == draw.meshInfo) {
				drawContext.drawSets.back().drawCommands.back().instanceCount++;
			} else {
				if (previousDraw == nullptr ||
					draw.mesh.IndexBufferView.buffer.buffer != previousDraw->mesh.IndexBufferView.buffer.buffer) {
					drawContext.drawSets.push_back(
						{
							.indexBuffer = draw.mesh.IndexBufferView.buffer,
							.vertexBuffer = draw.mesh.VertexBufferView.buffer,
						}
					);
				}
				drawContext.drawSets.back().drawCommands.push_back(
					{
						.indexCount = (uint32_t) draw.meshInfo->indices.size(),
						.instanceCount = 1,
						.firstIndex = static_cast<uint32_t>(draw.mesh.IndexBufferView.byteOffset /
															sizeof(Meshes::VertexIndex)),
						.vertexOffset = static_cast<uint32_t>(draw.mesh.VertexBufferView.byteOffset /
															  sizeof(Meshes::Vertex)),
						.firstInstance = drawIndex + drawOffset
					}
				);
			}

			drawContext.drawSets.back().draws.push_back(draw);

			const auto drawItemLocation = drawStore.get(drawIndex + drawOffset);

			const Models::Draw drawModel = {
				.meshItemLocation = draw.meshStoreItem,
				.objectItemLocation = draw.objectLocation,
			};

			drawItemLocation.pItem->meshItemLocation.storeIndex = drawModel.meshItemLocation.storeIndex;
			drawItemLocation.pItem->meshItemLocation.itemIndex = drawModel.meshItemLocation.itemIndex;
			drawItemLocation.pItem->objectItemLocation.storeIndex = drawModel.objectItemLocation.storeIndex;
			drawItemLocation.pItem->objectItemLocation.itemIndex = drawModel.objectItemLocation.itemIndex;

			previousDraw = &draw;
		}
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