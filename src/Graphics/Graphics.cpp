#include "Graphics.hpp"
#include "../Windows/Window.hpp"
#include "../Common/Common.hpp"
#include "../Devices/Device.hpp"
#include "../Meshes/Vertex.hpp"
#include <imgui_impl_vulkan.h>

namespace drk::Graphics {
	std::vector<const char *> Graphics::RequiredInstanceExtensions = Windows::Window::getVulkanInstanceExtension();
	std::vector<const char *> Graphics::RequiredDeviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
		VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME
	};

	Graphics::Graphics(
		const Devices::DeviceContext *deviceContext,
		drk::Graphics::EngineState *engineState,
		const vk::Extent2D extent
	) : DeviceContext(deviceContext), EngineState(engineState) {
		Swapchain = Devices::Device::createSwapchain(
			DeviceContext->Device,
			DeviceContext->PhysicalDevice,
			DeviceContext->Surface,
			extent
		);
		CreateMainRenderPass();
		CreateMainFramebufferResources();
		CreateMainFramebuffers();
		CreateMainPipelineLayout();
		CreateShaderModules();
		CreateMainGraphicPipeline();
		SetupImgui();
	}

	Graphics::~Graphics() {
		DeviceContext->Device.destroyDescriptorPool(ImGuiDescriptorPool);
		ImGui_ImplVulkan_Shutdown();

		DeviceContext->Device.destroyPipeline(MainGraphicPipeline);

		DeviceContext->Device.destroyShaderModule(MainFragmentShaderModule);
		DeviceContext->Device.destroyShaderModule(MainVertexShaderModule);

		DeviceContext->Device.destroyPipelineLayout(MainPipelineLayout);
		for (const auto &framebuffer: MainFramebuffers) {
			DeviceContext->Device.destroyFramebuffer(framebuffer);
		}
		MainFramebuffers.clear();
		DeviceContext->Device.destroyRenderPass(MainRenderPass);
		DeviceContext->Device.destroyImageView(MainFramebufferDepthImageView);
		DeviceContext->DestroyTexture(MainFramebufferDepthTexture);
		DeviceContext->Device.destroyImageView(MainFramebufferImageView);
		DeviceContext->DestroyTexture(MainFramebufferTexture);
		Devices::Device::destroySwapchain(DeviceContext->Device, Swapchain);
	}

	vk::PipelineDepthStencilStateCreateInfo Graphics::DefaultPipelineDepthStencilStateCreateInfo() {
		vk::PipelineDepthStencilStateCreateInfo depthStencil = {
			.depthTestEnable = true,
			.depthWriteEnable = true,
			.depthCompareOp = vk::CompareOp::eLess,
			.depthBoundsTestEnable = false,
			.stencilTestEnable = true,
			.minDepthBounds = 0.0f,
			.maxDepthBounds = 1.0f,
		};
		return depthStencil;
	}

	vk::PipelineColorBlendAttachmentState Graphics::DefaultPipelineColorBlendAttachmentState() {
		vk::PipelineColorBlendAttachmentState colorBlendAttachment = {
			.blendEnable = true,
			.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha,
			.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha,
			.colorBlendOp = vk::BlendOp::eAdd,
			.srcAlphaBlendFactor = vk::BlendFactor::eOne,
			.dstAlphaBlendFactor = vk::BlendFactor::eZero,
			.alphaBlendOp = vk::BlendOp::eAdd,
			.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
							  vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
		};
		return colorBlendAttachment;
	}

	vk::PipelineColorBlendStateCreateInfo
	Graphics::DefaultPipelineColorBlendStateCreateInfo(vk::PipelineColorBlendAttachmentState &pipelineColorBlendAttachmentState) {
		pipelineColorBlendAttachmentState = DefaultPipelineColorBlendAttachmentState();
		vk::PipelineColorBlendStateCreateInfo colorBlending = {
			.logicOpEnable = false,
			.logicOp = vk::LogicOp::eCopy,
			.attachmentCount = 1,
			.pAttachments = &pipelineColorBlendAttachmentState,
			.blendConstants = std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f}
		};
		return colorBlending;
	}

	vk::PipelineMultisampleStateCreateInfo Graphics::DefaultPipelineMultisampleStateCreateInfo() {
		vk::PipelineMultisampleStateCreateInfo multisampling = {
			.rasterizationSamples = vk::SampleCountFlagBits::e1,
			.sampleShadingEnable = false,
			.minSampleShading = 1.0f,
			.alphaToCoverageEnable = false,
			.alphaToOneEnable = false,
		};
		return multisampling;
	}

	vk::PipelineRasterizationStateCreateInfo Graphics::DefaultPipelineRasterizationStateCreateInfo() {
		vk::PipelineRasterizationStateCreateInfo rasterizer = {
			.depthClampEnable = false,
			.rasterizerDiscardEnable = false,
			.polygonMode = vk::PolygonMode::eFill,
			.cullMode = vk::CullModeFlagBits::eBack,
			.frontFace = vk::FrontFace::eCounterClockwise,
			.depthBiasEnable = false,
			.depthBiasConstantFactor = 0.0f,
			.depthBiasClamp = 0.0f,
			.depthBiasSlopeFactor = 0.0f,
			.lineWidth = 1.0f,
		};

		return rasterizer;
	}

	vk::PipelineViewportStateCreateInfo Graphics::DefaultPipelineViewportStateCreateInfo(
		const vk::Extent2D &extent,
		vk::Viewport &viewport,
		vk::Rect2D &scissor
	) {
		viewport = vk::Viewport{
			.x = 0.0f,
			.y = 0.0f,
			.width = static_cast<float>(extent.width),
			.height = static_cast<float>(extent.height),
			.minDepth = 0.0f,
			.maxDepth = 1.0f,
		};

		scissor = {
			.offset = {0, 0},
			.extent = extent,
		};

		vk::PipelineViewportStateCreateInfo viewportState = {
			.viewportCount = 1,
			.pViewports = &viewport,
			.scissorCount = 1,
			.pScissors = &scissor
		};

		return viewportState;
	}

	vk::PipelineInputAssemblyStateCreateInfo Graphics::DefaultPipelineInputAssemblyStateCreateInfo() {
		vk::PipelineInputAssemblyStateCreateInfo inputAssembly = {
			.topology = vk::PrimitiveTopology::eTriangleList,
			.primitiveRestartEnable = false,
		};
		return inputAssembly;
	}

	vk::PipelineVertexInputStateCreateInfo Graphics::DefaultPipelineVertexInputStateCreateInfo(
		std::vector<vk::VertexInputBindingDescription> &vertexInputBindingDescriptions,
		std::vector<vk::VertexInputAttributeDescription> &vertexInputAttributeDescriptions
	) {
		vertexInputBindingDescriptions = Meshes::Vertex::getBindingDescription();
		vertexInputAttributeDescriptions = Meshes::Vertex::getAttributeDescriptions();

		vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {
			.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexInputBindingDescriptions.size()),
			.pVertexBindingDescriptions = vertexInputBindingDescriptions.data(),
			.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributeDescriptions.size()),
			.pVertexAttributeDescriptions = vertexInputAttributeDescriptions.data(),
		};

		return vertexInputInfo;
	}

	vk::ShaderModule Graphics::CreateShaderModule(const std::string &shaderPath) const {
		auto code = Common::ReadFile(shaderPath);
		return Devices::Device::createShaderModules(
			DeviceContext->Device,
			static_cast<uint32_t>(code.size()),
			reinterpret_cast<uint32_t *>(code.data()));
	}

	void Graphics::RecreateSwapchain(vk::Extent2D extent) {
		Devices::Device::destroySwapchain(DeviceContext->Device, Swapchain);

		Swapchain = Devices::Device::createSwapchain(
			DeviceContext->Device,
			DeviceContext->PhysicalDevice,
			DeviceContext->Surface,
			extent
		);
	}

	void Graphics::SetExtent(const vk::Extent2D &extent) {
		RecreateSwapchain(extent);
	}

	void Graphics::CreateMainRenderPass() {
		vk::AttachmentDescription colorAttachment = {
			.format = Swapchain.imageFormat,
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
			.format = DeviceContext->DepthFormat,
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
			.format = Swapchain.imageFormat,
			.samples = vk::SampleCountFlagBits::e1,
			.loadOp = vk::AttachmentLoadOp::eClear,
			.storeOp = vk::AttachmentStoreOp::eStore,
			.stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
			.stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
			.initialLayout = vk::ImageLayout::eUndefined,
			.finalLayout = vk::ImageLayout::ePresentSrcKHR,
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

		auto renderPass = DeviceContext->Device.createRenderPass(renderPassCreationInfo);
		MainRenderPass = renderPass;
	}

	void Graphics::CreateMainFramebufferResources() {
		vk::ImageCreateInfo imageCreateInfo{
			.imageType = vk::ImageType::e2D,
			.format = Swapchain.imageFormat,
			.extent = {Swapchain.extent.width, Swapchain.extent.height, 1},
			.mipLevels = 1,
			.arrayLayers = 1,
			.samples = DeviceContext->MaxSampleCount,
			.usage = vk::ImageUsageFlagBits::eColorAttachment,
			.sharingMode = vk::SharingMode::eExclusive,
		};
		MainFramebufferTexture = DeviceContext->CreateTexture(
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
			.image = MainFramebufferTexture.image,
			.viewType = vk::ImageViewType::e2D,
			.format = Swapchain.imageFormat,
			.subresourceRange = subresourceRange
		};

		MainFramebufferImageView = DeviceContext->Device.createImageView(imageViewCreateInfo);

		vk::ImageCreateInfo depthImageCreateInfo{
			.imageType = vk::ImageType::e2D,
			.format = DeviceContext->DepthFormat,
			.extent = {Swapchain.extent.width, Swapchain.extent.height, 1},
			.mipLevels = 1,
			.arrayLayers = 1,
			.samples = DeviceContext->MaxSampleCount,
			.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment,
			.sharingMode = vk::SharingMode::eExclusive,
		};
		MainFramebufferDepthTexture = DeviceContext->CreateTexture(
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
			.image = MainFramebufferDepthTexture.image,
			.viewType = vk::ImageViewType::e2D,
			.format = DeviceContext->DepthFormat,
			.subresourceRange = depthSubresourceRange
		};

		MainFramebufferDepthImageView = DeviceContext->Device.createImageView(depthImageViewCreateInfo);
	}

	void Graphics::CreateMainFramebuffers() {
		for (const auto &swapChainImageView: Swapchain.imageViews) {
			std::array<vk::ImageView, 3> attachments{
				MainFramebufferImageView,
				MainFramebufferDepthImageView,
				swapChainImageView
			};
			vk::FramebufferCreateInfo framebufferCreateInfo = {
				.renderPass = MainRenderPass,
				.attachmentCount = attachments.size(),
				.pAttachments = attachments.data(),
				.width = Swapchain.extent.width,
				.height = Swapchain.extent.height,
				.layers = 1
			};
			auto framebuffer = DeviceContext->Device.createFramebuffer(framebufferCreateInfo);
			MainFramebuffers.push_back(framebuffer);
		}
	}

	void Graphics::CreateMainGraphicPipeline() {
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

		const auto &pipelineVertexInputStateCreateInfo = DefaultPipelineVertexInputStateCreateInfo(
			vertexInputBindingDescriptions,
			vertexInputAttributeDescriptions
		);
		const auto &pipelineInputAssemblyStateCreateInfo = DefaultPipelineInputAssemblyStateCreateInfo();
		const auto &pipelineViewportStateCreateInfo = DefaultPipelineViewportStateCreateInfo(
			Swapchain.extent,
			viewport,
			scissor
		);
		const auto &pipelineRasterizationStateCreateInfo = DefaultPipelineRasterizationStateCreateInfo();
		auto pipelineMultisampleStateCreateInfo = DefaultPipelineMultisampleStateCreateInfo();
		//TODO: Use configurable sample count
		pipelineMultisampleStateCreateInfo.rasterizationSamples = vk::SampleCountFlagBits::e8;
		const auto &pipelineColorBlendStateCreateInfo = DefaultPipelineColorBlendStateCreateInfo(
			pipelineColorBlendAttachmentState
		);
		const auto &pipelineDepthStencilStateCreateInfo = DefaultPipelineDepthStencilStateCreateInfo();

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

		auto result = DeviceContext->Device.createGraphicsPipeline(VK_NULL_HANDLE, graphicsPipelineCreateInfo);
		if ((VkResult) result.result != VK_SUCCESS) {
			throw new std::exception("Failed to create main graphic pipeline.");
		}
		MainGraphicPipeline = result.value;
	}

	void Graphics::CreateMainPipelineLayout() {
		vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
			.setLayoutCount = 0,
			.pSetLayouts = nullptr,
		};
		MainPipelineLayout = DeviceContext->Device.createPipelineLayout(pipelineLayoutCreateInfo);
	}

	void Graphics::SetupImgui() {

		vk::DescriptorPoolSize poolSizes[] =
			{
				{vk::DescriptorType::eSampler,              1000},
				{vk::DescriptorType::eCombinedImageSampler, 1000},
				{vk::DescriptorType::eSampledImage,         1000},
				{vk::DescriptorType::eStorageImage,         1000},
				{vk::DescriptorType::eUniformTexelBuffer,   1000},
				{vk::DescriptorType::eStorageTexelBuffer,   1000},
				{vk::DescriptorType::eUniformBuffer,        1000},
				{vk::DescriptorType::eStorageBuffer,        1000},
				{vk::DescriptorType::eUniformBufferDynamic, 1000},
				{vk::DescriptorType::eStorageBufferDynamic, 1000},
				{vk::DescriptorType::eInputAttachment,      1000}
			};

		vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo = {
			.maxSets = 1000,
			.poolSizeCount = std::size(poolSizes),
			.pPoolSizes = poolSizes,
		};

		ImGuiDescriptorPool = DeviceContext->Device.createDescriptorPool(descriptorPoolCreateInfo);

		ImGui::CreateContext();

		ImGui_ImplVulkan_InitInfo infos{
			.Instance = DeviceContext->Instance,
			.PhysicalDevice = DeviceContext->PhysicalDevice,
			.Device = DeviceContext->Device,
			.QueueFamily = 0,
			.Queue = DeviceContext->GraphicQueue,
			.PipelineCache = VK_NULL_HANDLE,
			.DescriptorPool = ImGuiDescriptorPool,
			.Subpass =0,
			.MinImageCount=2,
			.ImageCount=2,
			//TODO: Use configurable sample count
			.MSAASamples = (VkSampleCountFlagBits) vk::SampleCountFlagBits::e8,
			.Allocator = nullptr,
			.CheckVkResultFn = nullptr
		};
		ImGui_ImplVulkan_Init(&infos, MainRenderPass);
		vk::CommandBufferAllocateInfo commandBufferAllocationInfo = {
			.commandPool = DeviceContext->CommandPool,
			.level = vk::CommandBufferLevel::ePrimary,
			.commandBufferCount = 1,
		};
		auto commandBuffer = Devices::Device::beginSingleTimeCommands(
			DeviceContext->Device,
			DeviceContext->CommandPool
		);
		ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
		Devices::Device::endSingleTimeCommands(
			DeviceContext->Device,
			DeviceContext->GraphicQueue,
			DeviceContext->CommandPool,
			commandBuffer
		);
		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}

	void Graphics::CreateShaderModules() {
		MainVertexShaderModule = CreateShaderModule("shaders/spv/main.vert.spv");
		MainFragmentShaderModule = CreateShaderModule("shaders/spv/main.frag.spv");
	}

	void Graphics::Render() {
		ImGui::Render();
		const auto &frameState = EngineState->FrameStates[EngineState->FrameIndex];
		const auto &fence = frameState.Fence;
		const auto &imageReadySemaphore = frameState.ImageReadySemaphore;
		const auto &imageRenderedSemaphore = frameState.ImageRenderedSemaphore;

		const auto &waitForFenceResult = DeviceContext->Device.waitForFences(1, &fence, VK_TRUE, UINT64_MAX);
		//Todo: Handle suboptimal and out of date results
		auto swapchainImageIndex = DeviceContext->Device.acquireNextImageKHR(
			Swapchain.swapchain,
			UINT64_MAX,
			frameState.ImageReadySemaphore,
			VK_NULL_HANDLE
		);
		const auto &resetFenceResult = DeviceContext->Device.resetFences(1, &fence);

		frameState.CommandBuffer.reset();

		vk::CommandBufferBeginInfo commandBufferBeginInfo = {};
		const auto &result = frameState.CommandBuffer.begin(&commandBufferBeginInfo);
		vk::ClearValue colorClearValue = {
			.color = {std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}}
		};
		vk::ClearValue depthClearValue{
			.depthStencil = {1.0f, 0}
		};
		std::array<vk::ClearValue, 3> clearValues{colorClearValue, depthClearValue, colorClearValue};
		vk::RenderPassBeginInfo mainRenderPassBeginInfo = {
			.renderPass = MainRenderPass,
			.framebuffer = MainFramebuffers[swapchainImageIndex.value],
			.renderArea = {0, 0, Swapchain.extent},
			.clearValueCount = clearValues.size(),
			.pClearValues = clearValues.data(),
		};
		frameState.CommandBuffer.beginRenderPass(mainRenderPassBeginInfo, vk::SubpassContents::eInline);
