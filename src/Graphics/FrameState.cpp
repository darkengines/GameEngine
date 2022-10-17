#include "FrameState.hpp"
#include "Models/Global.hpp"
#include <memory>

namespace drk::Graphics {

	FrameState::FrameState(
		const Devices::DeviceContext *deviceContext,
		const vk::DescriptorSetLayout &storageBufferDescriptorSetLayout,
		const vk::DescriptorSetLayout &drawBufferDescriptorSetLayout,
		const vk::DescriptorSetLayout &globalUniformBufferDescriptorSetLayout,
		Graphics::DescriptorSetAllocator *descriptorSetAllocator,
		const vk::DescriptorSet &textureDescriptorSet
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
			)), GlobalUniformBufferDescriptorSetLayout(globalUniformBufferDescriptorSetLayout),
		GlobalUniformBufferDescriptorSet(
			CreateGlobalUniformBufferDescriptorSet(
				DescriptorSetAllocator,
				GlobalUniformBufferDescriptorSetLayout
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
		DrawStore(std::make_unique<Stores::Store<Models::Draw>>(DrawStoreBufferAllocator.get())),
		GlobalUniformBuffer(CreateUnitormBuffer(DeviceContext, GlobalUniformBufferDescriptorSet, &Global)) {
		DescriptorSets.push_back(TextureDescriptorSet);
		DescriptorSets.push_back(StorageBufferDescriptorSet);
		DescriptorSets.push_back(DrawStorageBufferDescriptorSet);
		DescriptorSets.push_back(GlobalUniformBufferDescriptorSet);
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
		GlobalUniformBufferDescriptorSetLayout = frameState.GlobalUniformBufferDescriptorSetLayout;
		TextureDescriptorSet = frameState.TextureDescriptorSet;
		DescriptorSets = std::move(frameState.DescriptorSets);
		StorageBufferDescriptorSet = frameState.StorageBufferDescriptorSet;
		DrawStorageBufferDescriptorSet = frameState.DrawStorageBufferDescriptorSet;
		GlobalUniformBufferDescriptorSet = frameState.GlobalUniformBufferDescriptorSet;
		GlobalUniformBuffer = frameState.GlobalUniformBuffer;
		Global = frameState.Global;

		frameState.Fence = VK_NULL_HANDLE;
		frameState.ImageReadySemaphore = VK_NULL_HANDLE;
		frameState.ImageRenderedSemaphore = VK_NULL_HANDLE;
		frameState.StorageBufferDescriptorSetLayout = VK_NULL_HANDLE;
		frameState.StorageBufferDescriptorSet = VK_NULL_HANDLE;
		frameState.DrawStorageBufferDescriptorSetLayout = VK_NULL_HANDLE;
		frameState.DrawStorageBufferDescriptorSet = VK_NULL_HANDLE;
		frameState.GlobalUniformBuffer.buffer = VK_NULL_HANDLE;
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
		if ((VkBuffer) GlobalUniformBuffer.buffer != VK_NULL_HANDLE) {
			Devices::Device::unmapBuffer(DeviceContext->Allocator, GlobalUniformBuffer);
			DeviceContext->DestroyBuffer(GlobalUniformBuffer);
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

	vk::DescriptorSet FrameState::CreateGlobalUniformBufferDescriptorSet(
		Graphics::DescriptorSetAllocator *const descriptorSetAllocator,
		const vk::DescriptorSetLayout &descriptorSetLayout
	) {
		return descriptorSetAllocator->AllocateDescriptorSets({descriptorSetLayout})[0];
	}

	Devices::Buffer FrameState::CreateUnitormBuffer(
		const Devices::DeviceContext *deviceContext,
		const vk::DescriptorSet &descriptorSet,
		Models::Global **global
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
			deviceContext->Allocator,
			vk::MemoryPropertyFlagBits::eHostVisible,
			vk::BufferUsageFlagBits::eUniformBuffer,
			allocationCreationInfo,
			byteLength
		);

		Devices::Device::mapBuffer(deviceContext->Allocator, uniformBuffer, (void **) global);

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

		deviceContext->Device.updateDescriptorSets(1, &writeDescriptorSet, 0, nullptr);

		return uniformBuffer;
	}
}
