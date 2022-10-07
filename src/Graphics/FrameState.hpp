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
		DescriptorSetLayoutCache *DescriptorSetLayoutCache;
		DescriptorSetAllocator *DescriptorSetAllocator;
		std::unique_ptr<StoreBufferAllocator> StoreBufferAllocator;
	public:
		FrameState(
			const Devices::DeviceContext *deviceContext,
			Graphics::DescriptorSetLayoutCache *descriptorSetLayoutCache,
			Graphics::DescriptorSetAllocator *descriptorSetAllocator
		);
		FrameState(FrameState &&frameState);
		~FrameState();
		void CreateStorageBufferDescriptorSet();

		template<typename T>
		StoreItemLocation<T> AddStoreItem() {
			auto typeIndex = std::type_index(typeid(T));
			auto keyValuePair = stores.find(typeIndex);
			GenericStore *store = nullptr;
			if (keyValuePair == stores.end()) {
				auto x = std::move(GenericStore(StoreBufferAllocator.get()));
				stores.emplace( typeIndex, std::move(x));
				store = &stores[typeIndex];
			} else {
				store = &keyValuePair->second;
			}
			auto location = store->AddItem<T>();
			return location;
		}

		vk::CommandBuffer CommandBuffer;
		vk::Fence Fence;
		vk::Semaphore ImageReadySemaphore;
		vk::Semaphore ImageRenderedSemaphore;
		vk::DescriptorSetLayout StorageBufferDescriptorSetLayout;
		vk::DescriptorSet StorageBufferDescriptorSet;
		std::unordered_map<std::type_index, GenericStore> stores;
	};
}