#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS
#include "../Textures/Image.hpp"
#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>

namespace drk::Devices {
	struct Texture {
		vk::Image image;
		VmaAllocation allocation;
		VmaAllocationInfo allocationInfo;
	};
}