//		frameState.CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, MainGraphicPipeline);
//		frameState.CommandBuffer.drawIndexed(0, 0, 0, 0, 0);

		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), frameState.CommandBuffer);
		frameState.CommandBuffer.endRenderPass();
		frameState.CommandBuffer.end();

		vk::PipelineStageFlags submissionWaitDstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		vk::SubmitInfo submitInfo = {
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &imageReadySemaphore,
			.pWaitDstStageMask = &submissionWaitDstStageMask,
			.commandBufferCount = 1,
			.pCommandBuffers = &frameState.CommandBuffer,
			.signalSemaphoreCount = 1,
			.pSignalSemaphores = &imageRenderedSemaphore,
		};

		DeviceContext->GraphicQueue.submit({submitInfo}, fence);

		vk::Result presentResults;
		vk::PresentInfoKHR presentInfoKHR = {
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &imageRenderedSemaphore,
			.swapchainCount = 1,
			.pSwapchains = &Swapchain.swapchain,
			.pImageIndices = &swapchainImageIndex.value,
			.pResults = &presentResults,
		};

		//Todo: make "frame in flight" count configurable
		EngineState->FrameIndex = (EngineState->FrameIndex + 1) % 2;
		const auto &presentResult = DeviceContext->PresentQueue.presentKHR(presentInfoKHR);

		//frameState.CommandBuffer.bindIndexBuffer(...);
		//frameState.CommandBuffer.bindVertexBuffers(...);
		//frameState.CommandBuffer.bindDescriptorSets(...);

		//frameState.CommandBuffer.bindPipeline(...);
		//frameState.CommandBuffer.drawIndexed(...);
	}
}