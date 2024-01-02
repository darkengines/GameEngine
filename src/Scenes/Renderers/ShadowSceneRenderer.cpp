
#include "ShadowSceneRenderer.hpp"
#include "../Draws/ShadowSceneDraw.hpp"
#include "../../Graphics/Graphics.hpp"

namespace drk::Scenes::Renderers {
	ShadowSceneRenderer::ShadowSceneRenderer(
		const Devices::DeviceContext& deviceContext,
		entt::registry& registry,
		std::unique_ptr<Meshes::Pipelines::ShadowMeshPipeline> meshShadowPipeline
	)
		: deviceContext(deviceContext), registry(registry),
		meshShadowPipeline(std::move(meshShadowPipeline)) {}

	ShadowSceneRenderer::~ShadowSceneRenderer() {
		destroyFramebuffers();
		destroyRenderPass();
	}
	Pipelines::Pipeline* ShadowSceneRenderer::getPipeline(std::type_index pipelineTypeIndex) {
		if (std::type_index(typeid(Meshes::Pipelines::ShadowMeshPipeline)) == pipelineTypeIndex) return meshShadowPipeline.get();
		throw std::runtime_error(fmt::format("Unsupported pipeline type index {0}.", pipelineTypeIndex.name()));
	}
	void ShadowSceneRenderer::destroyFramebuffers() {
		for (const auto& framebuffer : framebuffers) {
			deviceContext.device.destroyFramebuffer(framebuffer);
		}
		framebuffers.clear();
	}

