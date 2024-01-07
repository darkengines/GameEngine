
#include "SceneRenderer.hpp"
#include "../Draws/SceneDraw.hpp"
#include "../../Graphics/Graphics.hpp"

namespace drk::Scenes::Renderers {
	SceneRenderer::SceneRenderer(
		const Devices::DeviceContext& deviceContext,
		entt::registry& registry,
		std::unique_ptr<Meshes::Pipelines::MeshPipeline> meshPipeline,
		std::unique_ptr<Points::Pipelines::PointPrimitivePipeline> pointPrimitivePipeline,
		std::unique_ptr<Lines::Pipelines::LinePipeline> linePipeline,
		std::unique_ptr<BoundingVolumes::Pipelines::BoundingVolumePipeline> boundingVolumePipeline,
		std::unique_ptr<Frustums::Pipelines::FrustumPipeline> frustumPipeline,
		std::unique_ptr<ShadowSceneRenderer> shadowSceneRenderer,
		Lights::Systems::ShadowMappingSystem& shadowMappingSystem
	)
		: deviceContext(deviceContext), registry(registry),
		meshPipeline(std::move(meshPipeline)),
		pointPrimitivePipeline(std::move(pointPrimitivePipeline)),
		linePipeline(std::move(linePipeline)),
		boundingVolumePipeline(std::move(boundingVolumePipeline)),
		frustumPipeline(std::move(frustumPipeline)),
		shadowSceneRenderer(std::move(shadowSceneRenderer)),
		shadowMappingSystem(shadowMappingSystem),
		pipelines{
			{ std::type_index(typeid(Meshes::Pipelines::MeshPipeline)), this->meshPipeline.get() },
			{ std::type_index(typeid(Points::Pipelines::PointPrimitivePipeline)), this->pointPrimitivePipeline.get() },
			{ std::type_index(typeid(Lines::Pipelines::LinePipeline)), this->linePipeline.get() },
			{ std::type_index(typeid(BoundingVolumes::Pipelines::BoundingVolumePipeline)), this->boundingVolumePipeline.get() },
			{ std::type_index(typeid(Frustums::Pipelines::FrustumPipeline)), this->frustumPipeline.get() }
		}
	{
		this->shadowSceneRenderer->setTargetImageViews({
			.extent = shadowMappingSystem.shadowMappingTexture->imageCreateInfo.extent,
			.format = shadowMappingSystem.shadowMappingTexture->imageCreateInfo.format },
			{ shadowMappingSystem.shadowMappingTexture->imageView }
		);
	}

	SceneRenderer::~SceneRenderer() {
		destroyFramebuffers();
		destroyRenderPass();
		destroyFramebufferResources();
	}
	Pipelines::Pipeline* SceneRenderer::getPipeline(std::type_index pipelineTypeIndex) {
		auto keyValuePair = pipelines.find(pipelineTypeIndex);
		if (keyValuePair == pipelines.end()) throw std::runtime_error(fmt::format("Unsupported pipeline type index {0}.", pipelineTypeIndex.name()));
		return keyValuePair->second;
	}
	void SceneRenderer::destroyFramebuffers() {
		for (const auto& framebuffer : framebuffers) {
			deviceContext.device.destroyFramebuffer(framebuffer);
		}
		framebuffers.clear();
	}

