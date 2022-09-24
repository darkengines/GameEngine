#pragma once

#include <vulkan/vulkan.hpp>

namespace drk::Devices {
	struct VulkanLogicalDeviceInfo {
		vk::Device device;
		vk::Queue graphicQueue;
		vk::Queue presentQueue;
	};
}
