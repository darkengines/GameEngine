#include "MainRenderContext.hpp"
#include "Graphics.hpp"
#include "../Objects/Models/Object.hpp"
#include "../Meshes/MeshGroup.hpp"
#include "../Cameras/Camera.hpp"
#include "../Meshes/Models/Mesh.hpp"

namespace drk::Graphics {
	MainRenderContext::MainRenderContext(
		const Devices::DeviceContext& deviceContext,
		const drk::Graphics::EngineState& engineState,
		Devices::Texture target
	)
		: DeviceContext(deviceContext), EngineState(engineState), TargetTexture(target) {
		CreateMainRenderPass();
		CreateMainFramebufferResources();
		CreateMainFramebuffers();
		CreateMainPipelineLayout();
		CreateShaderModules();
		CreateMainGraphicPipeline();
	}

	MainRenderContext::~MainRenderContext() {
		DeviceContext.Device.destroyPipeline(MainGraphicPipeline);

		DestroyShaderModules();
		DeviceContext.Device.destroyPipelineLayout(MainPipelineLayout);
		DestroyMainFramebuffer();
		DeviceContext.Device.destroyRenderPass(MainRenderPass);

		DestroyMainFramebufferResources();
	}

	void MainRenderContext::DestroyShaderModules() {
		DeviceContext.Device.destroyShaderModule(MainFragmentShaderModule);
		DeviceContext.Device.destroyShaderModule(MainVertexShaderModule);
	}

	void MainRenderContext::DestroyMainFramebuffer() {
		for (const auto& framebuffer: MainFramebuffers) {
			DeviceContext.Device.destroyFramebuffer(framebuffer);
		}
		MainFramebuffers.clear();
	}

	void MainRenderContext::DestroyMainFramebufferResources() {
		DeviceContext.DestroyTexture(MainFramebufferDepthTexture);
		DeviceContext.DestroyTexture(MainFramebufferTexture);
	}

	void MainRenderContext::SetExtent(const vk::Extent2D& extent) {
		ExtentChanged = true;
	}

	void MainRenderContext::CreateMainRenderPass() {
		vk::AttachmentDescription colorAttachment = {
			.format = TargetTexture.Format,
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
			.format = DeviceContext.DepthFormat,
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
			.format = TargetTexture.Format,
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

		auto renderPass = DeviceContext.Device.createRenderPass(renderPassCreationInfo);
		MainRenderPass = renderPass;
	}

	void MainRenderContext::CreateMainFramebufferResources() {
		vk::ImageCreateInfo imageCreateInfo{
			.imageType = vk::ImageType::e2D,
			.format = TargetTexture.Format,
			.extent = {TargetTexture.Extent.width, TargetTexture.Extent.height, 1},
			.mipLevels = 1,
			.arrayLayers = 1,
			.samples = DeviceContext.MaxSampleCount,
			.usage = vk::ImageUsageFlagBits::eColorAttachment,
			.sharingMode = vk::SharingMode::eExclusive,
		};
		auto mainFramebufferImage = DeviceContext.CreateImage(
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
			.format = TargetTexture.Format,
			.subresourceRange = subresourceRange
		};

		auto mainFramebufferImageView = DeviceContext.Device.createImageView(imageViewCreateInfo);

		vk::ImageCreateInfo depthImageCreateInfo{
			.imageType = vk::ImageType::e2D,
			.format = DeviceContext.DepthFormat,
			.extent = {TargetTexture.Extent.width, TargetTexture.Extent.height, 1},
			.mipLevels = 1,
			.arrayLayers = 1,
			.samples = DeviceContext.MaxSampleCount,
			.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment,
			.sharingMode = vk::SharingMode::eExclusive,
		};
		auto mainFramebufferDepthImage = DeviceContext.CreateImage(
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
			.format = DeviceContext.DepthFormat,
			.subresourceRange = depthSubresourceRange
		};

		auto mainFramebufferDepthImageView = DeviceContext.Device.createImageView(depthImageViewCreateInfo);
		MainFramebufferTexture = {
			.image = mainFramebufferImage,
			.imageView = mainFramebufferImageView
		};
		MainFramebufferDepthTexture = {
			.image = mainFramebufferDepthImage,
			.imageView = mainFramebufferDepthImageView
		};
	}

	void MainRenderContext::CreateMainFramebuffers() {
		std::array<vk::ImageView, 3> attachments{
			MainFramebufferTexture.imageView,
			MainFramebufferDepthTexture.imageView,
			TargetTexture.imageView
		};
		vk::FramebufferCreateInfo framebufferCreateInfo = {
			.renderPass = MainRenderPass,
			.attachmentCount = (uint32_t) attachments.size(),
			.pAttachments = attachments.data(),
			.width = TargetTexture.Extent.width,
			.height = TargetTexture.Extent.height,
			.layers = 1
		};
		auto framebuffer = DeviceContext.Device.createFramebuffer(framebufferCreateInfo);
		MainFramebuffers.push_back(framebuffer);
	}

	void MainRenderContext::CreateMainGraphicPipeline() {
		vk::PipelineShaderStageCreateInfo vertexPipelineShaderStageCreateInfo = {
			.stage = vk::ShaderStageFlagBits::eVertex,
			.module = MainVertexShaderModule,
			.pName = "main"
		};
		vk::PipelineShaderStageCreateInfo fragmentPipelineShaderStageCreateInfo = {
			.stage = vk::ShaderStageFlagBits::eFragment,
			.module = MainFragmentShaderModule,
			.pName = "main"
		};

		std::vector<vk::PipelineShaderStageCreateInfo> pipelineShaderStageCreateInfos = {
			vertexPipelineShaderStageCreateInfo,
			fragmentPipelineShaderStageCreateInfo
		};

		vk::PipelineColorBlendAttachmentState pipelineColorBlendAttachmentState;
		std::vector<vk::VertexInputBindingDescription> vertexInputBindingDescriptions;
		std::vector<vk::VertexInputAttributeDescription> vertexInputAttributeDescriptions;
		vk::Viewport viewport;
		vk::Rect2D scissor;

		const auto& pipelineVertexInputStateCreateInfo = Graphics::DefaultPipelineVertexInputStateCreateInfo(
			vertexInputBindingDescriptions,
			vertexInputAttributeDescriptions
		);
		const auto& pipelineInputAssemblyStateCreateInfo = Graphics::DefaultPipelineInputAssemblyStateCreateInfo();
		const auto& pipelineViewportStateCreateInfo = Graphics::DefaultPipelineViewportStateCreateInfo(
			TargetTexture.Extent,
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
			.layout = MainPipelineLayout,
			.renderPass = MainRenderPass,
		};

		auto result = DeviceContext.Device.createGraphicsPipeline(VK_NULL_HANDLE, graphicsPipelineCreateInfo);
		if ((VkResult) result.result != VK_SUCCESS) {
			throw new std::runtime_error("Failed to create main graphic pipeline.");
		}
		MainGraphicPipeline = result.value;
	}

	void MainRenderContext::CreateMainPipelineLayout() {
		vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
			.setLayoutCount = static_cast<uint32_t>(EngineState.DescriptorSetLayouts.size()),
			.pSetLayouts = EngineState.DescriptorSetLayouts.data(),
		};
		MainPipelineLayout = DeviceContext.Device.createPipelineLayout(pipelineLayoutCreateInfo);
	}

