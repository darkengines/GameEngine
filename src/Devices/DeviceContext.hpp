#pragma once

#include "Buffer.hpp"
#include "Texture.hpp"
#include "../Configuration/Extensions.hpp"
#include "../Windows/Window.hpp"
#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>

namespace drk::Devices {
	class DeviceContext {
	public:
		explicit DeviceContext(
			const std::vector<const char*>& requiredInstanceExtensions,
			const std::vector<const char*>& requiredDeviceExtensions,
			const std::vector<const char*>& requiredValidationLayers,
			const std::function<vk::SurfaceKHR(const vk::Instance& instance)>& surfaceProvider,
			bool enableValidationLayer
		);

		DeviceContext(
			const Configuration::Configuration& configuration,
			const Windows::Window& window,
			...
		);

		~DeviceContext();

		Buffer CreateBuffer(
			vk::MemoryPropertyFlags properties,
			vk::BufferUsageFlags usage,
			const VmaAllocationCreateInfo& allocationCreationInfo,
			vk::DeviceSize size
		) const;
		void DestroyBuffer(const Buffer& buffer) const;

		Image createImage(const vk::ImageCreateInfo& imageCreationInfo, vk::MemoryPropertyFlags properties) const;
		void DestroyImage(const Image& image) const;
		void destroyTexture(const Texture& texture) const;

		vk::ShaderModule CreateShaderModule(const std::string& shaderPath) const;

		vk::Instance Instance;
		vk::SurfaceKHR Surface;
		vk::PhysicalDevice PhysicalDevice;
		vk::Device device;
		vk::Queue GraphicQueue;
		vk::Queue PresentQueue;
		vk::Queue ComputeQueue;
		vk::CommandPool CommandPool;
		VmaAllocator Allocator;
		vk::SampleCountFlagBits MaxSampleCount;
		vk::Format DepthFormat;
		Texture createTexture(
			const vk::ImageCreateInfo& imageCreateInfo,
			const vk::ImageViewCreateInfo& imageViewCreateInfo,
			vk::MemoryPropertyFlagBits memoryProperties
		) const;
	};
}