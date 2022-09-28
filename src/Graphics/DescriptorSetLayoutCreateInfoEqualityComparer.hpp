#pragma once

#include <vulkan/vulkan.hpp>
#include <numeric>

namespace drk::Graphics {
	class DescriptorSetLayoutCreateInfoEqualityComparer {
	public:
		bool operator()(
			const vk::DescriptorSetLayoutCreateInfo &leftDescriptorSetLayoutCreateInfo,
			const vk::DescriptorSetLayoutCreateInfo &rightDescriptorSetLayoutCreateInfo
		) const;

		size_t operator()(const vk::DescriptorSetLayoutCreateInfo &descriptorSetLayoutCreateInfo) const;
	};
}