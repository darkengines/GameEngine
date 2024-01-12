#include "DebugRenderer.hpp"
#include "../../Graphics/Graphics.hpp"

namespace drk::Debugging::Renderers {
	DebugRenderer::DebugRenderer(
		const Devices::DeviceContext& deviceContext,
		entt::registry& registry,
		std::unique_ptr<BoundingVolumes::Pipelines::BoundingVolumePipeline> boundingVolumePipeline
	) : deviceContext(deviceContext), registry(registry),
		boundingVolumePipeline(std::move(boundingVolumePipeline)),
		pipelines{
			{ std::type_index(typeid(BoundingVolumes::Pipelines::BoundingVolumePipeline)), this->boundingVolumePipeline.get() }
		}
	{}

	DebugRenderer::~DebugRenderer() {
		destroyFramebuffers();
		destroyRenderPass();
		destroyFramebufferResources();
	}
	Pipelines::GraphicsPipeline* DebugRenderer::getPipeline(std::type_index pipelineTypeIndex) {
		auto keyValuePair = pipelines.find(pipelineTypeIndex);
		if (keyValuePair == pipelines.end()) throw std::runtime_error(fmt::format("Unsupported pipeline type index {0}.", pipelineTypeIndex.name()));
		return keyValuePair->second;
	}
	void DebugRenderer::destroyFramebuffers() {
		for (const auto& framebuffer : framebuffers) {
			deviceContext.device.destroyFramebuffer(framebuffer);
		}
		framebuffers.clear();
	}

