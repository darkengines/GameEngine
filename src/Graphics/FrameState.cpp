#include "FrameState.hpp"
#include <memory>

namespace drk::Graphics {

	FrameState::FrameState(
		const Devices::DeviceContext *deviceContext, Graphics::DescriptorSetLayoutCache *descriptorSetLayoutCache,
		Graphics::DescriptorSetAllocator *descriptorSetAllocator
	) :
		DeviceContext(deviceContext),
		DescriptorSetLayoutCache(descriptorSetLayoutCache),
		DescriptorSetAllocator(descriptorSetAllocator),
		CommandBuffer(FrameState::CreateCommandBuffer(DeviceContext)),
		Fence(FrameState::CreateFence(DeviceContext)),
		ImageReadySemaphore(FrameState::CreateSemaphore(DeviceContext)),
		ImageRenderedSemaphore(FrameState::CreateSemaphore(DeviceContext)),
		StoreBufferAllocator(
			std::make_unique<Graphics::StoreBufferAllocator>(
				DeviceContext,
				StorageBufferDescriptorSet
			)) {
	}

	FrameState::FrameState(FrameState &&frameState) {
		DeviceContext = frameState.DeviceContext;
		DescriptorSetLayoutCache = frameState.DescriptorSetLayoutCache;
		DescriptorSetAllocator = frameState.DescriptorSetAllocator;
		CommandBuffer = frameState.CommandBuffer;
		Fence = frameState.Fence;
		ImageReadySemaphore = frameState.ImageReadySemaphore;
		ImageRenderedSemaphore = frameState.ImageRenderedSemaphore;
		stores = std::move(frameState.stores);

		frameState.Fence = VK_NULL_HANDLE;
		frameState.ImageReadySemaphore = VK_NULL_HANDLE;
		frameState.ImageRenderedSemaphore = VK_NULL_HANDLE;
		frameState.StorageBufferDescriptorSetLayout = VK_NULL_HANDLE;
		frameState.StorageBufferDescriptorSet = VK_NULL_HANDLE;
	}

	FrameState::~FrameState() {
		if ((VkFence) Fence != VK_NULL_HANDLE) {
			DeviceContext->Device.destroyFence(Fence);
		}
		if ((VkSemaphore) ImageReadySemaphore != VK_NULL_HANDLE) {
			DeviceContext->Device.destroySemaphore(ImageReadySemaphore);
		}
		if ((VkSemaphore) ImageRenderedSemaphore != VK_NULL_HANDLE) {
			DeviceContext->Device.destroySemaphore(ImageRenderedSemaphore);
		}
	}

	vk::Semaphore FrameState::CreateSemaphore(const Devices::DeviceContext *deviceContext) {
		vk::SemaphoreCreateInfo semaphoreCreateInfo = {};
		auto semaphore = deviceContext->Device.createSemaphore(semaphoreCreateInfo);
		return semaphore;
	}

	vk::Fence FrameState::CreateFence(const Devices::DeviceContext *deviceContext) {
		vk::FenceCreateInfo fenceCreateInfo = {
			.flags = vk::FenceCreateFlagBits::eSignaled
		};
		auto fence = deviceContext->Device.createFence(fenceCreateInfo);
		return fence;
	}

	vk::CommandBuffer FrameState::CreateCommandBuffer(const Devices::DeviceContext *deviceContext) {
		vk::CommandBufferAllocateInfo commandBufferAllocateInfo = {
			.commandPool = deviceContext->CommandPool,
			.level = vk::CommandBufferLevel::ePrimary,
			.commandBufferCount = 1
		};
		auto commandBuffer = deviceContext->Device.allocateCommandBuffers(commandBufferAllocateInfo);
		return commandBuffer[0];
	}

	void FrameState::CreateStorageBufferDescriptorSet() {
		vk::DescriptorSetLayoutBinding binding = {
			.binding = 0,
			.descriptorType = vk::DescriptorType::eStorageBuffer,
			.descriptorCount = 64,
			.stageFlags = vk::ShaderStageFlagBits::eAll
		};
		vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
			.bindingCount = 1,
			.pBindings = &binding
		};
		StorageBufferDescriptorSetLayout = DescriptorSetLayoutCache->get(descriptorSetLayoutCreateInfo);
		StorageBufferDescriptorSet = DescriptorSetAllocator->AllocateDescriptorSets({StorageBufferDescriptorSetLayout})[0];
	}
}
