
#include "SceneRenderer.hpp"
#include "../Draws/SceneDraw.hpp"
#include "../../Graphics/Graphics.hpp"

namespace drk::Scenes::Renderers {
	SceneRenderer::SceneRenderer(
		const Devices::DeviceContext& deviceContext,
		entt::registry& registry,
		std::unique_ptr<Meshes::Pipelines::MeshPipeline> meshPipeline
	)
		: deviceContext(deviceContext), registry(registry),
		  meshPipeline(std::move(meshPipeline)) {}

	SceneRenderer::~SceneRenderer() {
		destroyFramebuffers();
		destroyRenderPass();
		destroyFramebufferResources();
	}
	void SceneRenderer::destroyFramebuffers() {
		for (const auto& framebuffer: framebuffers) {
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
		for (const auto& swapChainImageView: targetImageViews) {
			std::array<vk::ImageView, 3> attachments{
				colorTexture->imageView,
				depthTexture->imageView,
				swapChainImageView
			};
			vk::FramebufferCreateInfo framebufferCreateInfo = {
				.renderPass = renderPass,
				.attachmentCount = (uint32_t) attachments.size(),
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
		meshPipeline->destroyPipeline();
		destroyFramebuffers();
		destroyFramebufferResources();
		destroyRenderPass();
		createRenderPass();

		vk::Viewport viewport;
		vk::Rect2D scissor;

		const auto& pipelineViewportStateCreateInfo = Graphics::Graphics::DefaultPipelineViewportStateCreateInfo(
			{targetImageInfo.extent.width, targetImageInfo.extent.height},
			viewport,
			scissor
		);
		meshPipeline->configure(
			[&](vk::GraphicsPipelineCreateInfo& graphicsPipelineCreateInfo) {
				graphicsPipelineCreateInfo.renderPass = renderPass;
				graphicsPipelineCreateInfo.pViewportState = &pipelineViewportStateCreateInfo;
			}
		);

		createFramebufferResources();
		createFramebuffers();
	}
	void SceneRenderer::render(uint32_t targetImageIndex, const vk::CommandBuffer& commandBuffer) {
		vk::ClearValue colorClearValue = {
			.color = {std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}}
		};
		vk::ClearValue depthClearValue{
			.depthStencil = {1.0f, 0}
		};
		std::array<vk::ClearValue, 3> clearValues{colorClearValue, depthClearValue, colorClearValue};
		auto extent = targetImageInfo.value().extent;
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
		drk::Draws::DrawSystem* previousDrawSystem = nullptr;
		const Draws::SceneDraw* previousSceneDraw = nullptr;
		entt::entity previousDrawEntity = entt::null;

		uint32_t instanceCount = 0u;
		uint32_t firstInstance = 0u;
		uint32_t indexCount = 0u;
		uint32_t firstIndex = 0u;
		uint32_t vertexOffset = 0u;

		draws.each(
			[&](entt::entity drawEntity, const Draws::SceneDraw& draw) {
				if (previousDrawSystem != draw.drawSystem) {
					//todo: fetch previousDrawSystem suitable pipeline
					const auto& pipeline = meshPipeline;
					pipeline->bind(commandBuffer);
				}

				if (previousSceneDraw != nullptr && previousSceneDraw->indexBufferView != draw.indexBufferView) {
					const auto& bufferInfo = previousDrawSystem->GetVertexBufferInfo(previousDrawEntity);
					commandBuffer.bindIndexBuffer(
						previousSceneDraw->indexBufferView->buffer.buffer,
						0,
						vk::IndexType::eUint32
					);
					commandBuffer.bindVertexBuffers(
						0,
						1,
						&previousSceneDraw->vertexBufferView->buffer.buffer,
						&previousSceneDraw->vertexBufferView->byteOffset
					);
					commandBuffer.drawIndexed(
						bufferInfo.indexCount,
						instanceCount,
						bufferInfo.firstIndex,
						bufferInfo.vertexOffset,
						firstInstance
					);
					instanceCount = 0u;
				}

				previousSceneDraw = &draw;
				instanceCount++;
				firstInstance++;
			}
		);

		commandBuffer.endRenderPass();
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
		meshPipeline->destroyPipeline();

		vk::Viewport viewport;
		vk::Rect2D scissor;

		const auto& pipelineViewportStateCreateInfo = Graphics::Graphics::DefaultPipelineViewportStateCreateInfo(
			{extent.width, extent.height},
			viewport,
			scissor
		);
		meshPipeline->configure(
			[&](vk::GraphicsPipelineCreateInfo& graphicsPipelineCreateInfo) {
				graphicsPipelineCreateInfo.renderPass = renderPass;
				graphicsPipelineCreateInfo.pViewportState = &pipelineViewportStateCreateInfo;
			}
		);
	}
}