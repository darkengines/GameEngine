#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS

#include <typeindex>
#include <unordered_map>
#include <vulkan/vulkan.hpp>
#include "StoreBuffer.hpp"
#include "../Devices/DeviceContext.hpp"
#include "DescriptorSetLayoutCache.hpp"
#include "DescriptorSetAllocator.hpp"
#include "../Common/IndexGenerator.hpp"
#include "../Devices/Device.hpp"
#include "Store.hpp"

namespace drk::Graphics {
	class FrameState {
	protected:
		const Devices::DeviceContext *DeviceContext;
		static vk::CommandBuffer CreateCommandBuffer(const Devices::DeviceContext *deviceContext);
		static vk::Fence CreateFence(const Devices::DeviceContext *deviceContext);
		static vk::Semaphore CreateSemaphore(const Devices::DeviceContext *deviceContext);
		static vk::DescriptorSetLayout CreateStorageBufferDescriptorSetLayout(Graphics::DescriptorSetLayoutCache* const descriptorSetLayoutCache);
		static vk::DescriptorSet CreateStorageBufferDescriptorSet(Graphics::DescriptorSetAllocator* const descriptorSetAllocator, const vk::DescriptorSetLayout& descriptorSetLayout);
		DescriptorSetLayoutCache *DescriptorSetLayoutCache;
		DescriptorSetAllocator *DescriptorSetAllocator;
		vk::DescriptorSetLayout StorageBufferDescriptorSetLayout;
		vk::DescriptorSet StorageBufferDescriptorSet;
		std::unique_ptr<StoreBufferAllocator> StoreBufferAllocator;
	public:
		FrameState(
			const Devices::DeviceContext *deviceContext,
			Graphics::DescriptorSetLayoutCache *descriptorSetLayoutCache,
			Graphics::DescriptorSetAllocator *descriptorSetAllocator
		);
		FrameState(FrameState &&frameState);
		~FrameState();

		template<typename T>
		StoreItemLocation<T> AddStoreItem() {
			auto typeIndex = std::type_index(typeid(T));
			auto keyValuePair = stores.find(typeIndex);
			Store<T> *store = nullptr;
			if (keyValuePair == stores.end()) {
				auto pStore = std::make_unique<Store<T>>(StoreBufferAllocator.get());
				store = pStore.get();
				stores.emplace(
					typeIndex,
					std::move(pStore)
				);
			} else {
				store = reinterpret_cast<Store<T> *>(keyValuePair->second.get());
			}
			auto location = store->AddItem();
			return location;
		}

		vk::CommandBuffer CommandBuffer;
		vk::Fence Fence;
		vk::Semaphore ImageReadySemaphore;
		vk::Semaphore ImageRenderedSemaphore;

		std::unordered_map<std::type_index, std::unique_ptr<GenericStore>> stores;
	};
}