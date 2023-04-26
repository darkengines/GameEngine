
#include "SceneRenderer.hpp"
#include "../Draws/SceneDraw.hpp"

namespace drk::Scenes::Renderers {
	SceneRenderer::SceneRenderer(
		const Devices::DeviceContext& deviceContext,
		Engine::EngineState& engineState,
		entt::registry& registry,
		std::unique_ptr<Meshes::Pipelines::MeshPipeline> meshPipeline
	)
		: deviceContext(deviceContext), engineState(engineState), registry(registry),
		  meshPipeline(std::move(meshPipeline)) {

	}
	SceneRenderer::~SceneRenderer() {
		deviceContext.device.destroyPipeline(pipeline);
		destroyFramebuffer();
		deviceContext.device.destroyRenderPass(renderPass);
		destroyFramebufferResources();
	}
	void SceneRenderer::destroyFramebuffer() {
		deviceContext.device.destroyFramebuffer(framebuffer);
	}

	void SceneRenderer::destroyFramebufferResources() {
		deviceContext.destroyTexture(depthTexture);
		deviceContext.destroyTexture(colorTexture);
	}
	void SceneRenderer::createFramebufferResources() {
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

	void SceneRenderer::createFramebuffer() {
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

	void SceneRenderer::createRenderPass() {
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
	void SceneRenderer::recreatePipeLine() {
		if ((VkRenderPass) renderPass != VK_NULL_HANDLE) {
			deviceContext.device.destroyPipeline(pipeline);
			deviceContext.device.destroyRenderPass(renderPass);
			destroyFramebuffer();
			destroyFramebufferResources();
		}

		createRenderPass();
		createFramebufferResources();
		createFramebuffer();
	}
	void SceneRenderer::Render() {

	}
	void SceneRenderer::render(const vk::CommandBuffer& commandBuffer) {

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

		const auto& draws = registry.view<Draws::SceneDraw>();
		drk::Draws::DrawSystem* previousDrawSystem = nullptr;
		const Draws::SceneDraw* previousSceneDraw = nullptr;
		entt::entity previousDrawEntity = entt::null;

		uint32_t instanceCount = 0u;
		uint32_t firstInstance = 0u;
		uint32_t indexCount = 0u;
		uint32_t firstIndex = 0u;
		uint32_t vertexOffset = 0u;

		draws.each([&](entt::entity drawEntity, const Draws::SceneDraw& draw) {
			if (previousDrawSystem != draw.drawSystem) {
				//todo: fetch previousDrawSystem suitable pipeline
				const auto& pipeline = meshPipeline;
				pipeline->bind(commandBuffer);
			}

			if (previousSceneDraw->indexBufferView != draw.indexBufferView) {
				if (previousSceneDraw != nullptr) {
					const auto& bufferInfo = previousDrawSystem->GetVertexBufferInfo(previousDrawEntity);
					commandBuffer.bindIndexBuffer(previousSceneDraw->indexBufferView->buffer.buffer, 0, vk::IndexType::eUint32);
					commandBuffer.bindVertexBuffers(0, 1, &previousSceneDraw->vertexBufferView->buffer.buffer, &previousSceneDraw->vertexBufferView->byteOffset);
					commandBuffer.drawIndexed(
						bufferInfo.indexCount,
						instanceCount,
						bufferInfo.firstIndex,
						bufferInfo.vertexOffset,
						firstInstance
					);
				}
				instanceCount = 0u;
			}

			previousSceneDraw = &draw;
			instanceCount++;
			firstInstance++;
		});

		commandBuffer.endRenderPass();
	}
}