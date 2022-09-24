#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS
#include "../Devices/Context.hpp"
#include "Image.hpp"
#include <vulkan/vulkan.hpp>

namespace drk::Textures {
	struct Texture {
		VkImage image;
		VmaAllocation allocation;
		VmaAllocationInfo allocationInfo;
	};
}