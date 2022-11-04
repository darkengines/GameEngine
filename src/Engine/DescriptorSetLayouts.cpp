#include "DescriptorSetLayouts.hpp"

namespace drk::Engine {

	vk::DescriptorSetLayout
	DescriptorSetLayouts::CreateEmptyDescriptorSetLayout(Engine::DescriptorSetLayoutCache& descriptorSetLayoutCache) {
		vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
		return descriptorSetLayoutCache.get(descriptorSetLayoutCreateInfo);
	}
	vk::DescriptorSetLayout
	DescriptorSetLayouts::CreateGlobalDescriptorSetLayout(DescriptorSetLayoutCache& descriptorSetLayoutCache) {
		vk::DescriptorSetLayoutBinding binding = {
			.binding = 0,
			.descriptorType = vk::DescriptorType::eUniformBuffer,
			.descriptorCount = 1,
			.stageFlags = vk::ShaderStageFlagBits::eAll
		};
		vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
			.bindingCount = 1,
			.pBindings = &binding,
		};
		return descriptorSetLayoutCache.get(descriptorSetLayoutCreateInfo);
	}

	vk::DescriptorSetLayout
	DescriptorSetLayouts::CreateStoreDescriptorSetLayout(DescriptorSetLayoutCache& descriptorSetLayoutCache) {

		auto bindFlags =
			vk::DescriptorBindingFlagBitsEXT::ePartiallyBound | vk::DescriptorBindingFlagBitsEXT::eUpdateAfterBind;

		vk::DescriptorSetLayoutBindingFlagsCreateInfoEXT extendedInfo = {
			.bindingCount = 1u,
			.pBindingFlags = &bindFlags
		};

		vk::DescriptorSetLayoutBinding binding = {
			.binding = 0,
			.descriptorType = vk::DescriptorType::eStorageBuffer,
			.descriptorCount = 64,
			.stageFlags = vk::ShaderStageFlagBits::eAll
		};
		vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
			.pNext = &extendedInfo,
			.flags = vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool,
			.bindingCount = 1,
			.pBindings = &binding,
		};

		return descriptorSetLayoutCache.get(descriptorSetLayoutCreateInfo);
	}
	vk::DescriptorSetLayout
	DescriptorSetLayouts::CreateTextureDescriptorSetLayout(DescriptorSetLayoutCache& descriptorSetLayoutCache) {

		auto bindFlags =
			vk::DescriptorBindingFlagBitsEXT::ePartiallyBound | vk::DescriptorBindingFlagBitsEXT::eUpdateAfterBind;

		vk::DescriptorSetLayoutBindingFlagsCreateInfoEXT extendedInfo = {
			.bindingCount = 1u,
			.pBindingFlags = &bindFlags
		};

		vk::DescriptorSetLayoutBinding binding = {
			.binding = 0,
			.descriptorType = vk::DescriptorType::eCombinedImageSampler,
			//TODO: make descriptorCount configurable
			.descriptorCount = 2048,
			.stageFlags = vk::ShaderStageFlagBits::eFragment
		};
		vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
			.pNext = &extendedInfo,
			.flags = vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool,
			.bindingCount = 1,
			.pBindings = &binding,
		};

		return descriptorSetLayoutCache.get(descriptorSetLayoutCreateInfo);
	}
	DescriptorSetLayouts::DescriptorSetLayouts(DescriptorSetLayoutCache& descriptorSetLayoutCache) :
		descriptorSetLayoutCache(descriptorSetLayoutCache),
		globalDescriptorSetLayout(CreateGlobalDescriptorSetLayout(descriptorSetLayoutCache)),
		textureDescriptorSetLayout(CreateTextureDescriptorSetLayout(descriptorSetLayoutCache)),
		storeDescriptorSetLayout(CreateStoreDescriptorSetLayout(descriptorSetLayoutCache)) {
	}
}