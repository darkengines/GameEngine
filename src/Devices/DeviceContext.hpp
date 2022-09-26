#pragma once

#include "Buffer.hpp"
#include "Texture.hpp"
#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>

namespace drk::Devices {
	class DeviceContext {
	public:
		DeviceContext(
			const std::vector<const char *> &requiredInstanceExtensions,
			const std::vector<const char *> &requiredDeviceExtensions,
			const std::vector<const char *> &requiredValidationLayers,
			const std::function<vk::SurfaceKHR(const vk::Instance &instance)> &surfaceProvider,
			bool enableValidationLayer
		);

		~DeviceContext();

		Buffer CreateBuffer(vk::MemoryPropertyFlags properties, vk::BufferUsageFlags usage, vk::DeviceSize size) const;

		void DestroyBuffer(const Buffer &buffer) const;

		Texture CreateTexture(const vk::ImageCreateInfo &imageCreationInfo, vk::MemoryPropertyFlags properties) const;

		void DestroyTexture(const Texture &texture) const;

		vk::Instance Instance;
		vk::SurfaceKHR Surface;
		vk::PhysicalDevice PhysicalDevice;
		vk::Device Device;
		vk::Queue GraphicQueue;
		vk::Queue PresentQueue;
		vk::Queue ComputeQueue;
		VmaAllocator Allocator;
		vk::SampleCountFlagBits MaxSampleCount;
		vk::Format DepthFormat;
	};
}