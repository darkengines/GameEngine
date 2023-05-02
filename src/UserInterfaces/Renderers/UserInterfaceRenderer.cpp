#include "UserInterfaceRenderer.hpp"
#include "../../Devices/ImageInfo.hpp"
#include "imgui_impl_vulkan.h"
#include <functional>

namespace drk::UserInterfaces::Renderers {
	UserInterfaceRenderer::UserInterfaceRenderer(
		Devices::DeviceContext& deviceContext,
		Engine::EngineState& engineState
	) : DeviceContext(deviceContext), EngineState(engineState) {}
	UserInterfaceRenderer::~UserInterfaceRenderer() {
		DeviceContext.device.destroyDescriptorPool(ImGuiDescriptorPool);
		ImGui_ImplVulkan_Shutdown();
		DestroyMainFramebuffer();
		DeviceContext.device.destroyRenderPass(MainRenderPass);
		if (SceneRenderTargetTexture.has_value()) DeviceContext.destroyTexture(SceneRenderTargetTexture.value());
		DestroyMainFramebufferResources();
	}
	void UserInterfaceRenderer::DestroyMainFramebuffer() {
		for (const auto& framebuffer: MainFramebuffers) {
			DeviceContext.device.destroyFramebuffer(framebuffer);
		}
		MainFramebuffers.clear();
	}

	void UserInterfaceRenderer::DestroyMainFramebufferResources() {
		if (MainFramebufferDepthTexture.has_value()) DeviceContext.destroyTexture(MainFramebufferDepthTexture.value());
		if (MainFramebufferTexture.has_value()) DeviceContext.destroyTexture(MainFramebufferTexture.value());
	}
	void UserInterfaceRenderer::SetTargetImageViews(
		Devices::ImageInfo targetImageInfo,
		std::vector<vk::ImageView> targetImageViews
	) {
		this->targetImageInfo = targetImageInfo;
		this->targetImageViews = targetImageViews;
		RecreateRenderPass();
	}
	void UserInterfaceRenderer::CreateMainRenderPass() {
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
			.format = targetImageInfo->format,
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
		vk::RenderPassCreateInfo renderPassCreateInfo = {
			.attachmentCount = static_cast<uint32_t>(attachments.size()),
			.pAttachments = attachments.data(),
			.subpassCount = 1,
			.pSubpasses = &subpass,
			.dependencyCount = 1,
			.pDependencies = &dependency,
		};
		auto renderPass = DeviceContext.device.createRenderPass(renderPassCreateInfo);
		MainRenderPass = renderPass;
	}

	void UserInterfaceRenderer::RecreateRenderPass() {
		DeviceContext.device.waitIdle();
		if (isImGuiInitialized) ImGui_ImplVulkan_Shutdown();
		DestroyMainFramebuffer();
		if (MainRenderPass) DeviceContext.device.destroyRenderPass(MainRenderPass);
		if (SceneRenderTargetTexture.has_value()) DeviceContext.destroyTexture(SceneRenderTargetTexture.value());
		DestroyMainFramebufferResources();

		CreateMainFramebufferResources();
		CreateMainRenderPass();
		CreateMainFramebuffers();
		SetupImgui();
	}

	void UserInterfaceRenderer::CreateMainFramebufferResources() {
		assert(this->targetImageInfo.has_value());
		vk::ImageCreateInfo imageCreateInfo{
			.imageType = vk::ImageType::e2D,
			.format = targetImageInfo->format,
			.extent = {targetImageInfo->extent.width, targetImageInfo->extent.height, 1},
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
			.format = targetImageInfo->format,
			.subresourceRange = subresourceRange
		};

		auto mainFramebufferImageView = DeviceContext.device.createImageView(imageViewCreateInfo);

		vk::ImageCreateInfo depthImageCreateInfo{
			.imageType = vk::ImageType::e2D,
			.format = DeviceContext.DepthFormat,
			.extent = {targetImageInfo->extent.width, targetImageInfo->extent.height, 1},
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

	void UserInterfaceRenderer::CreateMainFramebuffers() {
		for (const auto& swapChainImageView: targetImageViews) {
			std::array<vk::ImageView, 3> attachments{
				MainFramebufferTexture->imageView,
				MainFramebufferDepthTexture->imageView,
				swapChainImageView
			};
			vk::FramebufferCreateInfo framebufferCreateInfo = {
				.renderPass = MainRenderPass,
				.attachmentCount = (uint32_t) attachments.size(),
				.pAttachments = attachments.data(),
				.width = targetImageInfo->extent.width,
				.height = targetImageInfo->extent.height,
				.layers = 1
			};
			auto framebuffer = DeviceContext.device.createFramebuffer(framebufferCreateInfo);
			MainFramebuffers.push_back(framebuffer);
		}
	}
	void UserInterfaceRenderer::CreateImguiResources() {
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
	}
	void UserInterfaceRenderer::SetupImgui() {
		if (!isImGuiInitialized) {
			CreateImguiResources();
		}
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
		if (!isImGuiInitialized) {
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
		isImGuiInitialized = true;
	}

	void UserInterfaceRenderer::render(uint32_t targetImageIndex, const vk::CommandBuffer& commandBuffer) {
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
			.framebuffer = MainFramebuffers[targetImageIndex],
			.renderArea = {0, 0, targetImageInfo->extent},
			.clearValueCount = (uint32_t) clearValues.size(),
			.pClearValues = clearValues.data(),
		};
		commandBuffer.beginRenderPass(mainRenderPassBeginInfo, vk::SubpassContents::eInline);
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
		commandBuffer.endRenderPass();
	}
}