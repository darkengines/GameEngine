#include <iostream>
#include <set>
#include "Device.hpp"


namespace drk::Devices {
	VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
		void *pUserData
	) {
		std::cerr << pCallbackData->pMessage << std::endl;
		return VK_FALSE;
	}


	bool Device::checkDeviceExtensionSupport(
		const vk::PhysicalDevice &physicalDevice,
		const std::vector<const char *> &requiredExtensions
	) {
		auto availableExtensions = physicalDevice.enumerateDeviceExtensionProperties();
		auto hasAllRequiredExtensions = std::all_of(
			requiredExtensions.begin(),
			requiredExtensions.end(),
			[&availableExtensions](const char *requiredExtension) {
				auto foundExtension = std::find_if(
					availableExtensions.begin(),
					availableExtensions.end(),
					[&requiredExtension](const vk::ExtensionProperties &availableExtension) {
						return !strcmp(availableExtension.extensionName, requiredExtension);
					}
				);
				return foundExtension != availableExtensions.end();
			}
		);
		return hasAllRequiredExtensions;
	}

	bool Device::isDeviceSuitable(
		const vk::PhysicalDevice &physicalDevice, const vk::SurfaceKHR &surface,
		const std::vector<const char *> &requiredExtensions
	) {
		QueueFamilyIndices indices = Device::findQueueFamilies(physicalDevice, surface);
		bool extensionsSupported = Device::checkDeviceExtensionSupport(physicalDevice, requiredExtensions);
		bool swapChainAdequate = false;
		if (extensionsSupported) {
			auto swapChainInfos = Device::querySwapChainSupport(physicalDevice, surface);
			swapChainAdequate = !swapChainInfos.formats.empty() && !swapChainInfos.presentModes.empty();
		}

		vk::PhysicalDeviceDescriptorIndexingFeatures indexingFeatures;
		vk::PhysicalDeviceFeatures2 supportedFeatures = {.pNext = &indexingFeatures};
		physicalDevice.getFeatures2(&supportedFeatures);

		return indices.isComplete() && extensionsSupported && swapChainAdequate &&
			   supportedFeatures.features.samplerAnisotropy && indexingFeatures.runtimeDescriptorArray &&
			   indexingFeatures.descriptorBindingPartiallyBound;
	}

	QueueFamilyIndices
	Device::findQueueFamilies(const vk::PhysicalDevice &physicalDevice, const vk::SurfaceKHR &surface) {
		QueueFamilyIndices indices = {};

		uint32_t queueFamilyCount = 0;
		auto queueFamilies = physicalDevice.getQueueFamilyProperties();

		int i = 0;
		for (const auto &queueFamily : queueFamilies) {
			auto presentSupport = physicalDevice.getSurfaceSupportKHR(i, surface);
			if (presentSupport) {
				indices.presentFamily = i;
			}
			if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
				indices.graphicsFamily = i;
			}
			if (queueFamily.queueFlags & vk::QueueFlagBits::eCompute) {
				indices.computeFamily = i;
			}
			if (indices.isComplete()) {
				break;
			}
			i++;
		}

		return indices;
	}

	SwapChainSupportDetails
	Device::querySwapChainSupport(const vk::PhysicalDevice &physicalDevice, const vk::SurfaceKHR &surface) {
		SwapChainSupportDetails details = {
			.capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface),
			.formats = physicalDevice.getSurfaceFormatsKHR(surface),
			.presentModes = physicalDevice.getSurfacePresentModesKHR(surface)
		};
		return details;
	}

	vk::SampleCountFlagBits Device::getMaxSampleCount(const vk::PhysicalDevice &physicalDevice) {
		auto deviceProperties = physicalDevice.getProperties();
		auto counts = deviceProperties.limits.framebufferColorSampleCounts &
					  deviceProperties.limits.framebufferDepthSampleCounts;
		if (counts & vk::SampleCountFlagBits::e64) return vk::SampleCountFlagBits::e64;
		if (counts & vk::SampleCountFlagBits::e32) return vk::SampleCountFlagBits::e32;
		if (counts & vk::SampleCountFlagBits::e16) return vk::SampleCountFlagBits::e16;
		if (counts & vk::SampleCountFlagBits::e8) return vk::SampleCountFlagBits::e8;
		if (counts & vk::SampleCountFlagBits::e4) return vk::SampleCountFlagBits::e4;
		if (counts & vk::SampleCountFlagBits::e2) return vk::SampleCountFlagBits::e2;
		return vk::SampleCountFlagBits::e1;
	}

	vk::PhysicalDevice Device::pickPhysicalDevice(
		const vk::Instance &instance, const vk::SurfaceKHR &surface,
		const std::vector<const char *> &requiredExtensions
	) {
		auto devices = instance.enumeratePhysicalDevices();
		if (devices.empty()) throw std::exception("Failed to find GPU with Vulkan support.");
		for (const auto &device : devices) {
			if (Device::isDeviceSuitable(device, surface, requiredExtensions)) {
				return device;
			}
		}
		throw std::exception("Failed to find a suitable GPU.");
	}

	VulkanLogicalDeviceInfo Device::createLogicalDevice(
		const vk::PhysicalDevice &physicalDevice,
		const vk::SurfaceKHR &surface,
		const std::vector<const char *> &requiredExtensions,
		bool enableValidationLayers,
		const std::vector<const char *> &requiredLayers
	) {
		auto indices = Device::findQueueFamilies(physicalDevice, surface);
		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = {
			indices.graphicsFamily.value(),
			indices.presentFamily.value(),
			indices.computeFamily.value()
		};
		auto queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies) {
			vk::DeviceQueueCreateInfo queueCreateInfo = {
				.queueFamilyIndex = queueFamily,
				.queueCount = 1,
				.pQueuePriorities = &queuePriority
			};
			queueCreateInfos.push_back(queueCreateInfo);
		}

		vk::PhysicalDeviceDescriptorIndexingFeatures indexingFeatures = {
			.descriptorBindingSampledImageUpdateAfterBind = VK_TRUE,
			.descriptorBindingPartiallyBound = VK_TRUE,
			.runtimeDescriptorArray = VK_TRUE,
		};
		vk::PhysicalDeviceFeatures2 deviceFeatures2 = {
			.pNext = &indexingFeatures,
			.features = {
				.geometryShader = VK_TRUE,
				.sampleRateShading = VK_TRUE,
				.fillModeNonSolid = VK_TRUE,
				.multiViewport = VK_TRUE,
				.samplerAnisotropy = VK_TRUE,
			}
		};

		vk::DeviceCreateInfo deviceCreateInfo = {
			.pNext = &deviceFeatures2,
			.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
			.pQueueCreateInfos = queueCreateInfos.data(),
			.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
			.ppEnabledExtensionNames = requiredExtensions.data(),
		};

		if (enableValidationLayers) {
			deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(requiredLayers.size());
			deviceCreateInfo.ppEnabledLayerNames = requiredLayers.data();
		}

		auto device = physicalDevice.createDevice(deviceCreateInfo);
		auto graphicQueue = device.getQueue(indices.graphicsFamily.value(), 0);
		auto presentQueue = device.getQueue(indices.presentFamily.value(), 0);
		auto computeQueue = device.getQueue(indices.computeFamily.value(), 0);

		return {
			.device=device,
			.graphicQueue = graphicQueue,
			.presentQueue = presentQueue,
			.computeQueue = computeQueue
		};
	}

	vk::Instance Device::createInstance(
		const std::vector<const char *> &requiredInstanceExtensions,
		const std::vector<const char *> &requiredLayers
	) {
		vk::ApplicationInfo applicationInfo = {
			.pApplicationName = "DarkEngines",
			.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
			.pEngineName = "DarkEngines",
			.engineVersion = VK_MAKE_VERSION(1, 0, 0),
			.apiVersion = VK_API_VERSION_1_3,
		};

		vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreationInfo = {
			.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose
							   | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo
							   | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
							   | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
			.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
						   vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
						   vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
			.pfnUserCallback = debugCallback
		};

		vk::InstanceCreateInfo instanceCreationInfo = {
			.pNext = &debugUtilsMessengerCreationInfo,
			.pApplicationInfo = &applicationInfo,
			.enabledLayerCount = (uint32_t) requiredLayers.size(),
			.ppEnabledLayerNames = requiredLayers.data(),
			.enabledExtensionCount = (uint32_t) requiredInstanceExtensions.size(),
			.ppEnabledExtensionNames = requiredInstanceExtensions.data()
		};

		auto instance = vk::createInstance(instanceCreationInfo);
		return instance;
	}

	vk::Format Device::findDepthFormat(vk::PhysicalDevice physicalDevice) {
		auto format = Device::findSupportedFormat(
			physicalDevice,
			{
				vk::Format::eD32Sfloat,
				vk::Format::eD32SfloatS8Uint,
				vk::Format::eD24UnormS8Uint,
			},
			vk::ImageTiling::eOptimal,
			vk::FormatFeatureFlagBits::eDepthStencilAttachment
		);
		return format;
	}

	vk::Format Device::findSupportedFormat(
		vk::PhysicalDevice physicalDevice,
		std::vector<vk::Format> candidates,
		vk::ImageTiling tiling,
		vk::FormatFeatureFlags features
	) {
		for (auto format : candidates) {
			auto formatProperties = physicalDevice.getFormatProperties(format);

			if (tiling == vk::ImageTiling::eLinear &&
				(formatProperties.linearTilingFeatures & features) == features) {
				return format;
			} else if (tiling == vk::ImageTiling::eOptimal &&
					   (formatProperties.optimalTilingFeatures & features) == features) {
				return format;
			} else {
				throw std::exception("Failed to find any supported format.");
			}
		}
		throw std::exception("Failed to find any supported format.");
	}

	VmaAllocator Device::createAllocator(
		const vk::Instance &instance,
		const vk::PhysicalDevice &physicalDevice,
		const vk::Device &device
	) {
		VmaVulkanFunctions vulkanFunctions = {
			.vkGetInstanceProcAddr = &vkGetInstanceProcAddr,
			.vkGetDeviceProcAddr = &vkGetDeviceProcAddr
		};

		VmaAllocatorCreateInfo allocatorCreationInfo = {
			.physicalDevice = physicalDevice,
			.device = device,
			.pVulkanFunctions = &vulkanFunctions,
			.instance = instance,
			.vulkanApiVersion = VK_API_VERSION_1_3,
		};

		VmaAllocator allocator;
		vmaCreateAllocator(&allocatorCreationInfo, &allocator);
		return allocator;
	}

	Buffer Device::createVmaBuffer(
		const VmaAllocator allocator,
		const vk::BufferUsageFlags usage,
		const VmaAllocationCreateInfo *pAllocationCreationInfo,
		vk::DeviceSize size
	) {
		vk::BufferCreateInfo bufferCreateInfo = {
			.size = size,
			.usage = usage,
			.sharingMode = vk::SharingMode::eExclusive,
		};

		vk::Buffer buffer;
		VmaAllocation allocation;
		VmaAllocationInfo allocationInfo;

		auto result = vmaCreateBuffer(
			allocator,
			(VkBufferCreateInfo *) &bufferCreateInfo,
			pAllocationCreationInfo,
			(VkBuffer *) &buffer,
			&allocation,
			&allocationInfo
		);
		if (result != VK_SUCCESS) throw std::exception("Failed to create buffer.");

		return {buffer, allocation, allocationInfo};
	}

	Buffer Device::createBuffer(
		const VmaAllocator &allocator,
		vk::MemoryPropertyFlags properties,
		vk::BufferUsageFlags usage,
		const VmaAllocationCreateInfo &vmaAllocationCreateInfo,
		vk::DeviceSize size
	) {
		auto buffer = Device::createVmaBuffer(allocator, usage, &vmaAllocationCreateInfo, size);
		return buffer;
	}

	void Device::destroyVmaBuffer(const VmaAllocator &allocator, Buffer buffer) {
		vmaDestroyBuffer(allocator, buffer.buffer, buffer.allocation);
	}

	void Device::destroyBuffer(const VmaAllocator &allocator, const Buffer &buffer) {
		Device::destroyVmaBuffer(allocator, buffer);
	}

	void Device::copyBuffer(
		const vk::Device &device,
		const vk::Queue &queue,
		const vk::CommandPool &commandPool,
		const Buffer &source,
		const Buffer &destination,
		size_t sourceOffset,
		size_t destinationOffset,
		size_t length
	) {
		auto commandBuffer = Device::beginSingleTimeCommands(device, commandPool);
		vk::BufferCopy region = {
			.srcOffset = sourceOffset,
			.dstOffset = destinationOffset,
			.size = length
		};
		commandBuffer.copyBuffer(source.buffer, destination.buffer, region);
		Device::endSingleTimeCommands(device, queue, commandPool, commandBuffer);
	}

	void Device::mapBuffer(const VmaAllocator &allocator, const Buffer &buffer, void **memory) {
		vmaMapMemory(allocator, buffer.allocation, memory);
	}

	void Device::unmapBuffer(const VmaAllocator &allocator, const Buffer &buffer) {
		vmaUnmapMemory(allocator, buffer.allocation);
	}

	vk::CommandBuffer Device::beginSingleTimeCommands(const vk::Device &device, const vk::CommandPool &commandPool) {
		vk::CommandBufferAllocateInfo commandBufferAllocationInfo = {
			.commandPool = commandPool,
			.level = vk::CommandBufferLevel::ePrimary,
			.commandBufferCount = 1,
		};
		auto commandBuffer = device.allocateCommandBuffers(commandBufferAllocationInfo)[0];
		vk::CommandBufferBeginInfo beginInfo = {
			.flags = vk::CommandBufferUsageFlagBits::eRenderPassContinue
		};
		commandBuffer.begin(beginInfo);
		return commandBuffer;
	}

	void Device::endSingleTimeCommands(
		const vk::Device &device,
		const vk::Queue &queue,
		const vk::CommandPool &commandPool,
		const vk::CommandBuffer &commandBuffer
	) {
		commandBuffer.end();
		vk::SubmitInfo submitInfo = {
			.commandBufferCount = 1,
			.pCommandBuffers = &commandBuffer,
		};
		queue.submit(submitInfo);
		queue.waitIdle();
		device.freeCommandBuffers(commandPool, {commandBuffer});
	}

	Image Device::createImage(
		const VmaAllocator &allocator,
		const vk::ImageCreateInfo imageCreationInfo,
		vk::MemoryPropertyFlags properties
	) {
		VmaAllocationCreateInfo allocationCreationInfo = {
			.usage = VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO,
			.requiredFlags = (VkMemoryPropertyFlags) properties,
		};

		auto image = Device::createVmaImage(allocator, imageCreationInfo, allocationCreationInfo);
		return image;
	}

	Image Device::createVmaImage(
		const VmaAllocator &allocator,
		const vk::ImageCreateInfo &imageCreationInfo,
		const VmaAllocationCreateInfo &allocationCreationInfo
	) {
		VkImage image;
		VmaAllocation allocation;
		VmaAllocationInfo allocationInfo;

		auto result = vmaCreateImage(
			allocator,
			(VkImageCreateInfo *) &imageCreationInfo,
			&allocationCreationInfo,
			&image,
			&allocation,
			&allocationInfo
		);
		if (result != VkResult::VK_SUCCESS) throw std::exception("Failed to create image.");
		return {image, allocation, allocationInfo};
	}

	void Device::destroyVmaImage(
		const VmaAllocator &allocator,
		const Image &image
	) {
		vmaDestroyImage(allocator, image.image, image.allocation);
	}

	void Device::destroyImage(
		const vk::Device &device,
		const VmaAllocator &allocator,
		const Image &image
	) {
		Device::destroyVmaImage(allocator, image);
	}

	vk::Sampler Device::createSampler(
		const vk::Device &device,
		vk::SamplerCreateFlags flags,
		vk::Filter magFilter,
		vk::Filter minFilter,
		vk::SamplerMipmapMode mipmapMode,
		vk::SamplerAddressMode addressModeU,
		vk::SamplerAddressMode addressModeV,
		vk::SamplerAddressMode addressModeW,
		float mipLodBias,
		vk::Bool32 enableAnisotropy,
		vk::Bool32 enableCompare,
		vk::CompareOp compareOp,
		float minLod,
		float maxLod,
		vk::BorderColor borderColor,
		vk::Bool32 unnormalizedCoordinates
	) {
		vk::SamplerCreateInfo samplerCreationInfo = {
			.flags = flags,
			.magFilter = magFilter,
			.minFilter = minFilter,
			.mipmapMode = mipmapMode,
			.addressModeU = addressModeU,
			.addressModeV = addressModeV,
			.addressModeW = addressModeW,
			.mipLodBias = mipLodBias,
			.anisotropyEnable = enableAnisotropy,
			.compareEnable = enableCompare,
			.compareOp = compareOp,
			.minLod = minLod,
			.maxLod = maxLod,
			.borderColor = borderColor,
			.unnormalizedCoordinates = unnormalizedCoordinates
		};
		auto sampler = device.createSampler(samplerCreationInfo);
		return sampler;
	}

	void Device::destroySampler(const vk::Device &device, const vk::Sampler &sampler) {
		device.destroySampler(sampler);
	}

	vk::SurfaceFormatKHR Device::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats) {
		for (const auto &availableFormat : availableFormats) {
			if (availableFormat.format == vk::Format::eB8G8R8A8Srgb &&
				availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
				return availableFormat;
			}
		}
		auto format = availableFormats[0];
		return format;
	}

	vk::PresentModeKHR Device::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes) {
		for (const auto &availablePresentMode : availablePresentModes) {
			if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
				return availablePresentMode;
			}
		}
		return vk::PresentModeKHR::eFifo;
	}

	vk::Extent2D
	Device::chooseSwapExtent(vk::Extent2D extent, const vk::SurfaceCapabilitiesKHR &capabilities) {
		if (capabilities.currentExtent.width != UINT32_MAX) {
			return capabilities.currentExtent;
		} else {
			extent.width = std::max(
				capabilities.minImageExtent.width,
				std::min(capabilities.maxImageExtent.width, extent.width));
			extent.height = std::max(
				capabilities.minImageExtent.height,
				std::min(capabilities.maxImageExtent.height, extent.height));

			return extent;
		}
	}

	Swapchain Device::createSwapchain(
		const vk::Device &device,
		const vk::PhysicalDevice &physicalDevice,
		const vk::SurfaceKHR &surface,
		const vk::Extent2D &extent
	) {
		SwapChainSupportDetails swapChainSupport = Device::querySwapChainSupport(physicalDevice, surface);
		auto surfaceFormat = Device::chooseSwapSurfaceFormat(swapChainSupport.formats);
		auto presentMode = Device::chooseSwapPresentMode(swapChainSupport.presentModes);
		auto swapExtent = Device::chooseSwapExtent(extent, swapChainSupport.capabilities);

		auto imageCount = swapChainSupport.capabilities.minImageCount + 1;
		if (swapChainSupport.capabilities.maxImageCount > 0 &&
			imageCount > swapChainSupport.capabilities.maxImageCount) {
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		vk::SwapchainCreateInfoKHR swapChainCreationInfo = {
			.surface = surface,
			.minImageCount = imageCount,
			.imageFormat = surfaceFormat.format,
			.imageColorSpace = surfaceFormat.colorSpace,
			.imageExtent = swapExtent,
			.imageArrayLayers = 1,
			.imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
		};

		QueueFamilyIndices indices = Device::findQueueFamilies(physicalDevice, surface);
		uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

		if (indices.graphicsFamily != indices.presentFamily) {
			swapChainCreationInfo.imageSharingMode = vk::SharingMode::eConcurrent;
			swapChainCreationInfo.queueFamilyIndexCount = 2;
			swapChainCreationInfo.pQueueFamilyIndices = queueFamilyIndices;
		} else {
			swapChainCreationInfo.imageSharingMode = vk::SharingMode::eExclusive;
			swapChainCreationInfo.queueFamilyIndexCount = 0;
			swapChainCreationInfo.pQueueFamilyIndices = nullptr;
		}
		swapChainCreationInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		swapChainCreationInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
		swapChainCreationInfo.presentMode = presentMode;
		swapChainCreationInfo.clipped = VK_TRUE;

		swapChainCreationInfo.oldSwapchain = VK_NULL_HANDLE;

		auto vkSwapchain = device.createSwapchainKHR(swapChainCreationInfo);
		auto swapchainImages = device.getSwapchainImagesKHR(vkSwapchain);
		std::vector<Image> swapchainTextures(swapchainImages.size());
		std::transform(
			swapchainImages.begin(),
			swapchainImages.end(),
			swapchainTextures.data(),
			[](const vk::Image &vkImage) {
				return Image{
					.image = vkImage
				};
			}
		);
		std::vector<vk::ImageView> swapchainImageViews(swapchainImages.size());
		std::transform(
			swapchainTextures.begin(),
			swapchainTextures.end(),
			swapchainImageViews.data(),
			[&device, surfaceFormat](const auto &swapchainImage) {
				vk::ImageViewCreateInfo imageViewCreateInfo = {
					.image = swapchainImage.image,
					.viewType = vk::ImageViewType::e2D,
					.format = surfaceFormat.format,
					.subresourceRange = {
						.aspectMask=vk::ImageAspectFlagBits::eColor,
						.levelCount = 1,
						.layerCount = 1,
					}
				};
				return device.createImageView(imageViewCreateInfo);
			}
		);

		Swapchain swapchain = {
			.swapchain = vkSwapchain,
			.imageFormat = surfaceFormat.format,
			.extent = swapExtent,
			.images = swapchainImages,
			.imageViews = swapchainImageViews,
		};

		return swapchain;
	}

	void Device::destroySwapchain(const vk::Device &device, const Swapchain &swapchain) {
		for (auto &swapchainImageView : swapchain.imageViews) {
			device.destroyImageView(swapchainImageView);
		}
		device.destroySwapchainKHR(swapchain.swapchain);
	}

	vk::ShaderModule Device::createShaderModules(const vk::Device &device, uint32_t codeSize, const uint32_t *pCode) {
		vk::ShaderModuleCreateInfo createInfo = {
			.codeSize = codeSize,
			.pCode = pCode
		};

		auto shaderModule = device.createShaderModule(createInfo);
		return shaderModule;
	}

	void Device::destroyShaderModule(const vk::Device &device, const vk::ShaderModule &shaderModule) {
		device.destroyShaderModule(shaderModule);
	}

	void Device::copyBufferToImage(
		const vk::CommandBuffer &commandBuffer,
		const Devices::Buffer &source,
		const vk::Image &destination,
		const vk::BufferImageCopy &region
	) {
		commandBuffer.copyBufferToImage(
			source.buffer,
			destination,
			vk::ImageLayout::eTransferDstOptimal,
			1,
			&region
		);
	}

	void Device::transitionLayout(
		const vk::CommandBuffer &commandBuffer,
		const vk::Image &image,
		vk::Format format,
		vk::ImageLayout oldLayout,
		vk::ImageLayout newLayout,
		uint32_t mipLevels
	) {
		vk::ImageMemoryBarrier barrier = {
			.oldLayout = oldLayout,
			.newLayout = newLayout,
			.srcQueueFamilyIndex = ~0U,
			.dstQueueFamilyIndex = ~0U,
			.image = image,
			.subresourceRange = {
				.aspectMask = vk::ImageAspectFlagBits::eColor,
				.baseMipLevel = 0,
				.levelCount = mipLevels,
				.baseArrayLayer = 0,
				.layerCount = 1
			}
		};

		vk::PipelineStageFlags source, destination;

		if (newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal) {
			barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
			if (hasStencilComponent(format)) {
				barrier.subresourceRange.aspectMask |= vk::ImageAspectFlagBits::eStencil;
			}
		} else {
			barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		}
		if (oldLayout == vk::ImageLayout::eUndefined &&
			newLayout == vk::ImageLayout::eTransferDstOptimal) {
			barrier.srcAccessMask = vk::AccessFlagBits::eNone;
			barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

			source = vk::PipelineStageFlagBits::eTopOfPipe;
			destination = vk::PipelineStageFlagBits::eTransfer;
		} else if (oldLayout == vk::ImageLayout::eTransferDstOptimal &&
				   newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
			barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
			barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
			source = vk::PipelineStageFlagBits::eTransfer;
			destination = vk::PipelineStageFlagBits::eFragmentShader;
		} else if (oldLayout == vk::ImageLayout::eUndefined &&
				   newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal) {
			barrier.srcAccessMask = vk::AccessFlagBits::eNone;
			barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite |
									vk::AccessFlagBits::eDepthStencilAttachmentRead;
			source = vk::PipelineStageFlagBits::eTopOfPipe;
			destination = vk::PipelineStageFlagBits::eEarlyFragmentTests;
		} else {
			throw std::runtime_error("Unsupported layout transition!");
		}
		commandBuffer.pipelineBarrier(
			source,
			destination,
			{},
			{},
			{},
			std::vector<vk::ImageMemoryBarrier>{barrier}
		);
	}

	void Device::generatedMipmaps(
		const vk::CommandBuffer &commandBuffer,
		const vk::Image &image,
		int32_t width,
		int32_t height,
		uint32_t mipLevels
	) {
		vk::ImageMemoryBarrier barrier = {
			.image = image,
			.subresourceRange = {
				.aspectMask = vk::ImageAspectFlagBits::eColor,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1,
			}
		};


		int32_t mipWidth = width;
		int32_t mipHeight = height;

		for (uint32_t mipLevel = 1u; mipLevel < mipLevels; mipLevel++) {
			barrier.subresourceRange.baseMipLevel = mipLevel - 1u;
			barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
			barrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
			barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
			barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;

			commandBuffer.pipelineBarrier(
				vk::PipelineStageFlagBits::eTransfer,
				vk::PipelineStageFlagBits::eTransfer,
				{},
				{},
				{},
				{barrier}
			);

			vk::ImageBlit imageBlit = {
				.srcSubresource = {
					.aspectMask = vk::ImageAspectFlagBits::eColor,
					.mipLevel = mipLevel - 1u,
					.baseArrayLayer = 0,
					.layerCount = 1,
				},
				.srcOffsets = std::array<vk::Offset3D, 2>{
					vk::Offset3D{0, 0, 0},
					vk::Offset3D{mipWidth, mipHeight, 1}
				},
				.dstSubresource = {
					.aspectMask = vk::ImageAspectFlagBits::eColor,
					.mipLevel = mipLevel,
					.baseArrayLayer = 0,
					.layerCount = 1,
				},
				.dstOffsets = std::array<vk::Offset3D, 2>{
					vk::Offset3D{0, 0, 0},
					vk::Offset3D{
						mipWidth > 1 ? mipWidth / 2 : 1,
						mipHeight > 1 ? mipHeight / 2 : 1,
						1
					}
				}
			};


			commandBuffer.blitImage(
				image,
				vk::ImageLayout::eTransferSrcOptimal,
				image,
				vk::ImageLayout::eTransferDstOptimal,
				{imageBlit},
				vk::Filter::eLinear
			);

			barrier.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
			barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
			barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
			barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

			commandBuffer.pipelineBarrier(
				vk::PipelineStageFlagBits::eTransfer,
				vk::PipelineStageFlagBits::eFragmentShader,
				{},
				{},
				{},
				{barrier}
			);

			if (mipWidth > 1) mipWidth /= 2;
			if (mipHeight > 1) mipHeight /= 2;
		}

		barrier.subresourceRange.baseMipLevel = mipLevels - 1;
		barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
		barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
		barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

		commandBuffer.pipelineBarrier(
			vk::PipelineStageFlagBits::eTransfer,
			vk::PipelineStageFlagBits::eFragmentShader,
			{},
			{},
			{},
			{barrier}
		);
	}
}