#pragma once
#define VULKAN_HPP_NO_CONSTRUCTORS
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1

#include <vulkan/vulkan.hpp>

#include "QueueFamilyIndices.hpp"
#include "SwapChainSupportDetails.hpp"
#include "VulkanLogicalDeviceInfo.hpp"
#include "BufferView.hpp"
#include "Texture.hpp"
#include "Swapchain.hpp"
#include "BufferUploadResult.hpp"
#include <algorithm>
#include <numeric>
#include <iostream>

namespace drk::Devices {
	class Device {
	public:
		static bool checkDeviceExtensionSupport(
			const vk::PhysicalDevice &device,
			const std::vector<const char *> &requiredExtensions
		);

		static QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice &device, const vk::SurfaceKHR &surface);

		static SwapChainSupportDetails
		querySwapChainSupport(const vk::PhysicalDevice &device, const vk::SurfaceKHR &surface);

		static vk::SampleCountFlagBits getMaxSampleCount(const vk::PhysicalDevice &device);

		static bool isDeviceSuitable(
			const vk::PhysicalDevice &device, const vk::SurfaceKHR &surface,
			const std::vector<const char *> &requiredExtensions
		);

		static vk::PhysicalDevice pickPhysicalDevice(
			const vk::Instance &instance, const vk::SurfaceKHR &surface,
			const std::vector<const char *> &requiredExtensions
		);

		static VulkanLogicalDeviceInfo createLogicalDevice(
			const vk::PhysicalDevice &physicalDevice,
			const vk::SurfaceKHR &surface,
			const std::vector<const char *> &requiredExtensions,
			bool enableValidationLayers,
			const std::vector<const char *> &requiredLayers
		);

		static vk::Format findDepthFormat(vk::PhysicalDevice physicalDevice);

		static vk::Format findSupportedFormat(
			vk::PhysicalDevice physicalDevice,
			std::vector<vk::Format> candidates,
			vk::ImageTiling tiling,
			vk::FormatFeatureFlags features
		);

		static VmaAllocator createAllocator(
			const vk::Instance &instance,
			const vk::PhysicalDevice &physicalDevice,
			const vk::Device &device
		);

		static Buffer createVmaBuffer(
			const VmaAllocator allocator,
			const vk::BufferUsageFlags usage,
			const VmaAllocationCreateInfo *pAllocationCreationInfo,
			vk::DeviceSize size
		);

		static Buffer createBuffer(
			const VmaAllocator &allocator,
			vk::MemoryPropertyFlags properties,
			vk::BufferUsageFlags usage,
			const VmaAllocationCreateInfo &pAllocationCreationInfo,
			vk::DeviceSize size
		);

		static void destroyVmaBuffer(const VmaAllocator &allocator, Buffer buffer);

		static void destroyBuffer(const VmaAllocator &allocator, const Buffer &buffer);

		static void mapBuffer(const VmaAllocator &allocator, const Buffer &buffer, void **memory);

		static void unmapBuffer(const VmaAllocator &allocator, const Buffer &buffer);

		static void copyBuffer(
			const vk::Device &device,
			const vk::Queue &queue,
			const vk::CommandPool &commandPool,
			const Buffer &source,
			const Buffer &destination,
			size_t sourceOffset,
			size_t destinationOffset,
			size_t length
		);

		static void copyBufferToImage(
			const vk::CommandBuffer &commandBuffer,
			const Devices::Buffer &source,
			const vk::Image &destination,
			const vk::BufferImageCopy &region
		);

		static vk::CommandBuffer beginSingleTimeCommands(const vk::Device &device, const vk::CommandPool &commandPool);

		static void endSingleTimeCommands(
			const vk::Device &device,
			const vk::Queue &queue,
			const vk::CommandPool &commandPool,
			const vk::CommandBuffer &commandBuffer
		);

