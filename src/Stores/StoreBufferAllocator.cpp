#pragma once
#include "StoreBufferAllocator.hpp"

namespace drk::Stores {
	StoreBufferAllocator::StoreBufferAllocator(
		const Devices::DeviceContext& deviceContext,
		const vk::DescriptorSet& descriptorSet
	)
		: DeviceContext(
		deviceContext
	), DescriptorSet(descriptorSet) {

	}

	StoreBufferAllocator::~StoreBufferAllocator() {
		for (const auto buffer: Buffers) {
			Devices::Device::unmapBuffer(DeviceContext.Allocator, buffer);
			DeviceContext.DestroyBuffer(buffer);
		}
	}
	StoreBufferAllocator::StoreBufferAllocator(StoreBufferAllocator&& storeBufferAllocator) : DeviceContext(
		storeBufferAllocator.DeviceContext
	), DescriptorSet(storeBufferAllocator.DescriptorSet), Buffers(std::move(Buffers)) {
		storeBufferAllocator.Buffers.clear();
	}
}