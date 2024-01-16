#pragma once
#include <vulkan/vulkan.hpp>

namespace drk::Animations::Resources {
	class AnimationFrameResource {
	protected:
		std::vector<vk::Buffer> buffers;
	public:
		AnimationFrameResource();
	};
}