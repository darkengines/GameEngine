//
// Created by root on 27/09/2022.
//

#include <numeric>
#include "DescriptorSetLayoutCreateInfoEqualityComparer.hpp"

size_t
drk::Graphics::DescriptorSetLayoutCreateInfoEqualityComparer::operator()(const vk::DescriptorSetLayoutCreateInfo &descriptorSetLayoutCreateInfo) const {
	auto hasher = std::hash<size_t>();
	size_t hash = hasher(descriptorSetLayoutCreateInfo.bindingCount);
	hash = std::reduce(
		descriptorSetLayoutCreateInfo.pBindings,
		descriptorSetLayoutCreateInfo.pBindings + descriptorSetLayoutCreateInfo.bindingCount,
		hash,
		[&hasher](const size_t& hash, const vk::DescriptorSetLayoutBinding& binding) {
			size_t bindingHash =
				binding.binding | binding.descriptorCount << 8 | (uint32_t) binding.descriptorType << 16 |
				(uint32_t) binding.stageFlags << 24;
			return hash ^ hasher(bindingHash);
		}
	);
	return hash;
}

bool drk::Graphics::DescriptorSetLayoutCreateInfoEqualityComparer::operator()(
	const vk::DescriptorSetLayoutCreateInfo &leftDescriptorSetLayoutCreateInfo,
	const vk::DescriptorSetLayoutCreateInfo &rightDescriptorSetLayoutCreateInfo
) const {
	auto equals =
		leftDescriptorSetLayoutCreateInfo.bindingCount == rightDescriptorSetLayoutCreateInfo.bindingCount;
	for (auto bindingIndex = 0u;
		 equals && bindingIndex < leftDescriptorSetLayoutCreateInfo.bindingCount; bindingIndex++) {
		const auto &leftBinding = leftDescriptorSetLayoutCreateInfo.pBindings[bindingIndex];
		const auto &rightBinding = rightDescriptorSetLayoutCreateInfo.pBindings[bindingIndex];
		equals = leftBinding.binding == rightBinding.binding
				 && leftBinding.descriptorCount == rightBinding.descriptorCount
				 && leftBinding.descriptorType == rightBinding.descriptorType
				 && leftBinding.stageFlags == rightBinding.stageFlags;
	}
	return equals;
}