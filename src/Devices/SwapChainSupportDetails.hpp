#pragma once
#include <vulkan/vulkan.h>
#include <vector>

namespace drk::Devices {
	struct SwapChainSupportDetails {
		vk::SurfaceCapabilitiesKHR capabilities;
		std::vector<vk::SurfaceFormatKHR> formats;
		std::vector<vk::PresentModeKHR> presentModes;
	};
}
