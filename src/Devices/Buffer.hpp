#pragma once
#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>

namespace drk::Devices {
	struct Buffer {
		vk::Buffer buffer;
		VmaAllocation allocation;
		VmaAllocationInfo allocationInfo;
	};
}