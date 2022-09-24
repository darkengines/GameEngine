#pragma once
#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>

namespace drk::Devices {
	struct Context {
		vk::Instance instance;
		vk::SurfaceKHR surface;
		vk::PhysicalDevice physicalDevice;
		vk::Device device;
		vk::Queue graphicQueue;
        vk::Queue presentQueue;
        VmaAllocator vmaAllocator;
	};
}