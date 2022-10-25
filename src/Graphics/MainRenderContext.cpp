#include "MainRenderContext.hpp"
#include "Graphics.hpp"

namespace drk::Graphics {
	MainRenderContext::MainRenderContext(
		const Devices::DeviceContext* deviceContext,
		drk::Graphics::EngineState* engineState,
		const vk::Extent2D extent
	): DeviceContext(deviceContext), EngineState(engineState), Extent(extent) {
		CreateSwapchain(Extent);
		CreateMainRenderPass();
		CreateMainFramebufferResources();
		CreateMainFramebuffers();
		CreateMainPipelineLayout();
		CreateShaderModules();
		CreateMainGraphicPipeline();
	}

	MainRenderContext::~MainRenderContext() {
		DeviceContext->Device.destroyPipeline(MainGraphicPipeline);

		DestroyShaderModules();
		DeviceContext->Device.destroyPipelineLayout(MainPipelineLayout);
		DestroyMainFramebuffer();
		DeviceContext->Device.destroyRenderPass(MainRenderPass);

		DestroyMainFramebufferResources();
		DestroySwapchain();
	}

	void MainRenderContext::CreateSwapchain(const vk::Extent2D& extent) {
		Swapchain = Devices::Device::createSwapchain(
			DeviceContext->Device,
			DeviceContext->PhysicalDevice,
			DeviceContext->Surface,
			extent
		);
	}

	void MainRenderContext::DestroySwapchain() {
		Devices::Device::destroySwapchain(DeviceContext->Device, Swapchain);
	}

	void MainRenderContext::DestroyShaderModules() {
		DeviceContext->Device.destroyShaderModule(MainFragmentShaderModule);
		DeviceContext->Device.destroyShaderModule(MainVertexShaderModule);
	}

	void MainRenderContext::DestroyMainFramebuffer() {
		for (const auto& framebuffer: MainFramebuffers) {
			DeviceContext->Device.destroyFramebuffer(framebuffer);
		}
		MainFramebuffers.clear();
	}

	void MainRenderContext::DestroyMainFramebufferResources() {
		DeviceContext->DestroyTexture(MainFramebufferDepthTexture);
		DeviceContext->DestroyTexture(MainFramebufferTexture);
	}

	void MainRenderContext::RecreateSwapchain(vk::Extent2D extent) {
		DeviceContext->Device.waitIdle();

		DeviceContext->Device.destroyPipeline(MainGraphicPipeline);
		DestroyMainFramebuffer();
		DeviceContext->Device.destroyRenderPass(MainRenderPass);
		DestroyMainFramebufferResources();
		DestroySwapchain();

		CreateSwapchain(extent);
		CreateMainRenderPass();
		CreateMainFramebufferResources();
		CreateMainFramebuffers();
		CreateMainGraphicPipeline();

		DeviceContext->Device.waitIdle();
	}

	void MainRenderContext::SetExtent(const vk::Extent2D& extent) {
		Extent = extent;
		ExtentChanged = true;
	}

	void MainRenderContext::CreateMainRenderPass() {
		vk::AttachmentDescription colorAttachment = {
			.format = Swapchain.imageFormat,
			//TODO: Use configurable sample count
			.samples = vk::SampleCountFlagBits::e1,
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
			.samples = vk::SampleCountFlagBits::e1,
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

	void MainRenderContext::CreateMainFramebufferResources() {
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
		auto mainFramebufferImage = DeviceContext->CreateImage(
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
			.format = Swapchain.imageFormat,
			.subresourceRange = subresourceRange
		};

		auto mainFramebufferImageView = DeviceContext->Device.createImageView(imageViewCreateInfo);

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
		auto mainFramebufferDepthImage = DeviceContext->CreateImage(
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
			.format = DeviceContext->DepthFormat,
			.subresourceRange = depthSubresourceRange
		};

		auto mainFramebufferDepthImageView = DeviceContext->Device.createImageView(depthImageViewCreateInfo);
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
		for (const auto& swapChainImageView: Swapchain.imageViews) {
			std::array<vk::ImageView, 3> attachments{
				MainFramebufferTexture.imageView,
				MainFramebufferDepthTexture.imageView,
				swapChainImageView
			};
			vk::FramebufferCreateInfo framebufferCreateInfo = {
				.renderPass = MainRenderPass,
				.attachmentCount = (uint32_t) attachments.size(),
				.pAttachments = attachments.data(),
				.width = Swapchain.extent.width,
				.height = Swapchain.extent.height,
				.layers = 1
			};
			auto framebuffer = DeviceContext->Device.createFramebuffer(framebufferCreateInfo);
			MainFramebuffers.push_back(framebuffer);
		}
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
			Swapchain.extent,
			viewport,
			scissor
		);
		const auto& pipelineRasterizationStateCreateInfo = Graphics::DefaultPipelineRasterizationStateCreateInfo();
		auto pipelineMultisampleStateCreateInfo = Graphics::DefaultPipelineMultisampleStateCreateInfo();
		//TODO: Use configurable sample count
		pipelineMultisampleStateCreateInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;
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

		auto result = DeviceContext->Device.createGraphicsPipeline(VK_NULL_HANDLE, graphicsPipelineCreateInfo);
		if ((VkResult) result.result != VK_SUCCESS) {
			throw new std::runtime_error("Failed to create main graphic pipeline.");
		}
		MainGraphicPipeline = result.value;
	}

	void MainRenderContext::CreateMainPipelineLayout() {
		vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
			.setLayoutCount = static_cast<uint32_t>(EngineState->DescriptorSetLayouts.size()),
			.pSetLayouts = EngineState->DescriptorSetLayouts.data(),
		};
		MainPipelineLayout = DeviceContext->Device.createPipelineLayout(pipelineLayoutCreateInfo);
	}

	void MainRenderContext::CreateShaderModules() {
		MainVertexShaderModule = DeviceContext->CreateShaderModule("shaders/spv/main.vert.spv");
		MainFragmentShaderModule = DeviceContext->CreateShaderModule("shaders/spv/main.frag.spv");
	}
}