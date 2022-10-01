#pragma once

#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>

namespace drk::Devices {
	struct Image {
		vk::Image image;
		VmaAllocation allocation;
		VmaAllocationInfo allocationInfo;
	};
}