#include "./AnimationResourceManager.hpp"

namespace drk::Animations::Resources {
	AnimationResourceManager::AnimationResourceManager(
		const Configuration::Configuration& configuration,
		Devices::DeviceContext& deviceContext,
		Engine::DescriptorSetAllocator& descriptorSetAllocator,
		Engine::DescriptorSetLayoutCache& descriptorSetLayoutCache
	) : ResourceManager(configuration),
		deviceContext(deviceContext),
		descriptorSetAllocator(descriptorSetAllocator),
		descriptorSetLayoutCache(descriptorSetLayoutCache),
		skinnedMeshDescriptorSetLayout(createSkinnedMeshDescriptorSetLayout(this->descriptorSetLayoutCache)),
		skinnedMeshDescriptorSet(createSkinnedMeshDescriptorSet(this->descriptorSetAllocator, this->skinnedMeshDescriptorSetLayout)),
		vertexWeightDescriptorSetLayout(createVertexWeightDescriptorSetLayout(this->descriptorSetLayoutCache)),
		vertexWeightDescriptorSet(createVertexWeightDescriptorSet(this->descriptorSetAllocator, this->vertexWeightDescriptorSetLayout)),
		skinnedMeshDescriptorSetArrayElementOffset(0),
		vertexWeightDescriptorSetArrayElementOffset(0)
	{

	}

	vk::DescriptorSet AnimationResourceManager::createSkinnedMeshDescriptorSet(
		Engine::DescriptorSetAllocator& descriptorSetAllocator,
		vk::DescriptorSetLayout& skinnedMeshDescriptorSetLayout
	) {
		std::vector<vk::DescriptorSetLayout> descriptorSetLayouts{ skinnedMeshDescriptorSetLayout };
		return descriptorSetAllocator.allocateDescriptorSets(descriptorSetLayouts)[0];
	}

	vk::DescriptorSetLayout AnimationResourceManager::createSkinnedMeshDescriptorSetLayout(Engine::DescriptorSetLayoutCache& descriptorSetLayoutCache) {
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
			.stageFlags = vk::ShaderStageFlagBits::eCompute
		};
		vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
			.pNext = &extendedInfo,
			.flags = vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool,
			.bindingCount = 1,
			.pBindings = &binding,
		};

		return descriptorSetLayoutCache.get(descriptorSetLayoutCreateInfo);
	}

	vk::DescriptorSet AnimationResourceManager::createVertexWeightDescriptorSet(
		Engine::DescriptorSetAllocator& descriptorSetAllocator,
		vk::DescriptorSetLayout& vertexWeightDescriptorSetLayout
	) {
		std::vector<vk::DescriptorSetLayout> descriptorSetLayouts{ vertexWeightDescriptorSetLayout };
		return descriptorSetAllocator.allocateDescriptorSets(descriptorSetLayouts)[0];
	}

	vk::DescriptorSetLayout AnimationResourceManager::createVertexWeightDescriptorSetLayout(Engine::DescriptorSetLayoutCache& descriptorSetLayoutCache) {
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
			.stageFlags = vk::ShaderStageFlagBits::eCompute
		};
		vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
			.pNext = &extendedInfo,
			.flags = vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool,
			.bindingCount = 1,
			.pBindings = &binding,
		};

		return descriptorSetLayoutCache.get(descriptorSetLayoutCreateInfo);
	}
}