	void MainRenderContext::CreateShaderModules() {
		MainVertexShaderModule = DeviceContext.CreateShaderModule("shaders/spv/main.vert.spv");
		MainFragmentShaderModule = DeviceContext.CreateShaderModule("shaders/spv/main.frag.spv");
	}

	void MainRenderContext::Render(const vk::CommandBuffer& commandBuffer) const {
		const auto& frameState = EngineState.FrameStates[EngineState.FrameIndex];

		const auto drawContext = BuildMainRenderPass();

		vk::ClearValue colorClearValue = {
			.color = {std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}}
		};
		vk::ClearValue depthClearValue{
			.depthStencil = {1.0f, 0}
		};
		std::array<vk::ClearValue, 3> clearValues{colorClearValue, depthClearValue, colorClearValue};

		vk::RenderPassBeginInfo mainRenderPassBeginInfo = {
			.renderPass = MainRenderPass,
			.framebuffer = MainFramebuffers[0],
			.renderArea = {0, 0, TargetTexture.Extent},
			.clearValueCount = (uint32_t) clearValues.size(),
			.pClearValues = clearValues.data(),
		};
		commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, MainGraphicPipeline);
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

	DrawContext MainRenderContext::BuildMainRenderPass() const {
		auto objectEntities = EngineState.Registry.view<Stores::StoreItem<Objects::Models::Object>, Meshes::MeshGroup, Spatials::Spatial>();
		std::vector<Draw> draws;
		std::vector<Draw> transparencyDraws;
		objectEntities.each(
			[&](
				entt::entity objectEntity,
				auto& objectStoreItem,
				auto& meshGroup,
				auto& spatial
			) {
				const auto& objectStoreItemLocation = objectStoreItem.frameStoreItems[EngineState.FrameIndex];
				for (const auto& meshEntity : meshGroup.meshEntities) {
					Meshes::MeshInfo* meshInfo = EngineState.Registry.get<Meshes::MeshInfo*>(meshEntity);
					const Meshes::Mesh mesh = EngineState.Registry.get<Meshes::Mesh>(meshEntity);
					const Stores::StoreItem<Meshes::Models::Mesh> meshStoreItem = EngineState.Registry.get<Stores::StoreItem<Meshes::Models::Mesh>>(
						meshEntity
					);
					const auto& meshStoreItemLocation = meshStoreItem.frameStoreItems[EngineState.FrameIndex];
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

		auto cameraEntity = EngineState.CameraEntity;
		auto camera = EngineState.Registry.get<Cameras::Camera>(cameraEntity);

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

	void MainRenderContext::PopulateDrawContext(
		DrawContext& drawContext,
		const std::vector<Draw>& draws,
		uint32_t drawOffset
	) const {
		auto drawStore = EngineState.FrameStates[EngineState.FrameIndex].DrawStore.get();
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

			const auto drawItemLocation = drawStore->Get(drawIndex + drawOffset);

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
}