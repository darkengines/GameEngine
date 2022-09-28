#pragma once

#include <unordered_map>
#include <vulkan/vulkan.hpp>
#include "DescriptorSetLayoutCreateInfoEqualityComparer.hpp"

namespace drk::Graphics {
	class DescriptorSetLayoutCache {
	public:
		DescriptorSetLayoutCache(const vk::Device &device) : Device(device) {}

		~DescriptorSetLayoutCache() {
			for (auto &layout : Layouts) {
				Device.destroyDescriptorSetLayout(layout.second);
			}
		}

		vk::DescriptorSetLayout get(const vk::DescriptorSetLayoutCreateInfo &descriptorSetLayoutCreateInfo) {
			auto entry = Layouts.find(descriptorSetLayoutCreateInfo);
			if (entry == Layouts.end()) {
				auto layout = Device.createDescriptorSetLayout(descriptorSetLayoutCreateInfo);
				Layouts[descriptorSetLayoutCreateInfo] = layout;
				return layout;
			}
			return entry->second;
		}

	protected:
		const vk::Device Device;
		std::unordered_map<vk::DescriptorSetLayoutCreateInfo, vk::DescriptorSetLayout, DescriptorSetLayoutCreateInfoEqualityComparer, DescriptorSetLayoutCreateInfoEqualityComparer> Layouts;
	};
}