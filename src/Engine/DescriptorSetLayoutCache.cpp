#include "DescriptorSetLayoutCache.hpp"

namespace drk::Engine {

	DescriptorSetLayoutCache::DescriptorSetLayoutCache(const Devices::DeviceContext& deviceContext) : deviceContext(
		deviceContext
	) {}

	DescriptorSetLayoutCache::~DescriptorSetLayoutCache() {
		for (auto& layout : Layouts) {
			deviceContext.device.destroyDescriptorSetLayout(layout.second);
		}
	}

	vk::DescriptorSetLayout
	DescriptorSetLayoutCache::get(const vk::DescriptorSetLayoutCreateInfo& descriptorSetLayoutCreateInfo) {
		auto entry = Layouts.find(descriptorSetLayoutCreateInfo);
		if (entry == Layouts.end()) {
			auto layout = deviceContext.device.createDescriptorSetLayout(descriptorSetLayoutCreateInfo);
			Layouts[descriptorSetLayoutCreateInfo] = layout;
			return layout;
		}
		return entry->second;
	}
}