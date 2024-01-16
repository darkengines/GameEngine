
#pragma once

#include <vulkan/vulkan.hpp>

namespace drk::Devices {
	struct ImageInfo {
		vk::Extent3D extent;
		vk::Format format;
	};
}