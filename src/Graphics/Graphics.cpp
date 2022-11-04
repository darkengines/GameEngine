#include "Graphics.hpp"
#include "../Windows/Window.hpp"
#include "../Common/Common.hpp"
#include "../Devices/Device.hpp"
#include "../Meshes/Vertex.hpp"
#include "../Objects/Object.hpp"
#include "../Meshes/MeshGroup.hpp"
#include "Draw.hpp"
#include "../Spatials/Spatial.hpp"
#include "../Draws/DrawCommand.hpp"
#include "Models/Draw.hpp"
#include <imgui_impl_vulkan.h>
#include "../Objects/Models/Object.hpp"
#include "../Meshes/Models/Mesh.hpp"
#include "../Spatials/Models/Spatial.hpp"
#include <algorithm>
#include <vector>
#include "../Meshes/Vertex.hpp"
#include "../Materials/Material.hpp"
#include "../Cameras/Camera.hpp"
#include <stdexcept>

namespace drk::Graphics {
	std::vector<const char*> Graphics::RequiredInstanceExtensions = Windows::Window::getVulkanInstanceExtension();
	std::vector<const char*> Graphics::RequiredDeviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
	};

	Graphics::Graphics(
		Devices::DeviceContext& deviceContext,
		Engine::EngineState& engineState,
		Windows::Window& window
	) : DeviceContext(deviceContext), EngineState(engineState), Extent(window.GetExtent()) {

		CreateSwapchain(Extent);
		SceneRenderTargetTexture = BuildSceneRenderTargetTexture(DeviceContext, Swapchain);
		CreateMainRenderPass();
		CreateMainFramebufferResources();
		createMainFramebuffers();
		SetupImgui();
	}

	Graphics::~Graphics() {
		DeviceContext.device.destroyDescriptorPool(ImGuiDescriptorPool);
		ImGui_ImplVulkan_Shutdown();

		DestroyMainFramebuffer();
		DeviceContext.device.destroyRenderPass(MainRenderPass);

		DeviceContext.destroyTexture(SceneRenderTargetTexture);
		DestroyMainFramebufferResources();
		DestroySwapchain();
	}

	void Graphics::CreateSwapchain(vk::Extent2D& extent) {
		Swapchain = Devices::Device::createSwapchain(
			DeviceContext.device,
			DeviceContext.PhysicalDevice,
			DeviceContext.Surface,
			extent
		);
	}

	void Graphics::DestroySwapchain() {
		Devices::Device::destroySwapchain(DeviceContext.device, Swapchain);
	}

	void Graphics::DestroyMainFramebuffer() {
		for (const auto& framebuffer: MainFramebuffers) {
			DeviceContext.device.destroyFramebuffer(framebuffer);
		}
		MainFramebuffers.clear();
	}

	void Graphics::DestroyMainFramebufferResources() {
		DeviceContext.destroyTexture(MainFramebufferDepthTexture);
		DeviceContext.destroyTexture(MainFramebufferTexture);
	}

	void Graphics::RecreateSwapchain(vk::Extent2D extent) {
		DeviceContext.device.waitIdle();

		DestroyMainFramebuffer();
		DeviceContext.device.destroyRenderPass(MainRenderPass);
		DestroyMainFramebufferResources();
		DestroySwapchain();

		CreateSwapchain(extent);
		CreateMainRenderPass();
		CreateMainFramebufferResources();
		createMainFramebuffers();

		DeviceContext.device.waitIdle();
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
	Graphics::DefaultPipelineColorBlendStateCreateInfo(vk::PipelineColorBlendAttachmentState& pipelineColorBlendAttachmentState) {
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
			.rasterizationSamples = vk::SampleCountFlagBits::e8,
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
		const vk::Extent2D& extent,
		vk::Viewport& viewport,
		vk::Rect2D& scissor
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
		const std::vector<vk::VertexInputBindingDescription>& vertexInputBindingDescriptions,
		const std::vector<vk::VertexInputAttributeDescription>& vertexInputAttributeDescriptions
	) {
		vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {
			.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexInputBindingDescriptions.size()),
			.pVertexBindingDescriptions = vertexInputBindingDescriptions.data(),
			.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributeDescriptions.size()),
			.pVertexAttributeDescriptions = vertexInputAttributeDescriptions.data(),
		};

		return vertexInputInfo;
	}

	void Graphics::SetExtent(const vk::Extent2D& extent) {
		Extent = extent;
		ExtentChanged = true;
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
			.format = Swapchain.imageFormat,
			.samples = vk::SampleCountFlagBits::e1,
			.loadOp = vk::AttachmentLoadOp::eDontCare,
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

		auto renderPass = DeviceContext.device.createRenderPass(renderPassCreationInfo);
		MainRenderPass = renderPass;
	}

	void Graphics::CreateMainFramebufferResources() {
		vk::ImageCreateInfo imageCreateInfo{
			.imageType = vk::ImageType::e2D,
			.format = Swapchain.imageFormat,
			.extent = {Swapchain.extent.width, Swapchain.extent.height, 1},
			.mipLevels = 1,
			.arrayLayers = 1,
			.samples = vk::SampleCountFlagBits::e8,
			.usage = vk::ImageUsageFlagBits::eColorAttachment,
			.sharingMode = vk::SharingMode::eExclusive,
		};
		auto mainFramebufferImage = DeviceContext.createImage(
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

		auto mainFramebufferImageView = DeviceContext.device.createImageView(imageViewCreateInfo);

		vk::ImageCreateInfo depthImageCreateInfo{
			.imageType = vk::ImageType::e2D,
			.format = DeviceContext.DepthFormat,
			.extent = {Swapchain.extent.width, Swapchain.extent.height, 1},
			.mipLevels = 1,
			.arrayLayers = 1,
			.samples = vk::SampleCountFlagBits::e8,
			.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment,
			.sharingMode = vk::SharingMode::eExclusive,
		};
		auto mainFramebufferDepthImage = DeviceContext.createImage(
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

		auto mainFramebufferDepthImageView = DeviceContext.device.createImageView(depthImageViewCreateInfo);
		MainFramebufferTexture = {
			.image = mainFramebufferImage,
			.imageView = mainFramebufferImageView
		};
		MainFramebufferDepthTexture = {
			.image = mainFramebufferDepthImage,
			.imageView = mainFramebufferDepthImageView
		};
	}

	void Graphics::createMainFramebuffers() {
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
			auto framebuffer = DeviceContext.device.createFramebuffer(framebufferCreateInfo);
			MainFramebuffers.push_back(framebuffer);
		}
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
			.maxSets = 1000u,
			.poolSizeCount = (uint32_t) std::size(poolSizes),
			.pPoolSizes = poolSizes,
		};

		ImGuiDescriptorPool = DeviceContext.device.createDescriptorPool(descriptorPoolCreateInfo);

		ImGui_ImplVulkan_InitInfo infos{
			.Instance = DeviceContext.Instance,
			.PhysicalDevice = DeviceContext.PhysicalDevice,
			.Device = DeviceContext.device,
			.QueueFamily = 0,
			.Queue = DeviceContext.GraphicQueue,
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
		auto commandBuffer = Devices::Device::beginSingleTimeCommands(
			DeviceContext.device,
			DeviceContext.CommandPool
		);
		ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
		Devices::Device::endSingleTimeCommands(
			DeviceContext.device,
			DeviceContext.GraphicQueue,
			DeviceContext.CommandPool,
			commandBuffer
		);
		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}

	void Graphics::Render(const vk::CommandBuffer& commandBuffer, uint32_t swapchainImageIndex) const {
		ImGui::Render();

		const auto& frameState = EngineState.getCurrentFrameState();
		const auto& fence = frameState.fence;
		const auto& imageReadySemaphore = frameState.imageReadySemaphore;
		const auto& imageRenderedSemaphore = frameState.imageRenderedSemaphore;

		vk::ClearValue colorClearValue = {
			.color = {std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}}
		};
		vk::ClearValue depthClearValue{
			.depthStencil = {1.0f, 0}
		};
		std::array<vk::ClearValue, 3> clearValues{colorClearValue, depthClearValue, colorClearValue};

		vk::RenderPassBeginInfo mainRenderPassBeginInfo = {
			.renderPass = MainRenderPass,
			.framebuffer = MainFramebuffers[swapchainImageIndex],
			.renderArea = {0, 0, Swapchain.extent},
			.clearValueCount = (uint32_t) clearValues.size(),
			.pClearValues = clearValues.data(),
		};
		commandBuffer.beginRenderPass(mainRenderPassBeginInfo, vk::SubpassContents::eInline);
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
		commandBuffer.endRenderPass();
	}

	Devices::Texture
	Graphics::BuildSceneRenderTargetTexture(
		const Devices::DeviceContext& deviceContext,
		const Devices::Swapchain& swapchain
	) {
		vk::ImageCreateInfo imageCreateInfo{
			.imageType = vk::ImageType::e2D,
			.format = vk::Format::eR8G8B8A8Srgb,
			.extent = {swapchain.extent.width, swapchain.extent.height, 1},
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
	void Graphics::Present(uint32_t swapchainImageIndex) {
		const auto& frameState = EngineState.getCurrentFrameState();
		const auto& fence = frameState.fence;
		const auto& imageReadySemaphore = frameState.imageReadySemaphore;
		const auto& imageRenderedSemaphore = frameState.imageRenderedSemaphore;

		vk::Result presentResults;
		vk::PresentInfoKHR presentInfoKHR = {
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &imageRenderedSemaphore,
			.swapchainCount = 1,
			.pSwapchains = &Swapchain.swapchain,
			.pImageIndices = &swapchainImageIndex,
			.pResults = &presentResults,
		};

		vk::Result presentResult;
		bool outOfDate = false;
		try {
			presentResult = DeviceContext.PresentQueue.presentKHR(presentInfoKHR);
		} catch (const vk::OutOfDateKHRError& e) {
			outOfDate = true;
		}
		if (outOfDate || presentResult == vk::Result::eSuboptimalKHR ||
			ExtentChanged) {
			ExtentChanged = false;
			RecreateSwapchain(Extent);
			return;
		}
	}
	uint32_t Graphics::AcuireSwapchainImageIndex() {
		const auto& frameState = EngineState.getCurrentFrameState();
		auto swapchainImageIndex = DeviceContext.device.acquireNextImageKHR(
			Swapchain.swapchain,
			UINT64_MAX,
			frameState.imageReadySemaphore,
			VK_NULL_HANDLE
		);
		if (swapchainImageIndex.result == vk::Result::eSuboptimalKHR ||
			swapchainImageIndex.result == vk::Result::eErrorOutOfDateKHR || ExtentChanged) {
			ExtentChanged = false;
			RecreateSwapchain(Extent);
			return 0;
		}
		return swapchainImageIndex.value;
	}
}