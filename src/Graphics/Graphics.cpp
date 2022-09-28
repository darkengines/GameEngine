#include "Graphics.hpp"
#include "../Windows/Window.hpp"
#include "../Common/Common.hpp"
#include "../Devices/Device.hpp"
#include "../Meshes/Vertex.hpp"

namespace drk::Graphics {
	std::vector<const char *> Graphics::RequiredInstanceExtensions = Windows::Window::getVulkanInstanceExtension();
	std::vector<const char *> Graphics::RequiredDeviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
		VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME
	};

	Graphics::Graphics(
		const Devices::DeviceContext *deviceContext,
		const vk::Extent2D extent
	) : DeviceContext(deviceContext) {
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
		CreateMainGraphicPipeline();
	}

	Graphics::~Graphics() {
		DeviceContext->Device.destroyPipeline(MainGraphicPipeline);
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

	vk::PipelineColorBlendStateCreateInfo Graphics::DefaultPipelineColorBlendStateCreateInfo(vk::PipelineColorBlendAttachmentState& pipelineColorBlendAttachmentState) {
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
		const auto &pipelineMultisampleStateCreateInfo = DefaultPipelineMultisampleStateCreateInfo();
		const auto &pipelineColorBlendStateCreateInfo = DefaultPipelineColorBlendStateCreateInfo(pipelineColorBlendAttachmentState);
		const auto &pipelineDepthStencilStateCreateInfo = DefaultPipelineDepthStencilStateCreateInfo();

		vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {
//			.stageCount = static_cast<uint32_t>(pipelineShaderStageCreateInfos.size()),
//			.pStages = pipelineShaderStageCreateInfos.data(),
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
}