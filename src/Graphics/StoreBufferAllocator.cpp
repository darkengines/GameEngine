#pragma once
#define VULKAN_HPP_NO_CONSTRUCTORS

#include <vulkan/vulkan.hpp>

#include "StoreBufferAllocator.hpp"

namespace drk::Graphics {
	StoreBufferAllocator::StoreBufferAllocator(
		const Devices::DeviceContext *deviceContext,
		const vk::DescriptorSet &descriptorSet
	)
		: DeviceContext(
		deviceContext
	), DescriptorSet(descriptorSet) {

	}

	StoreBufferAllocator::~StoreBufferAllocator() {
		for (const auto buffer: Buffers) {
			Devices::Device::unmapBuffer(DeviceContext->Allocator, buffer);
			DeviceContext->DestroyBuffer(buffer);
		}
	}
}