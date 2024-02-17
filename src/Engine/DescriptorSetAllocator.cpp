#include "DescriptorSetAllocator.hpp"
#include "../Devices/DeviceContext.hpp"

namespace drk::Engine {

	DescriptorSetAllocator::DescriptorSetAllocator(const Devices::DeviceContext& deviceContext) : deviceContext(
		deviceContext
	) {}

	DescriptorSetAllocator::~DescriptorSetAllocator() {
		for (const auto pool: pools) deviceContext.device.destroyDescriptorPool(pool);
	}

	std::vector<vk::DescriptorSet>
	DescriptorSetAllocator::allocateDescriptorSets(const std::vector<vk::DescriptorSetLayout>& descriptorSetLayouts) {
		vk::DescriptorSetAllocateInfo descriptorSetAllocationInfo = {
			.descriptorPool = getCurrentPool(),
			.descriptorSetCount = static_cast<uint32_t>(descriptorSetLayouts.size()),
			.pSetLayouts = descriptorSetLayouts.data()
		};
		std::vector<vk::DescriptorSet> descriptorSets;
		try {
			descriptorSets = deviceContext.device.allocateDescriptorSets(descriptorSetAllocationInfo);
		} catch (const vk::OutOfPoolMemoryError& error) {
			currentPool = std::nullopt;
			descriptorSetAllocationInfo.descriptorPool = getCurrentPool();
			descriptorSets = deviceContext.device.allocateDescriptorSets(descriptorSetAllocationInfo);
		}

		return descriptorSets;
	}

	vk::DescriptorPool DescriptorSetAllocator::getCurrentPool() {
		vk::DescriptorPool pool;
		if (!currentPool.has_value()) {
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
			pool = deviceContext.device.createDescriptorPool(descriptorPoolCreationInfo);
			currentPool = pool;
			pools.push_back(pool);
		} else {
			pool = currentPool.value();
		}

		return pool;
	}
	DescriptorSetAllocator::DescriptorSetAllocator(DescriptorSetAllocator&& descriptorSetAllocator) noexcept
		: deviceContext(descriptorSetAllocator.deviceContext), pools(std::move(descriptorSetAllocator.pools)),
		  currentPool(descriptorSetAllocator.currentPool) {}
}