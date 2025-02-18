
#pragma once

#include <vulkan/vulkan.hpp>
#include <cstdint>

namespace drk::Renderers {
	class Renderer {
		virtual void render(uint32_t targetImageIndex, const vk::CommandBuffer& commandBuffer) = 0;
	};
}


