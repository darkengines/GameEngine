#pragma once

#include "../Textures/ImageInfo.hpp"
#include "Image.hpp"
#include "../Common/ComponentIndex.hpp"
#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>

namespace drk::Devices {
	struct Texture {
		Image image;
		vk::ImageView imageView;
		Common::Index index;
		vk::ImageCreateInfo imageCreateInfo;
		vk::ImageViewCreateInfo imageViewCreateInfo;
		vk::MemoryPropertyFlagBits memoryProperties;
	};
}