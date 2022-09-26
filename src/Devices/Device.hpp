#pragma once
#define VULKAN_HPP_NO_CONSTRUCTORS

#include <vulkan/vulkan.hpp>

#include "QueueFamilyIndices.hpp"
#include "SwapChainSupportDetails.hpp"
#include "VulkanLogicalDeviceInfo.hpp"
#include "BufferView.hpp"
#include "Texture.hpp"
#include "Swapchain.hpp"

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
			VmaAllocationCreateInfo *pAllocationCreationInfo,
			vk::DeviceSize size
		);

		static Buffer createBuffer(
			const VmaAllocator &allocator,
			vk::MemoryPropertyFlags properties,
			vk::BufferUsageFlags usage,
			vk::DeviceSize size
		);

		static void destroyVmaBuffer(const VmaAllocator &allocator, Buffer buffer);

		static void destroyBuffer(const VmaAllocator &allocator, const Buffer &buffer);

		static void mapBuffer(const VmaAllocator &allocator, const Buffer &buffer, void *memory);

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

		static vk::CommandBuffer beginSingleTimeCommands(const vk::Device &device, const vk::CommandPool &commandPool);

		static void endSingleTimeCommands(
			const vk::Device &device,
			const vk::Queue &queue,
			const vk::CommandPool &commandPool,
			const vk::CommandBuffer &commandBuffer
		);

		template<typename TBuffer>
		static std::vector<BufferView> uploadBuffers(
			const vk::Device &device,
			const vk::Queue &queue,
			const vk::CommandPool &commandPool,
			const VmaAllocator &allocator,
			std::vector<std::span<TBuffer>> buffers,
			vk::BufferUsageFlagBits memoryUsage,
			Buffer **deviceBuffer
		);

		static Texture createTexture(
			const VmaAllocator &allocator,
			const vk::ImageCreateInfo imageCreationInfo,
			vk::MemoryPropertyFlags properties
		);

		static Texture createVmaImage(
			const VmaAllocator &allocator,
			const vk::ImageCreateInfo &imageCreationInfo,
			const VmaAllocationCreateInfo &allocationCreationInfo
		);

		static void destroyVmaImage(
			const VmaAllocator &allocator,
			const Texture &texture
		);

		static void destroyTexture(
			const VmaAllocator &allocator,
			const Texture &texture
		);

		static vk::ImageView createImageView(
			const vk::Device &device,
			const Texture &texture,
			vk::ImageViewCreateFlags flags,
			vk::ImageViewType type,
			vk::Format format,
			vk::ImageAspectFlags aspect,
			const vk::ComponentMapping &components,
			const vk::ImageSubresourceRange &subresourceRange
		);

		static void destroyImageView(const vk::Device &device, const vk::ImageView &imageView);

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
			const vk::Device &device,
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
	};
}