	void ShadowSceneRenderer::createFramebuffers() {
		for (const auto& targetImageView : targetImageViews) {
			std::array<vk::ImageView, 1> attachments{
				targetImageView
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

	void ShadowSceneRenderer::createRenderPass() {

		vk::AttachmentDescription depthAttachment = {
			.format = deviceContext.DepthFormat,
			//TODO: Use configurable sample count
			.samples = vk::SampleCountFlagBits::e1,
			.loadOp = vk::AttachmentLoadOp::eClear,
			.storeOp = vk::AttachmentStoreOp::eStore,
			.stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
			.stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
			.initialLayout = vk::ImageLayout::eUndefined,
			.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal,
		};

		vk::AttachmentReference depthAttachmentRef = {
			.attachment = 0,
			.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal
		};

		vk::SubpassDescription subpass = {
			.pipelineBindPoint = vk::PipelineBindPoint::eGraphics,
			.colorAttachmentCount = 0,
			.pColorAttachments = nullptr,
			.pResolveAttachments = nullptr,
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
			depthAttachment
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
	void ShadowSceneRenderer::setTargetImageViews(
		Devices::ImageInfo targetImageInfo,
		std::vector<vk::ImageView> targetImageViews
	) {
		this->targetImageInfo = targetImageInfo;
		this->targetImageViews = targetImageViews;
		meshShadowPipeline->destroyPipeline();
		destroyFramebuffers();
		destroyRenderPass();
		createRenderPass();

		vk::Viewport viewport;
		vk::Rect2D scissor;

		const auto& pipelineViewportStateCreateInfo = Graphics::Graphics::DefaultPipelineViewportStateCreateInfo(
			{ targetImageInfo.extent.width, targetImageInfo.extent.height },
			viewport,
			scissor
		);
		meshShadowPipeline->configure(
			[&](vk::GraphicsPipelineCreateInfo& graphicsPipelineCreateInfo) {
				graphicsPipelineCreateInfo.renderPass = renderPass;
				graphicsPipelineCreateInfo.pViewportState = &pipelineViewportStateCreateInfo;
			}
		);
		createFramebuffers();
	}
	void ShadowSceneRenderer::render(uint32_t targetImageIndex, const vk::CommandBuffer& commandBuffer) {
		vk::ClearValue depthClearValue{
			.depthStencil = {1.0f, 0}
		};
		std::array<vk::ClearValue, 1> clearValues{ depthClearValue };
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

		const auto& draws = registry.view<Draws::ShadowSceneDraw>();
		const Draws::ShadowSceneDraw* previousSceneDraw = nullptr;
		entt::entity previousDrawEntity = entt::null;

		uint32_t instanceCount = 0u;

		std::map<std::type_index, int> pipelineDrawIndices;
		pipelineDrawIndices[std::type_index(typeid(meshShadowPipeline))] = 0;
		bool isFirst = true;
		Pipelines::Pipeline const* pCurrentPipeline;

		draws.each(
			[&](entt::entity drawEntity, const Draws::ShadowSceneDraw& sceneDraw) {
				SceneRenderOperation operations = SceneRenderOperation::None;
				if (isFirst ||
					previousSceneDraw->pipelineTypeIndex != sceneDraw.pipelineTypeIndex) {
					isFirst = false;
					operations |= SceneRenderOperation::BindPipeline;
				}
				if (previousDrawEntity == entt::null ||
					(previousSceneDraw->indexBufferView.buffer.buffer != sceneDraw.indexBufferView.buffer.buffer)) {
					operations |= SceneRenderOperation::BindIndexBuffer | SceneRenderOperation::BindVertexBuffer;
				}
				if (previousDrawEntity == entt::null ||
					(previousSceneDraw->lightPerspectiveEntity != sceneDraw.lightPerspectiveEntity)) {
					operations |= SceneRenderOperation::SetScissor;
				}
				if (previousDrawEntity != entt::null &&
					previousSceneDraw->indexBufferView.byteOffset != sceneDraw.indexBufferView.byteOffset) {
					operations |= SceneRenderOperation::Draw;
				}
				if (previousDrawEntity == entt::null) {
					doOperations(commandBuffer, operations, sceneDraw, &pCurrentPipeline);
				}
				else {
					if (operations != SceneRenderOperation::None) {
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
	void ShadowSceneRenderer::draw(
		entt::entity previousDrawEntity,
		const Draws::ShadowSceneDraw& previousSceneDraw,
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
	void ShadowSceneRenderer::doOperations(
		const vk::CommandBuffer& commandBuffer,
		SceneRenderOperation sceneRenderOperation,
		const Draws::ShadowSceneDraw& sceneDraw,
		Pipelines::Pipeline const** ppPipeline
	) {
		if ((sceneRenderOperation & SceneRenderOperation::BindPipeline) == SceneRenderOperation::BindPipeline) {
			const auto& pipeline = getPipeline(sceneDraw.pipelineTypeIndex);
			*ppPipeline = pipeline;
			pipeline->bind(commandBuffer);
		}
		if ((sceneRenderOperation & SceneRenderOperation::BindIndexBuffer) == SceneRenderOperation::BindIndexBuffer) {
			commandBuffer.bindIndexBuffer(
				sceneDraw.indexBufferView.buffer.buffer,
				0,
				vk::IndexType::eUint32
			);
		}
		if ((sceneRenderOperation & SceneRenderOperation::BindVertexBuffer) == SceneRenderOperation::BindVertexBuffer) {
			vk::DeviceSize offset = 0u;
			commandBuffer.bindVertexBuffers(
				0,
				1,
				&sceneDraw.vertexBufferView.buffer.buffer,
				&offset
			);
		}
		if ((sceneRenderOperation & SceneRenderOperation::SetScissor) == SceneRenderOperation::SetScissor) {
			vk::DeviceSize offset = 0u;
			vk::Viewport viewport{
				.x = static_cast<float>(sceneDraw.scissor.offset.x),
				.y = static_cast<float>(sceneDraw.scissor.offset.y),
				.width = static_cast<float>(sceneDraw.scissor.extent.width),
				.height = static_cast<float>(sceneDraw.scissor.extent.height),
				.minDepth = 0.0f,
				.maxDepth = 1.0f
			};
			std::vector<vk::Viewport> viewports{ viewport };
			commandBuffer.setViewport(0, viewports);
			commandBuffer.setScissor(0, 1, &sceneDraw.scissor);
		}
	}
	void ShadowSceneRenderer::destroyRenderPass() {
		deviceContext.device.destroyRenderPass(renderPass);
	}
	Devices::Texture
		ShadowSceneRenderer::BuildSceneRenderTargetTexture(const Devices::DeviceContext& deviceContext, vk::Extent3D extent) {
		vk::ImageCreateInfo imageCreateInfo{
			.imageType = vk::ImageType::e2D,
			.format = deviceContext.DepthFormat,
			.extent = extent,
			.mipLevels = 1,
			.arrayLayers = 1,
			.samples = vk::SampleCountFlagBits::e1,
			.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled,
			.sharingMode = vk::SharingMode::eExclusive,
		};
		auto memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;
		auto mainFramebufferImage = deviceContext.createImage(
			imageCreateInfo,
			memoryProperties
		);

		vk::ImageSubresourceRange subresourceRange = {
			.aspectMask = vk::ImageAspectFlagBits::eDepth,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		};

		vk::ImageViewCreateInfo imageViewCreateInfo = {
			.image = mainFramebufferImage.image,
			.viewType = vk::ImageViewType::e2D,
			.format = deviceContext.DepthFormat,
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
	void ShadowSceneRenderer::setTargetExtent(vk::Extent3D extent) {
		meshShadowPipeline->destroyPipeline();

		vk::Viewport viewport;
		vk::Rect2D scissor;

		const auto& pipelineViewportStateCreateInfo = Graphics::Graphics::DefaultPipelineViewportStateCreateInfo(
			{ extent.width, extent.height },
			viewport,
			scissor
		);
		meshShadowPipeline->configure(
			[&](vk::GraphicsPipelineCreateInfo& graphicsPipelineCreateInfo) {
				graphicsPipelineCreateInfo.renderPass = renderPass;
				graphicsPipelineCreateInfo.pViewportState = &pipelineViewportStateCreateInfo;
			}
		);
	}
}