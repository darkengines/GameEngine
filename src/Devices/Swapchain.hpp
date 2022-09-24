#pragma once

#include <vulkan/vulkan.hpp>

namespace drk::Devices {
	struct Swapchain {
		vk::SwapchainKHR swapchain;
		vk::Format imageFormat;
		vk::Extent2D extent;
		std::vector<vk::Image> images;
		std::vector<vk::ImageView> imageViews;
	};
}
