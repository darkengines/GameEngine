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
	}

	DeviceContext::~DeviceContext() {
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

	Texture
	DeviceContext::CreateTexture(
		const vk::ImageCreateInfo &imageCreationInfo,
		vk::MemoryPropertyFlags properties
	) const {
		return Device::createTexture(Allocator, imageCreationInfo, properties);
	}

	void DeviceContext::DestroyTexture(const Texture &texture) const {
		Device::destroyTexture(Allocator, texture);
	}
}