#pragma once
#define VULKAN_HPP_NO_CONSTRUCTORS

#include <vulkan/vulkan.hpp>
#include "../Devices/DeviceContext.hpp"
#include "../Devices/Device.hpp"
#include "StoreBuffer.hpp"
#include <vector>

namespace drk::Graphics {
	class StoreBufferAllocator {
	protected:
		const Devices::DeviceContext *DeviceContext;
		std::vector<vk::Buffer> Buffers;
		vk::DescriptorSet DescriptorSet;
	public:
		StoreBufferAllocator(const Devices::DeviceContext *deviceContext, const vk::DescriptorSet& descriptorSet) : DeviceContext(
			deviceContext
		), DescriptorSet(descriptorSet) {

		}

		~StoreBufferAllocator();

		template<typename T>
		StoreBuffer <T> Allocate(uint32_t itemCount) {
			auto itemByteLength = sizeof(T);
			auto byteLength = itemCount * itemByteLength;
			auto bufferIndex = static_cast<uint32_t>(Buffers.size());
			VmaAllocationCreateInfo allocationCreationInfo = {
				.flags = VmaAllocationCreateFlagBits::VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT,
				.usage = VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO,
				.requiredFlags = (VkMemoryPropertyFlags) (vk::MemoryPropertyFlagBits::eHostVisible |
														  vk::MemoryPropertyFlagBits::eHostCoherent),
			};
			auto storageBuffer = Devices::Device::createBuffer(
				DeviceContext->Allocator,
				vk::MemoryPropertyFlagBits::eHostVisible,
				vk::BufferUsageFlagBits::eStorageBuffer,
				allocationCreationInfo,
				byteLength
			);

			void *mappedMemory = nullptr;
			Devices::Device::mapBuffer(DeviceContext->Allocator, storageBuffer, (void **) &mappedMemory);

			vk::DescriptorBufferInfo descriptorBufferInfo = {
				.buffer = storageBuffer.buffer,
				.offset = 0,
				.range = VK_WHOLE_SIZE
			};

			vk::WriteDescriptorSet writeDescriptorSet = {
				.dstSet = DescriptorSet,
				.dstBinding = 0,
				.dstArrayElement = bufferIndex,
				.descriptorCount = 1,
				.descriptorType = vk::DescriptorType::eStorageBuffer,
				.pBufferInfo = &descriptorBufferInfo
			};

			DeviceContext->Device.updateDescriptorSets(1, &writeDescriptorSet, 0, nullptr);

			Devices::BufferView bufferView = {
				.buffer = storageBuffer,
				.byteOffset = 0,
				.byteLength = byteLength
			};

			StoreBuffer<T> store(itemCount, reinterpret_cast<T *>(mappedMemory));

			return store;
		}
	};
}