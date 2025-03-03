#pragma once
#include <vector>
#include <vulkan/vulkan.hpp>

#include "../Devices/Buffer.hpp"
#include "../Devices/Device.hpp"
#include "../Devices/DeviceContext.hpp"
#include "StoreBuffer.hpp"

namespace drk::Stores {
class StoreBufferAllocator {
protected:
	const Devices::DeviceContext& deviceContext;
	std::vector<Devices::Buffer> Buffers;
	vk::DescriptorSet DescriptorSet;

public:
	StoreBufferAllocator(const Devices::DeviceContext& deviceContext, const vk::DescriptorSet& descriptorSet);
	StoreBufferAllocator(StoreBufferAllocator&& storeBufferAllocator) noexcept;
	~StoreBufferAllocator();

	std::unique_ptr<GenericStoreBuffer> allocate(size_t itemByteLength, uint32_t itemCount);

	template <typename T>
	std::unique_ptr<StoreBuffer<T>> allocate(uint32_t itemCount) {
		auto itemByteLength = sizeof(T);
		auto byteLength = itemCount * itemByteLength;
		auto bufferIndex = static_cast<uint32_t>(Buffers.size());
		VmaAllocationCreateInfo allocationCreationInfo = {
			.flags = VmaAllocationCreateFlagBits::VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT,
			.usage = VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO,
			.requiredFlags = (VkMemoryPropertyFlags)(vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent),
		};
		auto storageBuffer = Devices::Device::createBuffer(
			deviceContext.Allocator, vk::MemoryPropertyFlagBits::eHostVisible, vk::BufferUsageFlagBits::eStorageBuffer, allocationCreationInfo, byteLength,
			fmt::format("Store buffer #{0}", bufferIndex).c_str()
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

		auto store = std::make_unique<StoreBuffer<T>>(itemCount, bufferIndex, reinterpret_cast<T*>(mappedMemory));

		return store;
	}
};
}  // namespace drk::Stores