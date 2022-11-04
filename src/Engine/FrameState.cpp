#include "FrameState.hpp"
#include "Models/Global.hpp"
#include <memory>

namespace drk::Engine {

	FrameState::FrameState(
		const Devices::DeviceContext& deviceContext,
		const DescriptorSetLayouts& descriptorSetLayouts,
		DescriptorSetAllocator& descriptorSetAllocator
	) :
		deviceContext(deviceContext),
		descriptorSetLayouts(descriptorSetLayouts),
		descriptorSetAllocator(descriptorSetAllocator),
		storeDescriptorSet(descriptorSetAllocator.allocateDescriptorSets({descriptorSetLayouts.storeDescriptorSetLayout})[0]),
		globalDescriptorSet(descriptorSetAllocator.allocateDescriptorSets({descriptorSetLayouts.globalDescriptorSetLayout})[0]),
		commandBuffer(FrameState::CreateCommandBuffer(deviceContext)),
		fence(FrameState::CreateFence(deviceContext)),
		imageReadySemaphore(FrameState::CreateSemaphore(deviceContext)),
		imageRenderedSemaphore(FrameState::CreateSemaphore(deviceContext)),
		storeBufferAllocator(deviceContext, storeDescriptorSet),
		globalUniformBuffer(createGlobalUniformBuffer(deviceContext, globalDescriptorSet, &Global)) {
	}

	FrameState::FrameState(FrameState&& frameState)
		: deviceContext(frameState.deviceContext),
		  descriptorSetLayouts(frameState.descriptorSetLayouts),
		  descriptorSetAllocator(frameState.descriptorSetAllocator),
		  storeDescriptorSet(frameState.storeDescriptorSet),
		  globalDescriptorSet(frameState.globalDescriptorSet),
		  commandBuffer(frameState.commandBuffer),
		  fence(frameState.fence),
		  imageReadySemaphore(frameState.imageRenderedSemaphore),
		  imageRenderedSemaphore(frameState.imageRenderedSemaphore),
		  storeBufferAllocator(std::move(frameState.storeBufferAllocator)),
		  globalUniformBuffer(std::move(frameState.globalUniformBuffer)) {
		Stores = std::move(frameState.Stores);
		UniformStores = std::move(frameState.UniformStores);
		Global = frameState.Global;
	}

	FrameState::~FrameState() {
		if ((VkFence) fence != VK_NULL_HANDLE) {
			deviceContext.device.destroyFence(fence);
		}
		if ((VkSemaphore) imageReadySemaphore != VK_NULL_HANDLE) {
			deviceContext.device.destroySemaphore(imageReadySemaphore);
		}
		if ((VkSemaphore) imageRenderedSemaphore != VK_NULL_HANDLE) {
			deviceContext.device.destroySemaphore(imageRenderedSemaphore);
		}
		if ((VkBuffer) globalUniformBuffer.buffer != VK_NULL_HANDLE) {
			Devices::Device::unmapBuffer(deviceContext.Allocator, globalUniformBuffer);
			deviceContext.DestroyBuffer(globalUniformBuffer);
		}
	}

	vk::Semaphore FrameState::CreateSemaphore(const Devices::DeviceContext& deviceContext) {
		vk::SemaphoreCreateInfo semaphoreCreateInfo = {};
		auto semaphore = deviceContext.device.createSemaphore(semaphoreCreateInfo);
		return semaphore;
	}

	vk::Fence FrameState::CreateFence(const Devices::DeviceContext& deviceContext) {
		vk::FenceCreateInfo fenceCreateInfo = {
			.flags = vk::FenceCreateFlagBits::eSignaled
		};
		auto fence = deviceContext.device.createFence(fenceCreateInfo);
		return fence;
	}

	vk::CommandBuffer FrameState::CreateCommandBuffer(const Devices::DeviceContext& deviceContext) {
		vk::CommandBufferAllocateInfo commandBufferAllocateInfo = {
			.commandPool = deviceContext.CommandPool,
			.level = vk::CommandBufferLevel::ePrimary,
			.commandBufferCount = 1
		};
		auto commandBuffer = deviceContext.device.allocateCommandBuffers(commandBufferAllocateInfo);
		return commandBuffer[0];
	}

	Devices::Buffer FrameState::createGlobalUniformBuffer(
		const Devices::DeviceContext& deviceContext,
		const vk::DescriptorSet& descriptorSet,
		Models::Global** global
	) {
		auto itemByteLength = sizeof(Models::Global);
		auto byteLength = itemByteLength;
		auto bufferIndex = 0u;
		VmaAllocationCreateInfo allocationCreationInfo = {
			.flags = VmaAllocationCreateFlagBits::VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT,
			.usage = VmaMemoryUsage::VMA_MEMORY_USAGE_AUTO,
			.requiredFlags = (VkMemoryPropertyFlags) (vk::MemoryPropertyFlagBits::eHostVisible |
													  vk::MemoryPropertyFlagBits::eHostCoherent),
		};
		auto uniformBuffer = Devices::Device::createBuffer(
			deviceContext.Allocator,
			vk::MemoryPropertyFlagBits::eHostVisible,
			vk::BufferUsageFlagBits::eUniformBuffer,
			allocationCreationInfo,
			byteLength
		);

		Devices::Device::mapBuffer(deviceContext.Allocator, uniformBuffer, (void**) global);

		vk::DescriptorBufferInfo descriptorBufferInfo = {
			.buffer = uniformBuffer.buffer,
			.offset = 0,
			.range = VK_WHOLE_SIZE
		};

		vk::WriteDescriptorSet writeDescriptorSet = {
			.dstSet = descriptorSet,
			.dstBinding = 0,
			.dstArrayElement = bufferIndex,
			.descriptorCount = 1,
			.descriptorType = vk::DescriptorType::eUniformBuffer,
			.pBufferInfo = &descriptorBufferInfo
		};

		deviceContext.device.updateDescriptorSets(1, &writeDescriptorSet, 0, nullptr);

		return uniformBuffer;
	}
}
