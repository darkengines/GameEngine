#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS

#include <typeindex>
#include <unordered_map>
#include <vulkan/vulkan.hpp>
#include "../Stores/StoreBuffer.hpp"
#include "../Devices/DeviceContext.hpp"
#include "DescriptorSetLayoutCache.hpp"
#include "DescriptorSetAllocator.hpp"
#include "../Common/IndexGenerator.hpp"
#include "../Devices/Device.hpp"
#include "../Stores/Store.hpp"
#include "Models/Draw.hpp"
#include "../Stores/StoreBufferAllocator.hpp"
#include "../Stores/Store.hpp"
#include "../Stores/StoreItem.hpp"

namespace drk::Graphics {
	class FrameState {
	protected:
		const Devices::DeviceContext *DeviceContext;
		static vk::CommandBuffer CreateCommandBuffer(const Devices::DeviceContext *deviceContext);
		static vk::Fence CreateFence(const Devices::DeviceContext *deviceContext);
		static vk::Semaphore CreateSemaphore(const Devices::DeviceContext *deviceContext);
		static vk::DescriptorSet CreateStorageBufferDescriptorSet(
			Graphics::DescriptorSetAllocator *const descriptorSetAllocator,
			const vk::DescriptorSetLayout &descriptorSetLayout
		);
		DescriptorSetAllocator *DescriptorSetAllocator;
		vk::DescriptorSetLayout StorageBufferDescriptorSetLayout;
		vk::DescriptorSet StorageBufferDescriptorSet;

		vk::DescriptorSetLayout DrawStorageBufferDescriptorSetLayout;
		vk::DescriptorSet DrawStorageBufferDescriptorSet;

		std::unique_ptr<Stores::StoreBufferAllocator> StoreBufferAllocator;
		std::unique_ptr<Stores::StoreBufferAllocator> DrawStoreBufferAllocator;
	public:
		FrameState(
			const Devices::DeviceContext *deviceContext,
			const vk::DescriptorSetLayout &storageBufferDescriptorSetLayout,
			const vk::DescriptorSetLayout &drawBufferDescriptorSetLayout,
			Graphics::DescriptorSetAllocator *descriptorSetAllocator,
			const vk::DescriptorSet &textureDescriptorSet
		);
		FrameState(FrameState &&frameState);
		~FrameState();

		template<typename T>
		Stores::StoreItemLocation<T> AddStoreItem() {
			auto typeIndex = std::type_index(typeid(T));
			auto keyValuePair = Stores.find(typeIndex);
			Stores::Store<T> *store = nullptr;
			if (keyValuePair == Stores.end()) {
				auto pStore = std::make_unique<Stores::Store < T>>
				(StoreBufferAllocator.get());
				store = pStore.get();
				Stores.emplace(
					typeIndex,
					std::move(pStore)
				);
			} else {
				store = reinterpret_cast<Stores::Store<T> *>(keyValuePair->second.get());
			}
			auto location = store->AddItem();
			return location;
		}

		template<typename TModel>
		Stores::Store<TModel> *GetStore() {
			auto typeIndex = std::type_index(typeid(TModel));
			auto keyValuePair = Stores.find(typeIndex);
			Stores::Store<TModel> *store = nullptr;
			if (keyValuePair == Stores.end()) {
				auto pStore = std::make_unique<Stores::Store<TModel>>
					(StoreBufferAllocator.get());
				store = pStore.get();
				Stores.emplace(
					typeIndex,
					std::move(pStore)
				);
			} else {
				store = reinterpret_cast<Stores::Store<TModel> *>(keyValuePair->second.get());
			}
			return store;
		}

		vk::CommandBuffer CommandBuffer;
		vk::Fence Fence;
		vk::Semaphore ImageReadySemaphore;
		vk::Semaphore ImageRenderedSemaphore;

		std::unordered_map<std::type_index, std::unique_ptr<Stores::GenericStore>> Stores;
		std::unique_ptr<Stores::Store<Models::Draw>> DrawStore;
		std::vector<vk::DescriptorSet> DescriptorSets;
		vk::DescriptorSet TextureDescriptorSet;
	};
}