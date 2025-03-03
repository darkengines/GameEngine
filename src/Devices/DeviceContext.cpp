#include "DeviceContext.hpp"

#include <fmt/format.h>

#include <stdexcept>

#include "../Common/Common.hpp"
#include "Device.hpp"
#include "VulkanInstanceConfiguration.hpp"

namespace drk::Devices
{

  DeviceContext::DeviceContext(const std::vector<const char*>& requiredInstanceExtensions,
      const std::vector<const char*>& requiredDeviceExtensions,
      const std::vector<const char*>& requiredValidationLayers,
      const std::function<vk::SurfaceKHR(const vk::Instance& instance)>& surfaceProvider,
      bool enableValidationLayer)
  {
    Instance = drk::Devices::Device::createInstance(
        requiredInstanceExtensions, enableValidationLayer ? requiredValidationLayers : std::vector<const char*>{});
    Surface = surfaceProvider(Instance);
    PhysicalDevice = drk::Devices::Device::pickPhysicalDevice(Instance, Surface, requiredDeviceExtensions);
    auto logicalDevice = drk::Devices::Device::createLogicalDevice(
        PhysicalDevice, Surface, requiredDeviceExtensions, enableValidationLayer, requiredValidationLayers);
    device = logicalDevice.device, GraphicQueue = logicalDevice.graphicQueue, PresentQueue = logicalDevice.presentQueue,
    ComputeQueue = logicalDevice.computeQueue,

    Allocator = Device::createAllocator(Instance, PhysicalDevice, device);

    MaxSampleCount = Device::getMaxSampleCount(PhysicalDevice);
    DepthFormat = Device::findDepthFormat(PhysicalDevice);

    vk::CommandPoolCreateInfo commandPoolCreateInfo = { .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer };
    CommandPool = device.createCommandPool(commandPoolCreateInfo);
  }
  DeviceContext::DeviceContext(const Configuration::Configuration& configuration, const Windows::Window& window, ...)
  {
    const auto vulkanInstanceConfiguration =
        configuration.jsonConfiguration[std::string(nameof::nameof_short_type<DeviceContext>()).c_str()]
            .get<VulkanInstanceConfiguration>();

    const auto glfwExtensions = Windows::Window::getVulkanInstanceExtension();
    std::vector<const char*> requiredInstanceExtensions;
    for (const auto& requiredInstanceExtension : vulkanInstanceConfiguration.RequiredInstanceExtensions)
    {
      requiredInstanceExtensions.push_back(requiredInstanceExtension.c_str());
    }
    for (const auto& requiredInstanceExtension : glfwExtensions)
    {
      requiredInstanceExtensions.push_back(requiredInstanceExtension);
    }

    std::vector<const char*> requiredValidationLayers;
    for (const auto& requiredValidationLayer : vulkanInstanceConfiguration.RequiredValidationLayers)
    {
      requiredValidationLayers.push_back(requiredValidationLayer.c_str());
    }

    std::vector<const char*> requiredDeviceExtensions;
    for (const auto& requiredDeviceExtension : vulkanInstanceConfiguration.RequiredDeviceExtensions)
    {
      requiredDeviceExtensions.push_back(requiredDeviceExtension.c_str());
    }

    Instance = drk::Devices::Device::createInstance(requiredInstanceExtensions, requiredValidationLayers);

    auto result = glfwCreateWindowSurface((VkInstance)Instance, window.GetWindow(), nullptr, (VkSurfaceKHR*)&Surface);
    if (result != VK_SUCCESS)
    {
      throw std::runtime_error("Failed to create surface.");
    }
    PhysicalDevice = drk::Devices::Device::pickPhysicalDevice(Instance, Surface, requiredDeviceExtensions);
    auto logicalDevice = drk::Devices::Device::createLogicalDevice(PhysicalDevice,
        Surface,
        requiredDeviceExtensions,
        vulkanInstanceConfiguration.EnableValidationLayers,
        requiredValidationLayers);
    device = logicalDevice.device;
    GraphicQueue = logicalDevice.graphicQueue;
    PresentQueue = logicalDevice.presentQueue;
    ComputeQueue = logicalDevice.computeQueue;

    auto properties = PhysicalDevice.getProperties();

    std::cout << fmt::format("Using Vulkan {0}", VK_API_VERSION_1_3) << std::endl;
    std::cout << fmt::format("Using physical device #{0} ({1})", properties.deviceID, properties.deviceName.data())
              << std::endl;

    Allocator = Device::createAllocator(Instance, PhysicalDevice, device);

    MaxSampleCount = Device::getMaxSampleCount(PhysicalDevice);
    DepthFormat = Device::findDepthFormat(PhysicalDevice);

    vk::CommandPoolCreateInfo commandPoolCreateInfo = { .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer };
    CommandPool = device.createCommandPool(commandPoolCreateInfo);
  }

  DeviceContext::~DeviceContext()
  {
    device.destroyCommandPool(CommandPool);
    vmaDestroyAllocator(Allocator);
    device.destroy();
    Instance.destroySurfaceKHR(Surface);
    Instance.destroy();
  }

  Buffer DeviceContext::CreateBuffer(vk::MemoryPropertyFlags properties,
      vk::BufferUsageFlags usage,
      const VmaAllocationCreateInfo& allocationCreationInfo,
      vk::DeviceSize size,
      const char* name) const
  {
    return Device::createBuffer(Allocator, properties, usage, allocationCreationInfo, size, name);
  }

  void DeviceContext::DestroyBuffer(const Buffer& buffer) const
  {
    Device::destroyBuffer(Allocator, buffer);
  }

  Image DeviceContext::createImage(const vk::ImageCreateInfo& imageCreationInfo, vk::MemoryPropertyFlags properties) const
  {
    return Device::createImage(Allocator, imageCreationInfo, properties);
  }

  void DeviceContext::DestroyImage(const Image& image) const
  {
    Device::destroyImage(device, Allocator, image);
  }

  Texture DeviceContext::createTexture(const vk::ImageCreateInfo& imageCreateInfo,
      const vk::ImageViewCreateInfo& imageViewCreateInfo,
      vk::MemoryPropertyFlagBits memoryProperties) const
  {
    const auto image = createImage(imageCreateInfo, vk::MemoryPropertyFlagBits::eDeviceLocal);

    const auto imageView = device.createImageView(imageViewCreateInfo);

    return { .image = image,
      .imageView = imageView,
      .imageCreateInfo = imageCreateInfo,
      .imageViewCreateInfo = imageViewCreateInfo,
      .memoryProperties = memoryProperties };
  }

  void DeviceContext::destroyTexture(const Texture& texture) const
  {
    device.destroyImageView(texture.imageView);
    Device::destroyImage(device, Allocator, texture.image);
  }
  vk::ShaderModule DeviceContext::CreateShaderModule(const std::string& shaderPath) const
  {
    auto code = Common::ReadFile(shaderPath);
    return Devices::Device::createShaderModules(
        device, static_cast<uint32_t>(code.size()), reinterpret_cast<uint32_t*>(code.data()));
  }
}  // namespace drk::Devices
