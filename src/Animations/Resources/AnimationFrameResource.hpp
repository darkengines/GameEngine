#pragma once
#include <vulkan/vulkan.hpp>

namespace drk::Animations::Resources {
	struct AnimationFrameResource {
		vk::DescriptorSet skinnedMeshDescriptorSet;
		uint32_t skinnedVertexBufferDescriptorSetArrayElementOffset;
	};
}