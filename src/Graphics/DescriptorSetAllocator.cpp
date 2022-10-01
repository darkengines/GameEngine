#include "DescriptorSetAllocator.hpp"

namespace drk::Graphics {

	DescriptorSetAllocator::DescriptorSetAllocator(const vk::Device &device) : Device(device) {}

	DescriptorSetAllocator::~DescriptorSetAllocator() {
		for (const auto pool : Pools) Device.destroyDescriptorPool(pool);
	}

	std::vector<vk::DescriptorSet>
	DescriptorSetAllocator::AllocateDescriptorSet(const std::vector<vk::DescriptorSetLayout> &descriptorSetLayouts) {
		vk::DescriptorSetAllocateInfo descriptorSetAllocationInfo = {
			.descriptorPool = GetCurrentPool(),
			.descriptorSetCount = static_cast<uint32_t>(descriptorSetLayouts.size()),
			.pSetLayouts = descriptorSetLayouts.data()
		};
		std::vector<vk::DescriptorSet> descriptorSets;
		try {
			descriptorSets = Device.allocateDescriptorSets(descriptorSetAllocationInfo);
		} catch (const vk::OutOfPoolMemoryError &error) {
			CurrentPool = std::nullopt;
			descriptorSetAllocationInfo.descriptorPool = GetCurrentPool();
			descriptorSets = Device.allocateDescriptorSets(descriptorSetAllocationInfo);
		}

		return descriptorSets;
	}

	vk::DescriptorPool DescriptorSetAllocator::GetCurrentPool() {
		vk::DescriptorPool pool;
		if (!CurrentPool.has_value()) {
			//TODO: make descriptorCount configurable
			std::vector<vk::DescriptorPoolSize> poolSizes{
				{vk::DescriptorType::eStorageBuffer,        2048},
				{vk::DescriptorType::eCombinedImageSampler, 2048},
				{vk::DescriptorType::eUniformBuffer,        2048},
			};
			vk::DescriptorPoolCreateInfo descriptorPoolCreationInfo = {
				.flags = vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind,
				.maxSets = 2048,
				.poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
				.pPoolSizes = poolSizes.data()
			};
			pool = Device.createDescriptorPool(descriptorPoolCreationInfo);
			CurrentPool = pool;
			Pools.push_back(pool);
		} else {
			pool = CurrentPool.value();
		}

		return pool;
	}
}