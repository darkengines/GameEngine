
#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
#include "DescriptorSetLayoutCache.hpp"

namespace drk::Engine {

	struct DescriptorSetLayouts {
		DescriptorSetLayoutCache& descriptorSetLayoutCache;
		const vk::DescriptorSetLayout globalDescriptorSetLayout;
		const vk::DescriptorSetLayout textureDescriptorSetLayout;
		const vk::DescriptorSetLayout storeDescriptorSetLayout;

		DescriptorSetLayouts(DescriptorSetLayoutCache& descriptorSetLayoutCache);

		static vk::DescriptorSetLayout
		CreateEmptyDescriptorSetLayout(Engine::DescriptorSetLayoutCache& descriptorSetLayoutCache);

		static vk::DescriptorSetLayout
		CreateGlobalDescriptorSetLayout(Engine::DescriptorSetLayoutCache& descriptorSetLayoutCache);

		static vk::DescriptorSetLayout
		CreateStoreDescriptorSetLayout(Engine::DescriptorSetLayoutCache& descriptorSetLayoutCache);

		static vk::DescriptorSetLayout
		CreateTextureDescriptorSetLayout(Engine::DescriptorSetLayoutCache& descriptorSetLayoutCache);
	};
}