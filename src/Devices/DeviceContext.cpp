#include "DeviceContext.hpp"
#include "Device.hpp"

namespace drk::Devices {

	DeviceContext::DeviceContext(
		const std::vector<const char *> &requiredInstanceExtensions,
		const std::vector<const char *> &requiredDeviceExtensions,
		const std::vector<const char *> &requiredValidationLayers,
		const std::function<vk::SurfaceKHR(const vk::Instance &)> &surfaceProvider,
		bool enableValidationLayer
	) {
		Instance = drk::Devices::Device::createInstance(requiredInstanceExtensions, requiredValidationLayers);
		Surface = surfaceProvider(Instance);
		PhysicalDevice = drk::Devices::Device::pickPhysicalDevice(
			Instance,
			Surface,
			requiredDeviceExtensions
		);
		auto device = drk::Devices::Device::createLogicalDevice(
			PhysicalDevice,
			Surface,
			requiredDeviceExtensions,
			enableValidationLayer,
			requiredValidationLayers
		);
		Device = device.device,
		GraphicQueue = device.graphicQueue,
		PresentQueue = device.presentQueue,
		ComputeQueue = device.computeQueue,

		Allocator = Device::createAllocator(Instance, PhysicalDevice, Device);

		MaxSampleCount = Device::getMaxSampleCount(PhysicalDevice);
		DepthFormat = Device::findDepthFormat(PhysicalDevice);

		vk::CommandPoolCreateInfo commandPoolCreateInfo = {
			.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer
		};
		CommandPool = Device.createCommandPool(commandPoolCreateInfo);
	}

	DeviceContext::~DeviceContext() {
		Device.destroyCommandPool(CommandPool);
		vmaDestroyAllocator(Allocator);
		Device.destroy();
		Instance.destroySurfaceKHR(Surface);
		Instance.destroy();
	}

	Buffer
	DeviceContext::CreateBuffer(
		vk::MemoryPropertyFlags properties,
		vk::BufferUsageFlags usage,
		vk::DeviceSize size
	) const {
		return Device::createBuffer(Allocator, properties, usage, size);
	}

	void DeviceContext::DestroyBuffer(const Buffer &buffer) const {
		Device::destroyBuffer(Allocator, buffer);
	}

	Image
	DeviceContext::CreateImage(
		const vk::ImageCreateInfo &imageCreationInfo,
		vk::MemoryPropertyFlags properties
	) const {
		return Device::createImage(Allocator, imageCreationInfo, properties);
	}

	void DeviceContext::DestroyImage(const Image &image) const {
		Device::destroyImage(Device, Allocator, image);
	}

	void DeviceContext::DestroyTexture(const Texture &texture) const {
		Device.destroyImageView(texture.imageView);
		Device::destroyImage(Device, Allocator, texture.image);
	}
}