	void DebugRenderer::destroyFramebufferResources() {
		if (depthTexture.has_value()) deviceContext.destroyTexture(depthTexture.value());
		if (colorTexture.has_value()) deviceContext.destroyTexture(colorTexture.value());
	}
	void DebugRenderer::createFramebufferResources() {
		vk::ImageCreateInfo imageCreateInfo{
			.imageType = vk::ImageType::e2D,
			.format = targetImageInfo->format,
			.extent = {targetImageInfo->extent.width, targetImageInfo->extent.height, 1},
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
			.format = targetImageInfo->format,
			.subresourceRange = subresourceRange
		};

		auto mainFramebufferImageView = deviceContext.device.createImageView(imageViewCreateInfo);

		vk::ImageCreateInfo depthImageCreateInfo{
			.imageType = vk::ImageType::e2D,
			.format = deviceContext.DepthFormat,
			.extent = {targetImageInfo->extent.width, targetImageInfo->extent.height, 1},
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

	void DebugRenderer::createFramebuffers() {
		for (const auto& swapChainImageView : targetImageViews) {
			std::array<vk::ImageView, 3> attachments{
				colorTexture->imageView,
				depthTexture->imageView,
				swapChainImageView
			};
			vk::FramebufferCreateInfo framebufferCreateInfo = {
				.renderPass = renderPass,
				.attachmentCount = (uint32_t)attachments.size(),
				.pAttachments = attachments.data(),
				.width = targetImageInfo->extent.width,
				.height = targetImageInfo->extent.height,
				.layers = 1
			};
			auto framebuffer = deviceContext.device.createFramebuffer(framebufferCreateInfo);
			framebuffers.push_back(framebuffer);
		}
	}

	void DebugRenderer::createRenderPass() {
		vk::AttachmentDescription colorAttachment = {
			.format = targetImageInfo->format,
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
			.format = targetImageInfo->format,
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
	void DebugRenderer::setTargetImageViews(
		Devices::ImageInfo targetImageInfo,
		std::vector<vk::ImageView> targetImageViews
	) {
		this->targetImageInfo = targetImageInfo;
		this->targetImageViews = targetImageViews;
		for (const auto& pipeline : pipelines) pipeline.second->destroyPipeline();
		destroyFramebuffers();
		destroyFramebufferResources();
		destroyRenderPass();
		createRenderPass();

		vk::Viewport viewport;
		vk::Rect2D scissor;

		const auto& pipelineViewportStateCreateInfo = Graphics::Graphics::DefaultPipelineViewportStateCreateInfo(
			{ targetImageInfo.extent.width, targetImageInfo.extent.height },
			viewport,
			scissor
		);
		for (const auto& pipeline : pipelines) {
			pipeline.second->configure(
				[&](vk::GraphicsPipelineCreateInfo& graphicsPipelineCreateInfo) {
					graphicsPipelineCreateInfo.renderPass = renderPass;
					graphicsPipelineCreateInfo.pViewportState = &pipelineViewportStateCreateInfo;
				}
			);
		}

		createFramebufferResources();
		createFramebuffers();
	}
	void DebugRenderer::render(uint32_t targetImageIndex, const vk::CommandBuffer& commandBuffer) {

		vk::ClearValue colorClearValue = {
			.color = {std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}}
		};
		vk::ClearValue depthClearValue{
			.depthStencil = {1.0f, 0}
		};
		std::array<vk::ClearValue, 3> clearValues{ colorClearValue, depthClearValue, colorClearValue };
		const auto& extent = targetImageInfo.value().extent;
		vk::RenderPassBeginInfo mainRenderPassBeginInfo = {
			.renderPass = renderPass,
			.framebuffer = framebuffers[targetImageIndex],
			.renderArea = {
				0,
				0,
				{extent.width, extent.height}
			},
			.clearValueCount = clearValues.size(),
			.pClearValues = clearValues.data(),
		};
		commandBuffer.beginRenderPass(mainRenderPassBeginInfo, vk::SubpassContents::eInline);

		const auto& draws = registry.view<Components::DebugDraw>();
		const Components::DebugDraw* previousDebugDraw = nullptr;
		entt::entity previousDrawEntity = entt::null;

		uint32_t instanceCount = 0u;

		std::map<std::type_index, int> pipelineDrawIndices;
		for (const auto& pipeline : pipelines) {
			pipelineDrawIndices[pipeline.first] = 0;
		}
		bool isFirst = true;
		Pipelines::GraphicsPipeline const* pCurrentPipeline;

		draws.each(
			[&](entt::entity drawEntity, const Components::DebugDraw& debugDraw) {
				drk::Renderers::RenderOperation operations = drk::Renderers::RenderOperation::None;
				if (isFirst ||
					previousDebugDraw->pipelineTypeIndex != debugDraw.pipelineTypeIndex) {
					isFirst = false;
					operations |= drk::Renderers::RenderOperation::BindPipeline;
				}
				if (previousDrawEntity == entt::null ||
					(previousDebugDraw->indexBufferView.buffer.buffer != debugDraw.indexBufferView.buffer.buffer)) {
					operations |= drk::Renderers::RenderOperation::BindIndexBuffer | drk::Renderers::RenderOperation::BindVertexBuffer;
				}
				if (previousDrawEntity != entt::null &&
					previousDebugDraw->indexBufferView.byteOffset != debugDraw.indexBufferView.byteOffset) {
					operations |= drk::Renderers::RenderOperation::Draw;
				}
				if (previousDrawEntity == entt::null) {
					doOperations(commandBuffer, operations, debugDraw, &pCurrentPipeline);
				}
				else {
					if (operations != drk::Renderers::RenderOperation::None) {
						draw(
							previousDrawEntity,
							*previousDebugDraw,
							commandBuffer,
							instanceCount,
							pipelineDrawIndices[previousDebugDraw->pipelineTypeIndex],
							pCurrentPipeline
						);
						pipelineDrawIndices[previousDebugDraw->pipelineTypeIndex] += instanceCount;
						instanceCount = 0u;
					}
					doOperations(
						commandBuffer,
						operations,
						debugDraw,
						&pCurrentPipeline
					);
				}
				previousDebugDraw = &debugDraw;
				previousDrawEntity = drawEntity;
				instanceCount++;
			}
		);
		if (previousDrawEntity != entt::null) {
			this->draw(
				previousDrawEntity,
				*previousDebugDraw,
				commandBuffer,
				instanceCount,
				pipelineDrawIndices[previousDebugDraw->pipelineTypeIndex],
				pCurrentPipeline
			);
		}
		commandBuffer.endRenderPass();
	}
	void DebugRenderer::draw(
		entt::entity previousDrawEntity,
		const Components::DebugDraw& previousDebugDraw,
		const vk::CommandBuffer& commandBuffer,
		int instanceCount,
		int firstInstance,
		Pipelines::GraphicsPipeline const* pPipeline
	) {
		auto bufferInfo = pPipeline->getBufferInfo(registry, previousDrawEntity);
		commandBuffer.drawIndexed(
			bufferInfo.indexCount,
			instanceCount,
			bufferInfo.firstIndex,
			bufferInfo.vertexOffset,
			firstInstance
		);
	}
	void DebugRenderer::doOperations(
		const vk::CommandBuffer& commandBuffer,
		drk::Renderers::RenderOperation sceneRenderOperation,
		const Components::DebugDraw& debugDraw,
		Pipelines::GraphicsPipeline const** ppPipeline
	) {
		if ((sceneRenderOperation & drk::Renderers::RenderOperation::BindPipeline) == drk::Renderers::RenderOperation::BindPipeline) {
			const auto& pipeline = getPipeline(debugDraw.pipelineTypeIndex);
			*ppPipeline = pipeline;
			pipeline->bind(commandBuffer);
		}
		if ((sceneRenderOperation & drk::Renderers::RenderOperation::BindIndexBuffer) == drk::Renderers::RenderOperation::BindIndexBuffer) {
			commandBuffer.bindIndexBuffer(
				debugDraw.indexBufferView.buffer.buffer,
				0,
				vk::IndexType::eUint32
			);
		}
		if ((sceneRenderOperation & drk::Renderers::RenderOperation::BindVertexBuffer) == drk::Renderers::RenderOperation::BindVertexBuffer) {
			vk::DeviceSize offset = 0u;
			commandBuffer.bindVertexBuffers(
				0,
				1,
				&debugDraw.vertexBufferView.buffer.buffer,
				&offset
			);
		}
	}
	void DebugRenderer::destroyRenderPass() {
		deviceContext.device.destroyRenderPass(renderPass);
	}
	Devices::Texture
		DebugRenderer::BuildSceneRenderTargetTexture(const Devices::DeviceContext& deviceContext, vk::Extent3D extent) {
		vk::ImageCreateInfo imageCreateInfo{
			.imageType = vk::ImageType::e2D,
			.format = vk::Format::eR8G8B8A8Srgb,
			.extent = extent,
			.mipLevels = 1,
			.arrayLayers = 1,
			.samples = vk::SampleCountFlagBits::e1,
			.usage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
			.sharingMode = vk::SharingMode::eExclusive,
		};
		auto memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;
		auto mainFramebufferImage = deviceContext.createImage(
			imageCreateInfo,
			memoryProperties
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
			.format = vk::Format::eR8G8B8A8Srgb,
			.subresourceRange = subresourceRange
		};

		auto mainFramebufferImageView = deviceContext.device.createImageView(imageViewCreateInfo);

		Devices::Texture target = {
			.image = mainFramebufferImage,
			.imageView = mainFramebufferImageView,
			.imageCreateInfo = imageCreateInfo,
			.imageViewCreateInfo = imageViewCreateInfo,
			.memoryProperties = memoryProperties
		};

		return target;
	}
	void DebugRenderer::setTargetExtent(vk::Extent3D extent) {
		for (const auto& pipeline : pipelines) {
			pipeline.second->destroyPipeline();
		}

		vk::Viewport viewport;
		vk::Rect2D scissor;

		const auto& pipelineViewportStateCreateInfo = Graphics::Graphics::DefaultPipelineViewportStateCreateInfo(
			{ extent.width, extent.height },
			viewport,
			scissor
		);
		for (const auto& pipeline : pipelines) {
			pipeline.second->configure(
				[&](vk::GraphicsPipelineCreateInfo& graphicsPipelineCreateInfo) {
					graphicsPipelineCreateInfo.renderPass = renderPass;
					graphicsPipelineCreateInfo.pViewportState = &pipelineViewportStateCreateInfo;
				}
			);
		}
	}
}