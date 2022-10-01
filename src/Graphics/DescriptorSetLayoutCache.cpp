#include "DescriptorSetLayoutCache.hpp"

namespace drk::Graphics {

	DescriptorSetLayoutCache::DescriptorSetLayoutCache(const vk::Device &device) : Device(device) {}

	DescriptorSetLayoutCache::~DescriptorSetLayoutCache() {
		for (auto &layout : Layouts) {
			Device.destroyDescriptorSetLayout(layout.second);
		}
	}

	vk::DescriptorSetLayout
	DescriptorSetLayoutCache::get(const vk::DescriptorSetLayoutCreateInfo &descriptorSetLayoutCreateInfo) {
		auto entry = Layouts.find(descriptorSetLayoutCreateInfo);
		if (entry == Layouts.end()) {
			auto layout = Device.createDescriptorSetLayout(descriptorSetLayoutCreateInfo);
			Layouts[descriptorSetLayoutCreateInfo] = layout;
			return layout;
		}
		return entry->second;
	}
}