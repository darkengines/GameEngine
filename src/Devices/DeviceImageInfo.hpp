
#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
#include "ImageInfo.hpp"

namespace drk::Devices {
	struct DeviceImageInfo {
		ImageInfo imageInfo;
		vk::Image image;
		vk::ImageView imageView;
	};
}