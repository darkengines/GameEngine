#pragma once

#include <unordered_map>
#include <vulkan/vulkan.hpp>
#include "DescriptorSetLayoutCreateInfoEqualityComparer.hpp"

namespace drk::Graphics {
	class DescriptorSetLayoutCache {
	public:
		DescriptorSetLayoutCache(const vk::Device &device);
		~DescriptorSetLayoutCache();
		vk::DescriptorSetLayout get(const vk::DescriptorSetLayoutCreateInfo &descriptorSetLayoutCreateInfo);
	protected:
		const vk::Device Device;
		std::unordered_map<vk::DescriptorSetLayoutCreateInfo, vk::DescriptorSetLayout, DescriptorSetLayoutCreateInfoEqualityComparer, DescriptorSetLayoutCreateInfoEqualityComparer> Layouts;
	};
}