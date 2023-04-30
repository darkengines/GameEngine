
#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

namespace drk::Devices {
	struct ImageInfo {
		vk::Extent2D extent;
		vk::Format format;
	};
}