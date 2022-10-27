#include "DeviceContext.hpp"
#include "Device.hpp"
#include "../Common/Common.hpp"
#include "VulkanInstanceConfiguration.hpp"

namespace drk::Devices {

	DeviceContext::DeviceContext(
		const std::vector<const char*>& requiredInstanceExtensions,
		const std::vector<const char*>& requiredDeviceExtensions,
		const std::vector<const char*>& requiredValidationLayers,
		const std::function<vk::SurfaceKHR(const vk::Instance& instance)>& surfaceProvider,
		bool enableValidationLayer
	) {
		Instance = drk::Devices::Device::createInstance(
			requiredInstanceExtensions,
			enableValidationLayer ? requiredValidationLayers
								  : std::vector<const char*>{}
		);
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
	DeviceContext::DeviceContext(
		const Configuration::Configuration& configuration,
		const Windows::Window& window,
		...
	) {
		const auto vulkanInstanceConfiguration = configuration.jsonConfiguration[std::string(nameof::nameof_short_type<DeviceContext>()).c_str()].get<VulkanInstanceConfiguration>();

		const auto glfwExtensions = Windows::Window::getVulkanInstanceExtension();
		std::vector<const char*> requiredInstanceExtensions;
		for (const auto& requiredInstanceExtension: vulkanInstanceConfiguration.RequiredInstanceExtensions) {
			requiredInstanceExtensions.push_back(requiredInstanceExtension.c_str());
		}
		for (const auto& requiredInstanceExtension: glfwExtensions) {
			requiredInstanceExtensions.push_back(requiredInstanceExtension);
		}

		std::vector<const char*> requiredValidationLayers;
		for (const auto& requiredValidationLayer: vulkanInstanceConfiguration.RequiredValidationLayers) {
			requiredValidationLayers.push_back(requiredValidationLayer.c_str());
		}

		std::vector<const char*> requiredDeviceExtensions;
		for (const auto& requiredDeviceExtension: vulkanInstanceConfiguration.RequiredDeviceExtensions) {
			requiredDeviceExtensions.push_back(requiredDeviceExtension.c_str());
		}

		Instance = drk::Devices::Device::createInstance(
			requiredInstanceExtensions,
			requiredValidationLayers
		);

		vk::SurfaceKHR surface;
		auto result = glfwCreateWindowSurface(
			(VkInstance) Instance,
			window.GetWindow(),
			nullptr,
			(VkSurfaceKHR*) &Surface
		);
		if (result != VK_SUCCESS) {
			throw "Failed to create surface.";
		}
		PhysicalDevice = drk::Devices::Device::pickPhysicalDevice(
			Instance,
			Surface,
			requiredDeviceExtensions
		);
		auto device = drk::Devices::Device::createLogicalDevice(
			PhysicalDevice,
			Surface,
			requiredDeviceExtensions,
			vulkanInstanceConfiguration.EnableValidationLayers,
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
		const VmaAllocationCreateInfo& allocationCreationInfo,
		vk::DeviceSize size
	) const {
		return Device::createBuffer(Allocator, properties, usage, allocationCreationInfo, size);
	}

	void DeviceContext::DestroyBuffer(const Buffer& buffer) const {
		Device::destroyBuffer(Allocator, buffer);
	}

	Image
	DeviceContext::CreateImage(
		const vk::ImageCreateInfo& imageCreationInfo,
		vk::MemoryPropertyFlags properties
	) const {
		return Device::createImage(Allocator, imageCreationInfo, properties);
	}

	void DeviceContext::DestroyImage(const Image& image) const {
		Device::destroyImage(Device, Allocator, image);
	}

	void DeviceContext::DestroyTexture(const Texture& texture) const {
		Device.destroyImageView(texture.imageView);
		Device::destroyImage(Device, Allocator, texture.image);
	}
	vk::ShaderModule DeviceContext::CreateShaderModule(const std::string& shaderPath) const {
		auto code = Common::ReadFile(shaderPath);
		return Devices::Device::createShaderModules(
			Device,
			static_cast<uint32_t>(code.size()),
			reinterpret_cast<uint32_t*>(code.data()));
	}
}