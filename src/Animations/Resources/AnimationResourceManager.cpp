#include "./AnimationResourceManager.hpp"

namespace drk::Animations::Resources {
AnimationResourceManager::AnimationResourceManager(
	const Configuration::Configuration& configuration,
	Devices::DeviceContext& deviceContext,
	Engine::DescriptorSetAllocator& descriptorSetAllocator,
	Engine::DescriptorSetLayoutCache& descriptorSetLayoutCache
)
	: ResourceManager(configuration),
	  deviceContext(deviceContext),
	  vertexBufferDescriptorSetArrayElementOffset(0),
	  skinnedVertexBufferDescriptorSetArrayElementOffset(0),
	  vertexWeightDescriptorSetArrayElementOffset(0),
	  skinnedVertexRangeDescriptorSetArrayElementOffset(0),
	  descriptorSetAllocator(descriptorSetAllocator),
	  descriptorSetLayoutCache(descriptorSetLayoutCache),
	  vertexBufferDescriptorSetLayout(createVertexBufferDescriptorSetLayout(this->descriptorSetLayoutCache)),
	  skinnedVertexBufferDescriptorSetLayout(createVertexBufferDescriptorSetLayout(this->descriptorSetLayoutCache)),
	  vertexWeightDescriptorSetLayout(createVertexWeightDescriptorSetLayout(this->descriptorSetLayoutCache)),
	  skinnedVertexRangeDescriptorSetLayout(createSkinnedVertexRangeDescriptorSetLayout(this->descriptorSetLayoutCache)),
	  vertexWeightDescriptorSet(createVertexWeightDescriptorSet(this->descriptorSetAllocator, this->vertexWeightDescriptorSetLayout)),
	  vertexBufferDescriptorSet(createVertexBufferDescriptorSet(this->descriptorSetAllocator, this->vertexBufferDescriptorSetLayout)),
	  skinnedVertexBufferDescriptorSet(createVertexBufferDescriptorSet(this->descriptorSetAllocator, this->vertexBufferDescriptorSetLayout)),
	  skinnedVertexRangeDescriptorSet(createSkinnedVertexRangeDescriptorSet(this->descriptorSetAllocator, this->skinnedVertexRangeDescriptorSetLayout)),
	  frameResources(createFrameResources(this->configuration, this->descriptorSetAllocator, this->vertexBufferDescriptorSetLayout)) {}

AnimationResourceManager::~AnimationResourceManager() {
	for (const auto& buffer : buffers) {
		deviceContext.DestroyBuffer(buffer);
	}
}

vk::DescriptorSet AnimationResourceManager::createVertexBufferDescriptorSet(
	Engine::DescriptorSetAllocator& descriptorSetAllocator,
	vk::DescriptorSetLayout& vertexBufferDescriptorSetLayout
) {
	std::vector<vk::DescriptorSetLayout> descriptorSetLayouts{vertexBufferDescriptorSetLayout};
	return descriptorSetAllocator.allocateDescriptorSets(descriptorSetLayouts)[0];
}

std::vector<AnimationFrameResource> AnimationResourceManager::createFrameResources(
	const drk::Resources::Configuration::ResourcesConfiguration& resourceConfiguration,
	Engine::DescriptorSetAllocator& descriptorSetAllocator,
	vk::DescriptorSetLayout& vertexBufferDescriptorSetLayout
) {
	std::vector<AnimationFrameResource> frameResources(resourceConfiguration.frameCount);
	for (auto frameIndex = 0u; frameIndex < resourceConfiguration.frameCount; frameIndex++) {
		auto skinnedMeshDescriptorSet = createVertexBufferDescriptorSet(descriptorSetAllocator, vertexBufferDescriptorSetLayout);
		frameResources[frameIndex] = {.skinnedMeshDescriptorSet = skinnedMeshDescriptorSet};
	}
	return frameResources;
}

vk::DescriptorSetLayout AnimationResourceManager::createVertexBufferDescriptorSetLayout(Engine::DescriptorSetLayoutCache& descriptorSetLayoutCache) {
	auto bindFlags = vk::DescriptorBindingFlagBitsEXT::ePartiallyBound | vk::DescriptorBindingFlagBitsEXT::eUpdateAfterBind;

	vk::DescriptorSetLayoutBindingFlagsCreateInfoEXT extendedInfo = {.bindingCount = 1u, .pBindingFlags = &bindFlags};

	vk::DescriptorSetLayoutBinding binding = {
		.binding = 0, .descriptorType = vk::DescriptorType::eStorageBuffer, .descriptorCount = 64, .stageFlags = vk::ShaderStageFlagBits::eCompute
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
	std::vector<vk::DescriptorSetLayout> descriptorSetLayouts{vertexWeightDescriptorSetLayout};
	return descriptorSetAllocator.allocateDescriptorSets(descriptorSetLayouts)[0];
}

vk::DescriptorSetLayout AnimationResourceManager::createVertexWeightDescriptorSetLayout(Engine::DescriptorSetLayoutCache& descriptorSetLayoutCache) {
	auto bindFlags = vk::DescriptorBindingFlagBitsEXT::ePartiallyBound | vk::DescriptorBindingFlagBitsEXT::eUpdateAfterBind;

	vk::DescriptorSetLayoutBindingFlagsCreateInfoEXT extendedInfo = {.bindingCount = 1u, .pBindingFlags = &bindFlags};

	vk::DescriptorSetLayoutBinding binding = {
		.binding = 0, .descriptorType = vk::DescriptorType::eStorageBuffer, .descriptorCount = 64, .stageFlags = vk::ShaderStageFlagBits::eCompute
	};
	vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
		.pNext = &extendedInfo,
		.flags = vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool,
		.bindingCount = 1,
		.pBindings = &binding,
	};

	return descriptorSetLayoutCache.get(descriptorSetLayoutCreateInfo);
}

vk::DescriptorSetLayout AnimationResourceManager::createSkinnedVertexRangeDescriptorSetLayout(Engine::DescriptorSetLayoutCache& descriptorSetLayoutCache) {
	auto bindFlags = vk::DescriptorBindingFlagBitsEXT::ePartiallyBound | vk::DescriptorBindingFlagBitsEXT::eUpdateAfterBind;

	vk::DescriptorSetLayoutBindingFlagsCreateInfoEXT extendedInfo = {.bindingCount = 1u, .pBindingFlags = &bindFlags};

	vk::DescriptorSetLayoutBinding binding = {
		.binding = 0, .descriptorType = vk::DescriptorType::eStorageBuffer, .descriptorCount = 64, .stageFlags = vk::ShaderStageFlagBits::eCompute
	};
	vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
		.pNext = &extendedInfo,
		.flags = vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool,
		.bindingCount = 1,
		.pBindings = &binding,
	};

	return descriptorSetLayoutCache.get(descriptorSetLayoutCreateInfo);
}
vk::DescriptorSet AnimationResourceManager::createSkinnedVertexRangeDescriptorSet(
	Engine::DescriptorSetAllocator& descriptorSetAllocator,
	vk::DescriptorSetLayout& skinnedVertexRangeDescriptorSetLayout
) {
	std::vector<vk::DescriptorSetLayout> descriptorSetLayouts{skinnedVertexRangeDescriptorSetLayout};
	return descriptorSetAllocator.allocateDescriptorSets(descriptorSetLayouts)[0];
}
}  // namespace drk::Animations::Resources