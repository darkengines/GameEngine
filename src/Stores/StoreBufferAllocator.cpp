#pragma once
#include "StoreBufferAllocator.hpp"

namespace drk::Stores {
	StoreBufferAllocator::StoreBufferAllocator(
		const Devices::DeviceContext& deviceContext,
		const vk::DescriptorSet& descriptorSet
	)
		: deviceContext(
		deviceContext
	), DescriptorSet(descriptorSet) {

	}

	StoreBufferAllocator::~StoreBufferAllocator() {
		for (const auto buffer: Buffers) {
			Devices::Device::unmapBuffer(deviceContext.Allocator, buffer);
			deviceContext.DestroyBuffer(buffer);
		}
	}
	StoreBufferAllocator::StoreBufferAllocator(StoreBufferAllocator&& storeBufferAllocator) noexcept: deviceContext(
		storeBufferAllocator.deviceContext
	), DescriptorSet(storeBufferAllocator.DescriptorSet), Buffers(std::move(storeBufferAllocator.Buffers)) {
		storeBufferAllocator.Buffers.clear();
	}
	std::unique_ptr<GenericStoreBuffer> StoreBufferAllocator::allocate(size_t itemByteLength, uint32_t itemCount) {
		auto byteLength = itemCount * itemByteLength;
		auto bufferIndex = static_cast<uint32_t>(Buffers.size());
		VmaAllocationCreateInfo allocationCreationInfo = {
			.flags = VmaAllocationCreateFlagBits::VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT,
			.usage = VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO,
			.requiredFlags = (VkMemoryPropertyFlags) (vk::MemoryPropertyFlagBits::eHostVisible |
													  vk::MemoryPropertyFlagBits::eHostCoherent),
		};
		auto storageBuffer = Devices::Device::createBuffer(
			deviceContext.Allocator,
			vk::MemoryPropertyFlagBits::eHostVisible,
			vk::BufferUsageFlagBits::eStorageBuffer,
			allocationCreationInfo,
			byteLength,
			fmt::format("{0}.StorageBuffer", typeid(StoreBufferAllocator).name()).c_str()
		);
		Buffers.push_back(storageBuffer);

		void* mappedMemory = nullptr;
		Devices::Device::mapBuffer(deviceContext.Allocator, storageBuffer, &mappedMemory);

		vk::DescriptorBufferInfo descriptorBufferInfo{};
		descriptorBufferInfo.buffer = storageBuffer.buffer;
		descriptorBufferInfo.offset = 0u;
		descriptorBufferInfo.range = VK_WHOLE_SIZE;


		vk::WriteDescriptorSet writeDescriptorSet = {};
		writeDescriptorSet.dstSet = DescriptorSet;
		writeDescriptorSet.dstBinding = 0;
		writeDescriptorSet.dstArrayElement = bufferIndex;
		writeDescriptorSet.descriptorCount = 1;
		writeDescriptorSet.descriptorType = vk::DescriptorType::eStorageBuffer;
		writeDescriptorSet.pBufferInfo = &descriptorBufferInfo;

		deviceContext.device.updateDescriptorSets(1, &writeDescriptorSet, 0, nullptr);

		auto store = std::make_unique<GenericStoreBuffer>(itemCount, bufferIndex, mappedMemory);

		return store;
	}
}