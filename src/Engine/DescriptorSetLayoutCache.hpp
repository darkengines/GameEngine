#pragma once
#include <unordered_map>
#include <vulkan/vulkan.hpp>
#include "DescriptorSetLayoutCreateInfoEqualityComparer.hpp"
#include "../Devices/DeviceContext.hpp"

namespace drk::Engine {
	class DescriptorSetLayoutCache {
	public:
		DescriptorSetLayoutCache(const Devices::DeviceContext& deviceContext);
		~DescriptorSetLayoutCache();
		vk::DescriptorSetLayout get(const vk::DescriptorSetLayoutCreateInfo &descriptorSetLayoutCreateInfo);
	protected:
		const Devices::DeviceContext& deviceContext;
		std::unordered_map<vk::DescriptorSetLayoutCreateInfo, vk::DescriptorSetLayout, DescriptorSetLayoutCreateInfoEqualityComparer, DescriptorSetLayoutCreateInfoEqualityComparer> Layouts;
	};
}