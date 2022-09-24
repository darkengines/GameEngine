#include <iostream>
#include <set>
#include <numeric>
#include "Device.hpp"
#include <algorithm>

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
		std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};
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

		return {
			.device=device,
			.graphicQueue = graphicQueue,
			.presentQueue = presentQueue
		};
	}

	Context Device::createContext(
		GLFWwindow *window,
		const std::vector<const char *> &requiredInstanceExtensions,
		const std::vector<const char *> &requiredDeviceExtensions,
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
			.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
							   vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
							   vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
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

		VkSurfaceKHR surface;
		glfwCreateWindowSurface(instance, window, nullptr, &surface);

		auto physicalDevice = Device::pickPhysicalDevice(instance, surface, requiredDeviceExtensions);
		auto logicalDeviceInfo = Device::createLogicalDevice(
			physicalDevice,
			surface,
			requiredDeviceExtensions,
			true,
			requiredLayers
		);
		auto allocator = Device::createAllocator(instance, physicalDevice, logicalDeviceInfo.device);

		Context context = {
			.instance = instance,
			.surface = surface,
			.physicalDevice = physicalDevice,
			.device = logicalDeviceInfo.device,
			.graphicQueue = logicalDeviceInfo.graphicQueue,
			.presentQueue = logicalDeviceInfo.presentQueue,
			.vmaAllocator = allocator
		};

		return context;
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
			vk::FormatProperties properties;
			auto formatProperties = physicalDevice.getFormatProperties(format);

			if (tiling == vk::ImageTiling::eLinear &&
				(properties.linearTilingFeatures & features) == features) {
				return format;
			} else if (tiling == vk::ImageTiling::eOptimal &&
					   (properties.optimalTilingFeatures & features) == features) {
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

	void Device::destroyContext(const Context &context) {
		vmaDestroyAllocator(context.vmaAllocator);
		context.device.destroy();
		context.instance.destroy();
	}

	Buffer Device::createVmaBuffer(
		const VmaAllocator allocator,
		const vk::BufferUsageFlags usage,
		VmaAllocationCreateInfo *pAllocationCreationInfo,
		vk::DeviceSize size
	) {
		vk::BufferCreateInfo bufferCreateInfo = {
			.size = size,
			.usage = usage,
			.sharingMode = vk::SharingMode::eExclusive
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
		vk::MemoryPropertyFlags memoryProperties,
		VmaMemoryUsage vmaUsage,
		vk::BufferUsageFlags usage,
		vk::DeviceSize size
	) {
		VmaAllocationCreateInfo vmaAllocationCreationInfo = {
			.usage = vmaUsage,
			.requiredFlags = (VkMemoryPropertyFlags) memoryProperties,
		};
		auto buffer = Device::createVmaBuffer(allocator, usage, &vmaAllocationCreationInfo, size);
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

	template<typename TBuffer>
	std::vector<BufferView> Device::uploadBuffers(
		const vk::Device &device,
		const vk::Queue &queue,
		const vk::CommandPool &commandPool,
		const VmaAllocator &allocator,
		std::vector<std::span<TBuffer>> buffers,
		vk::BufferUsageFlagBits memoryUsage,
		Buffer **deviceBuffer
	) {
		const auto bufferCount = buffers.size();
		const auto &properties = Device->GetPhysicalDeviceMemoryProperties();
		const auto stagingMemoryTypeIterator = std::find_if(
			properties.memoryTypes.begin(),
			properties.memoryTypes.end(),
			[](const auto &memoryType) {
				return memoryType.propertyFlags & vk::MemoryPropertyFlagBits::eHostVisible;
			}
		);
		const auto stagingMemoryTypeIndex = std::distance(properties.memoryTypes.begin(), stagingMemoryTypeIterator);
		const auto stagingMemoryHeap = properties.memoryHeaps[stagingMemoryTypeIndex];
		const auto itemSize = sizeof(TBuffer);
		const auto bufferLength = std::accumulate(
			buffers.begin(), buffers.end(), 0, [](size_t totalBytes, const auto &buffer) {
				return totalBytes + buffer.size();
			}
		);
		const auto bufferByteLength = bufferLength * itemSize;
		const auto stagingBufferByteLength = std::min(stagingMemoryHeap.size, bufferByteLength);

		auto buffer = Device::createBuffer(
			allocator,
			vk::MemoryPropertyFlagBits::eDeviceLocal,
			memoryUsage | vk::BufferUsageFlagBits::eTransferDst,
			bufferByteLength
		);
		auto stagingBuffer = Device::createBuffer(
			allocator,
			vk::MemoryPropertyFlagBits::eHostVisible,
			vk::BufferUsageFlagBits::eTransferSrc,
			stagingBufferByteLength
		);

		char *mappedStagingBufferMemory;
		vmaMapMemory(allocator, stagingBuffer.allocation, &mappedStagingBufferMemory);

		auto stagingBufferAvailableByteLength = stagingBufferByteLength;
		auto remainingBufferByteLength = bufferByteLength;

		auto bufferIndex = 0;
		auto currentBuffer = buffers[0];
		auto currentBufferByteLength = currentBuffer.size() * itemSize;
		auto currentBufferByteOffset = 0;
		auto currentBufferRemainingByteLength = currentBufferByteLength;
		auto deviceBufferByteOffset = 0u;
		std::vector<BufferView> bufferViews(bufferCount);

		while (remainingBufferByteLength) {
			auto availableStagingBufferByteLength = stagingBufferByteLength;
			while (availableStagingBufferByteLength || bufferIndex >= bufferCount) {
				const auto stagingBufferByteOffset = stagingBufferByteLength - availableStagingBufferByteLength;
				const auto writableByteLength = std::min(currentBufferByteLength, availableStagingBufferByteLength);
				memcpy(
					mappedStagingBufferMemory + stagingBufferByteOffset,
					currentBuffer.data() + currentBufferByteOffset,
					writableByteLength
				);
				availableStagingBufferByteLength -= writableByteLength;
				currentBufferRemainingByteLength -= writableByteLength;
				if (currentBufferRemainingByteLength <= 0) {
					bufferIndex++;
					if (bufferIndex < bufferCount) {
						currentBuffer = buffers[bufferIndex];
						currentBufferByteLength = currentBuffer.size() * itemSize;
						currentBufferByteOffset = 0;
						currentBufferRemainingByteLength = currentBufferByteLength;
						bufferViews[bufferIndex] = {
							*deviceBuffer,
							deviceBufferByteOffset,
							currentBufferByteLength
						};
					}
				} else {
					currentBufferByteOffset += writableByteLength;
				}
			}
			const auto writableByteLength = stagingBufferByteLength - availableStagingBufferByteLength;
			Device::copyBuffer(
				device,
				queue,
				commandPool,
				stagingBuffer,
				*deviceBuffer,
				0,
				deviceBufferByteOffset,
				writableByteLength
			);
			deviceBufferByteOffset += writableByteLength;
			remainingBufferByteLength -= writableByteLength;
		}

		Device::unmapBuffer(allocator, stagingBuffer);
		Device::destroyBuffer(allocator, stagingBuffer);

		return bufferViews;
	}

	void Device::mapBuffer(const VmaAllocator &allocator, const Buffer &buffer, void *memory) {
		vmaMapMemory(allocator, buffer.allocation, &memory);
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

	Textures::Texture Device::createTexture(
		const Devices::Context &context,
		const drk::Textures::Image &image,
		uint32_t arrayLayers,
		uint32_t mipLevels,
		vk::ImageType imageType,
		vk::Format format,
		vk::ImageLayout initialLayout,
		vk::SampleCountFlagBits samples,
		vk::ImageUsageFlags usage,
		vk::MemoryPropertyFlags requiredProperties,
		VmaMemoryUsage vmaUsage
	) {
		vk::ImageCreateInfo imageCreationInfo = {
			.imageType = imageType,
			.format = format,
			.extent = {image.width, image.height, image.depth},
			.mipLevels = mipLevels,
			.arrayLayers = arrayLayers,
			.samples = samples,
			.usage = usage,
			.sharingMode = vk::SharingMode::eExclusive,
			.queueFamilyIndexCount = 0,
			.initialLayout = initialLayout,
		};

		VmaAllocationCreateInfo vmaAllocationInfo = {
			.usage = vmaUsage,
			.requiredFlags = (VkMemoryPropertyFlags) requiredProperties,
		};

		auto texture = Device::createVmaImage(context, imageCreationInfo, vmaAllocationInfo);
		return texture;
	}

	Textures::Texture Device::createVmaImage(
		const Devices::Context &context,
		vk::ImageCreateInfo imageCreationInfo,
		VmaAllocationCreateInfo vmaAllocationInfo
	) {
		VkImage image;
		VmaAllocation allocation;
		VmaAllocationInfo allocationInfo;

		auto result = vmaCreateImage(
			context.vmaAllocator,
			(VkImageCreateInfo *) &imageCreationInfo,
			&vmaAllocationInfo,
			&image,
			&allocation,
			&allocationInfo
		);
		if (result != VkResult::VK_SUCCESS) throw std::exception("Failed to create image.");
		return {image, allocation, allocationInfo};
	}

	void Device::destroyVmaImage(
		const VmaAllocator &allocator,
		const Textures::Texture &texture
	) {
		vmaDestroyImage(allocator, texture.image, texture.allocation);
	}

	void Device::destroyTexture(
		const VmaAllocator &allocator,
		const Textures::Texture &texture
	) {
		Device::destroyVmaImage(allocator, texture);
	}

	vk::ImageView Device::createImageView(
		const vk::Device &device,
		const Textures::Texture &texture,
		vk::ImageViewCreateFlags flags,
		vk::ImageViewType type,
		vk::Format format,
		vk::ImageAspectFlags aspect,
		const vk::ComponentMapping &components,
		const vk::ImageSubresourceRange &subresourceRange
	) {
		vk::ImageViewCreateInfo imageViewCreationInfo = {
			.flags = flags,
			.image = texture.image,
			.viewType = type,
			.format = format,
			.components = components,
			.subresourceRange = subresourceRange
		};
		auto imageView = device.createImageView(imageViewCreationInfo);
		return imageView;
	}

	void Device::destroyImageView(const vk::Device &device, const vk::ImageView &imageView) {
		device.destroyImageView(imageView);
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
	Device::chooseSwapExtent(GLFWwindow *window, const vk::SurfaceCapabilitiesKHR &capabilities) {
		if (capabilities.currentExtent.width != UINT32_MAX) {
			return capabilities.currentExtent;
		} else {
			int width, height;
			glfwGetFramebufferSize(window, &width, &height);

			vk::Extent2D actualExtent = {
				.width = static_cast<uint32_t>(width),
				.height = static_cast<uint32_t>(height)
			};

			actualExtent.width = std::max(
				capabilities.minImageExtent.width,
				std::min(capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(
				capabilities.minImageExtent.height,
				std::min(capabilities.maxImageExtent.height, actualExtent.height));

			return actualExtent;
		}
	}

	Swapchain Device::createSwapchain(
		const vk::Device &device,
		GLFWwindow *window,
		const vk::PhysicalDevice &physicalDevice,
		const vk::SurfaceKHR &surface
	) {
		SwapChainSupportDetails swapChainSupport = Device::querySwapChainSupport(physicalDevice, surface);
		auto surfaceFormat = Device::chooseSwapSurfaceFormat(swapChainSupport.formats);
		auto presentMode = Device::chooseSwapPresentMode(swapChainSupport.presentModes);
		auto swapExtent = Device::chooseSwapExtent(window, swapChainSupport.capabilities);

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
		std::vector<Textures::Texture> swapchainTextures(swapchainImages.size());
		std::transform(
			swapchainImages.begin(),
			swapchainImages.end(),
			swapchainTextures.data(),
			[](const vk::Image &vkImage) {
				return Textures::Texture{
					.image = vkImage
				};
			}
		);
		std::vector<vk::ImageView> swapchainImageViews(swapchainImages.size());
		std::transform(
			swapchainTextures.begin(),
			swapchainTextures.end(),
			swapchainImageViews.data(),
			[&device, surfaceFormat](const auto &swapchainTexture) {
				return Device::createImageView(
					device,
					swapchainTexture,
					{},
					vk::ImageViewType::e2D,
					surfaceFormat.format,
					vk::ImageAspectFlagBits::eColor,
					{},
					{}

				);
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
			Device::destroyImageView(device, swapchainImageView);
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
}