		template<typename TBuffer>
		static BufferUploadResult uploadBuffers(
			const vk::PhysicalDevice &physicalDevice,
			const vk::Device &device,
			const vk::Queue &queue,
			const vk::CommandPool &commandPool,
			const VmaAllocator &allocator,
			std::vector<std::span<TBuffer>> buffers,
			vk::BufferUsageFlagBits memoryUsage
		) {
			const auto bufferCount = buffers.size();
			const auto &properties = physicalDevice.getMemoryProperties();
			const auto stagingMemoryTypeIterator = std::find_if(
				properties.memoryTypes.begin(),
				properties.memoryTypes.end(),
				[](const auto &memoryType) {
					return memoryType.propertyFlags & vk::MemoryPropertyFlagBits::eHostVisible;
				}
			); 
			const auto stagingMemoryHeap = properties.memoryHeaps[stagingMemoryTypeIterator->heapIndex];
			const auto itemSize = sizeof(TBuffer);
			vk::DeviceSize bufferLength = std::accumulate(
				buffers.begin(), buffers.end(), 0, [](size_t totalBytes, const auto &buffer) {
					return totalBytes + buffer.size();
				}
			);
			vk::DeviceSize bufferByteLength = bufferLength * itemSize;
			vk::DeviceSize stagingBufferByteLength = std::min(stagingMemoryHeap.size, bufferByteLength);

			VmaAllocationCreateInfo allocationCreationInfo = {
				.usage = VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO,
			};
			auto buffer = Device::createBuffer(
				allocator,
				vk::MemoryPropertyFlagBits::eDeviceLocal,
				memoryUsage | vk::BufferUsageFlagBits::eTransferDst,
				allocationCreationInfo,
				bufferByteLength
			);

			VmaAllocationCreateInfo stagingAllocationCreationInfo = {
				.flags = VmaAllocationCreateFlagBits::VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
				.usage = VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO,
				.requiredFlags = (VkMemoryPropertyFlags) (vk::MemoryPropertyFlagBits::eHostVisible |
														  vk::MemoryPropertyFlagBits::eHostCoherent),
			};
			auto stagingBuffer = Device::createBuffer(
				allocator,
				vk::MemoryPropertyFlagBits::eHostVisible,
				vk::BufferUsageFlagBits::eTransferSrc,
				stagingAllocationCreationInfo,
				stagingBufferByteLength
			);

			char *mappedStagingBufferMemory;
			vmaMapMemory(allocator, stagingBuffer.allocation, (void**)&mappedStagingBufferMemory);

			vk::DeviceSize remainingBufferByteLength = bufferByteLength;

			auto bufferIndex = 0;
			auto currentBuffer = (char*)buffers[0].data();
			vk::DeviceSize currentBufferByteLength = buffers[0].size() * itemSize;
			vk::DeviceSize currentBufferByteOffset = 0;
			vk::DeviceSize currentBufferRemainingByteLength = currentBufferByteLength;
			vk::DeviceSize deviceBufferByteOffset = 0u;
			std::vector<BufferView> bufferViews(bufferCount);

			while (remainingBufferByteLength) {
				auto availableStagingBufferByteLength = stagingBufferByteLength;
				while (availableStagingBufferByteLength && bufferIndex < bufferCount) {
					if (currentBufferByteOffset == 0) {
						bufferViews[bufferIndex] = {
							buffer,
							deviceBufferByteOffset + stagingBufferByteLength - availableStagingBufferByteLength,
							currentBufferByteLength
						};
					}
					const auto stagingBufferByteOffset = stagingBufferByteLength - availableStagingBufferByteLength;
					const auto writableByteLength = std::min(currentBufferRemainingByteLength, availableStagingBufferByteLength);
					memcpy(
						mappedStagingBufferMemory + stagingBufferByteOffset,
						currentBuffer + currentBufferByteOffset,
						writableByteLength
					);
					availableStagingBufferByteLength -= writableByteLength;
					currentBufferRemainingByteLength -= writableByteLength;
					if (currentBufferRemainingByteLength <= 0) {
						bufferIndex++;
						if (bufferIndex < bufferCount) {
							currentBuffer = (char*)buffers[bufferIndex].data();
							currentBufferByteLength = buffers[bufferIndex].size() * itemSize;
							currentBufferByteOffset = 0;
							currentBufferRemainingByteLength = currentBufferByteLength;
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
					buffer,
					0,
					deviceBufferByteOffset,
					writableByteLength
				);
				deviceBufferByteOffset += writableByteLength;
				remainingBufferByteLength -= writableByteLength;
			}

			Device::unmapBuffer(allocator, stagingBuffer);
			Device::destroyBuffer(allocator, stagingBuffer);

			return BufferUploadResult{
				.buffer = buffer,
				.bufferViews = bufferViews
			};
		}

		static Image createImage(
			const VmaAllocator &allocator,
			const vk::ImageCreateInfo imageCreationInfo,
			vk::MemoryPropertyFlags properties
		);

		static Image createVmaImage(
			const VmaAllocator &allocator,
			const vk::ImageCreateInfo &imageCreationInfo,
			const VmaAllocationCreateInfo &allocationCreationInfo
		);

		static void destroyVmaImage(
			const VmaAllocator &allocator,
			const Image &image
		);

		static void destroyImage(
			const vk::Device &device,
			const VmaAllocator &allocator,
			const Image &image
		);

		static vk::Sampler createSampler(
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
		);

		static void destroySampler(const vk::Device &device, const vk::Sampler &sampler);

		static vk::SurfaceFormatKHR
		chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats);

		static vk::PresentModeKHR
		chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes);

		static vk::Extent2D
		chooseSwapExtent(vk::Extent2D extent, const vk::SurfaceCapabilitiesKHR &capabilities);

		static Swapchain createSwapchain(
			const vk::Device &swapchainImage,
			const vk::PhysicalDevice &physicalDevice,
			const vk::SurfaceKHR &surface,
			const vk::Extent2D &extent
		);

		static void destroySwapchain(const vk::Device &device, const Swapchain &swapchain);

		static vk::ShaderModule createShaderModules(const vk::Device &device, uint32_t codeSize, const uint32_t *pCode);

		static void destroyShaderModule(const vk::Device &device, const vk::ShaderModule &shaderModule);

		static vk::Instance createInstance(
			const std::vector<const char *> &requiredInstanceExtensions,
			const std::vector<const char *> &requiredLayers
		);

		static bool hasStencilComponent(vk::Format format) {
			return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
		}

		static void transitionLayout(
			const vk::CommandBuffer &commandBuffer,
			const vk::Image &image,
			vk::Format format,
			vk::ImageLayout oldLayout,
			vk::ImageLayout newLayout,
			uint32_t mipLevels
		);

		static void generatedMipmaps(
			const vk::CommandBuffer &commandBuffer,
			const vk::Image &image,
			int32_t width,
			int32_t height,
			uint32_t mipLevels
		);
	};
}