	void SceneRenderer::destroyFramebufferResources() {
		if (depthTexture.has_value()) deviceContext.destroyTexture(depthTexture.value());
		if (colorTexture.has_value()) deviceContext.destroyTexture(colorTexture.value());
	}
	void SceneRenderer::createFramebufferResources() {
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

	void SceneRenderer::createFramebuffers() {
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

	void SceneRenderer::createRenderPass() {
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
	void SceneRenderer::setTargetImageViews(
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
	void SceneRenderer::render(uint32_t targetImageIndex, const vk::CommandBuffer& commandBuffer) {

		shadowSceneRenderer->render(0, commandBuffer);

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

		const auto& draws = registry.view<Draws::SceneDraw>();
		const Draws::SceneDraw* previousSceneDraw = nullptr;
		entt::entity previousDrawEntity = entt::null;

		uint32_t instanceCount = 0u;

		std::map<std::type_index, int> pipelineDrawIndices;
		for (const auto& pipeline : pipelines) {
			pipelineDrawIndices[pipeline.first] = 0;
		}
		bool isFirst = true;
		Pipelines::Pipeline const* pCurrentPipeline;

		draws.each(
			[&](entt::entity drawEntity, const Draws::SceneDraw& sceneDraw) {
				auto operations = drk::Renderers::RenderOperation::None;
				if (isFirst ||
					previousSceneDraw->pipelineTypeIndex != sceneDraw.pipelineTypeIndex) {
					isFirst = false;
					operations |= drk::Renderers::RenderOperation::BindPipeline;
				}
				if (previousDrawEntity == entt::null ||
					(previousSceneDraw->indexBufferView.buffer.buffer != sceneDraw.indexBufferView.buffer.buffer)) {
					operations |= drk::Renderers::RenderOperation::BindIndexBuffer | drk::Renderers::RenderOperation::BindVertexBuffer;
				}
				if (previousDrawEntity != entt::null &&
					previousSceneDraw->indexBufferView.byteOffset != sceneDraw.indexBufferView.byteOffset) {
					operations |= drk::Renderers::RenderOperation::Draw;
				}
				if (previousDrawEntity == entt::null) {
					doOperations(commandBuffer, operations, sceneDraw, &pCurrentPipeline);
				}
				else {
					if (operations != drk::Renderers::RenderOperation::None) {
						draw(
							previousDrawEntity,
							*previousSceneDraw,
							commandBuffer,
							instanceCount,
							pipelineDrawIndices[previousSceneDraw->pipelineTypeIndex],
							pCurrentPipeline
						);
						pipelineDrawIndices[previousSceneDraw->pipelineTypeIndex] += instanceCount;
						instanceCount = 0u;
					}
					doOperations(
						commandBuffer,
						operations,
						sceneDraw,
						&pCurrentPipeline
					);
				}
				previousSceneDraw = &sceneDraw;
				previousDrawEntity = drawEntity;
				instanceCount++;
			}
		);
		if (previousDrawEntity != entt::null) {
			this->draw(
				previousDrawEntity,
				*previousSceneDraw,
				commandBuffer,
				instanceCount,
				pipelineDrawIndices[previousSceneDraw->pipelineTypeIndex],
				pCurrentPipeline
			);
		}
		commandBuffer.endRenderPass();
	}
	void SceneRenderer::draw(
		entt::entity previousDrawEntity,
		const Draws::SceneDraw& previousSceneDraw,
		const vk::CommandBuffer& commandBuffer,
		int instanceCount,
		int firstInstance,
		Pipelines::Pipeline const* pPipeline
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
	void SceneRenderer::doOperations(
		const vk::CommandBuffer& commandBuffer,
		drk::Renderers::RenderOperation sceneRenderOperation,
		const Draws::SceneDraw& sceneDraw,
		Pipelines::Pipeline const** ppPipeline
	) {
		if ((sceneRenderOperation & drk::Renderers::RenderOperation::BindPipeline) == drk::Renderers::RenderOperation::BindPipeline) {
			const auto& pipeline = getPipeline(sceneDraw.pipelineTypeIndex);
			*ppPipeline = pipeline;
			pipeline->bind(commandBuffer);
		}
		if ((sceneRenderOperation & drk::Renderers::RenderOperation::BindIndexBuffer) == drk::Renderers::RenderOperation::BindIndexBuffer) {
			commandBuffer.bindIndexBuffer(
				sceneDraw.indexBufferView.buffer.buffer,
				0,
				vk::IndexType::eUint32
			);
		}
		if ((sceneRenderOperation & drk::Renderers::RenderOperation::BindVertexBuffer) == drk::Renderers::RenderOperation::BindVertexBuffer) {
			vk::DeviceSize offset = 0u;
			commandBuffer.bindVertexBuffers(
				0,
				1,
				&sceneDraw.vertexBufferView.buffer.buffer,
				&offset
			);
		}
	}
	void SceneRenderer::destroyRenderPass() {
		deviceContext.device.destroyRenderPass(renderPass);
	}
	Devices::Texture
		SceneRenderer::BuildSceneRenderTargetTexture(const Devices::DeviceContext& deviceContext, vk::Extent3D extent) {
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
	void SceneRenderer::setTargetExtent(vk::Extent3D extent) {
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