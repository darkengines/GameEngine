#pragma once

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
#include "../Graphics/Models/Draw.hpp"
#include "../Stores/StoreBufferAllocator.hpp"
#include "../Stores/Store.hpp"
#include "../Stores/StoreItem.hpp"
#include "Models/Global.hpp"
#include "DescriptorSetLayouts.hpp"

namespace drk::Engine {
	class FrameState {
	protected:
		const Devices::DeviceContext& deviceContext;
		const DescriptorSetLayouts& descriptorSetLayouts;
		DescriptorSetAllocator& descriptorSetAllocator;
	public:
		const vk::DescriptorSet globalDescriptorSet;
		const vk::DescriptorSet storeDescriptorSet;
		const vk::CommandBuffer commandBuffer;
		const vk::Fence fence;
		const vk::Semaphore imageReadySemaphore;
		const vk::Semaphore imageRenderedSemaphore;
	protected:
		Stores::StoreBufferAllocator storeBufferAllocator;
		const Devices::Buffer globalUniformBuffer;
	public:
		std::unordered_map<std::type_index, std::unique_ptr<Stores::GenericStore>> Stores;
		std::unordered_map<std::type_index, std::unique_ptr<Stores::GenericStore>> UniformStores;
		Models::Global* Global;

		static vk::CommandBuffer CreateCommandBuffer(const Devices::DeviceContext& deviceContext);
		static vk::Fence CreateFence(const Devices::DeviceContext& deviceContext);
		static vk::Semaphore CreateSemaphore(const Devices::DeviceContext& deviceContext);
		static Devices::Buffer createGlobalUniformBuffer(
			const Devices::DeviceContext& deviceContext,
			const vk::DescriptorSet& descriptorSet,
			Models::Global** global
		);
	public:
		FrameState(
			const Devices::DeviceContext& deviceContext,
			const DescriptorSetLayouts& descriptorSetLayouts,
			DescriptorSetAllocator& descriptorSetAllocator
		);
		FrameState(FrameState&& frameState) noexcept;
		~FrameState();

		template<typename T>
		Stores::StoreItemLocation<T> AddStoreItem() {
			auto typeIndex = std::type_index(typeid(T));
			auto keyValuePair = Stores.find(typeIndex);
			Stores::Store<T>* store;
			if (keyValuePair == Stores.end()) {
				auto pStore = std::make_unique<Stores::Store<T>>(storeBufferAllocator);
				store = pStore.get();
				Stores.emplace(
					typeIndex,
					std::move(pStore)
				);
			} else {
				store = reinterpret_cast<Stores::Store<T>*>(keyValuePair->second.get());
			}
			auto location = store->AddItem();
			return location;
		}

		template<typename TModel>
		Stores::UniformStore<TModel>& getUniformStore() {
			auto typeIndex = std::type_index(typeid(TModel));
			auto keyValuePair = UniformStores.find(typeIndex);
			Stores::UniformStore<TModel>* store;
			if (keyValuePair == UniformStores.end()) {
				auto descriptorSet = descriptorSetAllocator.allocateDescriptorSets({descriptorSetLayouts.storeDescriptorSetLayout})[0];
				auto pStore = std::make_unique<Stores::UniformStore<TModel>>(deviceContext, descriptorSet);
				store = pStore.get();
				UniformStores.emplace(
					typeIndex,
					std::move(pStore)
				);
			} else {
				store = static_cast<Stores::UniformStore<TModel>*>(keyValuePair->second.get());
			}
			return *store;
		}
//		template<>
//		Stores::UniformStore& getUniformStore(const std::type_index& typeIndex) {
//			auto keyValuePair = UniformStores.find(typeIndex);
//			Stores::UniformStore<TModel>* store = nullptr;
//			if (keyValuePair == UniformStores.end()) {
//				auto descriptorSet = descriptorSetAllocator.allocateDescriptorSets({descriptorSetLayouts.storeDescriptorSetLayout})[0];
//				auto pStore = std::make_unique<Stores::UniformStore<TModel>>(deviceContext, descriptorSet);
//				store = pStore.get();
//				UniformStores.emplace(
//					typeIndex,
//					std::move(pStore)
//				);
//			} else {
//				store = static_cast<Stores::UniformStore<TModel>*>(keyValuePair->second.get());
//			}
//			return *store;
//		}
	};
}