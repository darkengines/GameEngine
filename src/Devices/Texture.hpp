#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS

#include "../Textures/ImageInfo.hpp"
#include "Image.hpp"
#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>

namespace drk::Devices {
	struct Texture {
		Image image;
		vk::ImageView imageView;
	};
}