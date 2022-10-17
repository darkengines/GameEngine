#include "FrameState.hpp"
#include <memory>

namespace drk::Graphics {

	FrameState::FrameState(
		const Devices::DeviceContext *deviceContext,
		const vk::DescriptorSetLayout &storageBufferDescriptorSetLayout,
		const vk::DescriptorSetLayout &drawBufferDescriptorSetLayout,
		Graphics::DescriptorSetAllocator *descriptorSetAllocator, const vk::DescriptorSet &textureDescriptorSet
	) :
		DeviceContext(deviceContext),
		DescriptorSetAllocator(descriptorSetAllocator),
		TextureDescriptorSet(textureDescriptorSet),
		CommandBuffer(FrameState::CreateCommandBuffer(DeviceContext)),
		Fence(FrameState::CreateFence(DeviceContext)),
		ImageReadySemaphore(FrameState::CreateSemaphore(DeviceContext)),
		ImageRenderedSemaphore(FrameState::CreateSemaphore(DeviceContext)),
		StorageBufferDescriptorSetLayout(storageBufferDescriptorSetLayout),
		StorageBufferDescriptorSet(
			CreateStorageBufferDescriptorSet(
				DescriptorSetAllocator,
				StorageBufferDescriptorSetLayout
			)),
		DrawStorageBufferDescriptorSetLayout(drawBufferDescriptorSetLayout),
		DrawStorageBufferDescriptorSet(
			CreateStorageBufferDescriptorSet(
				DescriptorSetAllocator,
				DrawStorageBufferDescriptorSetLayout
			)),
		DrawStoreBufferAllocator(
			std::make_unique<Stores::StoreBufferAllocator>(
				DeviceContext,
				DrawStorageBufferDescriptorSet
			)),
		StoreBufferAllocator(
			std::make_unique<Stores::StoreBufferAllocator>(
				DeviceContext,
				StorageBufferDescriptorSet
			)),
		DrawStore(std::make_unique<Stores::Store<Models::Draw>>(DrawStoreBufferAllocator.get())) {

		DescriptorSets.push_back(TextureDescriptorSet);
		DescriptorSets.push_back(StorageBufferDescriptorSet);
		DescriptorSets.push_back(DrawStorageBufferDescriptorSet);
	}

	FrameState::FrameState(FrameState &&frameState) {
		DeviceContext = frameState.DeviceContext;
		DescriptorSetAllocator = frameState.DescriptorSetAllocator;
		CommandBuffer = frameState.CommandBuffer;
		Fence = frameState.Fence;
		ImageReadySemaphore = frameState.ImageReadySemaphore;
		ImageRenderedSemaphore = frameState.ImageRenderedSemaphore;
		Stores = std::move(frameState.Stores);
		StoreBufferAllocator = std::move(frameState.StoreBufferAllocator);
		DrawStoreBufferAllocator = std::move(frameState.DrawStoreBufferAllocator);
		DrawStore = std::move(frameState.DrawStore);
		StorageBufferDescriptorSetLayout = frameState.StorageBufferDescriptorSetLayout;
		DrawStorageBufferDescriptorSetLayout = frameState.DrawStorageBufferDescriptorSetLayout;
		TextureDescriptorSet = frameState.TextureDescriptorSet;
		DescriptorSets = std::move(frameState.DescriptorSets);
		StorageBufferDescriptorSet = frameState.StorageBufferDescriptorSet;
		DrawStorageBufferDescriptorSet = frameState.DrawStorageBufferDescriptorSet;

		frameState.Fence = VK_NULL_HANDLE;
		frameState.ImageReadySemaphore = VK_NULL_HANDLE;
		frameState.ImageRenderedSemaphore = VK_NULL_HANDLE;
		frameState.StorageBufferDescriptorSetLayout = VK_NULL_HANDLE;
		frameState.StorageBufferDescriptorSet = VK_NULL_HANDLE;
		frameState.DrawStorageBufferDescriptorSetLayout = VK_NULL_HANDLE;
		frameState.DrawStorageBufferDescriptorSet = VK_NULL_HANDLE;
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

	vk::DescriptorSet FrameState::CreateStorageBufferDescriptorSet(
		Graphics::DescriptorSetAllocator *const descriptorSetAllocator,
		const vk::DescriptorSetLayout &descriptorSetLayout
	) {
		return descriptorSetAllocator->AllocateDescriptorSets({descriptorSetLayout})[0];
	}
}
