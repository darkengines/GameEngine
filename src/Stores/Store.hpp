#pragma once

#include <unordered_map>
#include <vector>
#include <queue>
#include "StoreBuffer.hpp"
#include "StoreBufferAllocator.hpp"
#include "StoreItemLocation.hpp"

namespace drk::Stores {

	class GenericStore {
	protected:
		StoreBufferAllocator& storeAllocator;
		uint32_t ItemPerBuffer;

	public:
		std::vector<std::unique_ptr<GenericStoreBuffer>> stores;
		GenericStore(StoreBufferAllocator& storeBufferAllocator) :
			storeAllocator(storeBufferAllocator),
			ItemPerBuffer(131072u) /* todo: make this ItemPerBuffer(1024u) configurable */ {

		}

		GenericStore(GenericStore&& genericStore) :
			storeAllocator(genericStore.storeAllocator), stores(std::move(genericStore.stores)),
			ItemPerBuffer(genericStore.ItemPerBuffer) {
		}
		virtual ~GenericStore() {};
		GenericStoreItemLocation get(uint32_t index) {
			auto bufferIndex = index / ItemPerBuffer;
			auto bufferItemIndex = index % ItemPerBuffer;
			return {
				.descriptorArrayElement = stores[bufferIndex]->descriptorArrayElement,
				.itemIndex = bufferItemIndex
			};
		}
	};

	template<typename T>
	class Store : public GenericStore {
	protected:
		StoreItemLocation<T> NextLocation;
		std::queue<StoreItemLocation<T>> AvailableLocations;
	public:
		Store(StoreBufferAllocator& storeBufferAllocator) : GenericStore(storeBufferAllocator) {
			auto store = storeAllocator.allocate<T>(ItemPerBuffer);
			auto index = store->add();
			NextLocation = {
				.pStore = store.get(),
				.index = index,
				.pItem = store->mappedMemory()
			};
			stores.push_back(std::move(store));
		}

		Store(Store&& store)
			: GenericStore(store), AvailableLocations(std::move(store.AvailableLocations)),
			  NextLocation(store.NextLocation) {}

		StoreItemLocation<T> get(int index) {
			auto bufferIndex = index / ItemPerBuffer;
			auto bufferItemIndex = index % ItemPerBuffer;

			if (stores.size() <= bufferIndex) {
				stores.resize(bufferIndex + 1);
				auto store = storeAllocator.allocate<T>(ItemPerBuffer);
				auto memory = store->mappedMemory();

				StoreItemLocation<T> location = {
					.pStore = store.get(),
					.index = bufferItemIndex,
					.pItem = memory
				};

				stores[bufferIndex] = std::move(store);

				return location;
			}
			auto pStore = reinterpret_cast<StoreBuffer<T>*>(stores[bufferIndex].get());
			if (pStore == nullptr) {
				auto store = storeAllocator.allocate<T>(ItemPerBuffer);
				auto memory = store->mappedMemory();

				StoreItemLocation<T> location = {
					.pStore = store.get(),
					.index = bufferItemIndex,
					.pItem = memory
				};

				stores[bufferIndex] = std::move(store);

				return location;
			}

			StoreItemLocation<T> location = {
				.pStore = pStore,
				.index = bufferItemIndex,
				.pItem = &pStore->mappedMemory()[bufferItemIndex]
			};

			return location;
		}

//		T &operator[](int index) {
//			auto bufferIndex = index / ItemPerBuffer;
//			auto bufferItemIndex = index % ItemPerBuffer;
//			if (Stores.size() <= bufferIndex) {
//				Stores.resize(bufferIndex + 1);
//				auto store = storeAllocator.Allocate<T>(ItemPerBuffer);
//				auto memory = store->mappedMemory();
//				Stores[bufferIndex] = std::move(store);
//				return &memory[bufferItemIndex];
//			}
//			auto pStore = reinterpret_cast<StoreBuffer<T>>(Stores[bufferIndex].get());
//			if (pStore == nullptr) {
//				auto store = storeAllocator.Allocate<T>(ItemPerBuffer);
//				auto memory = store->mappedMemory();
//				Stores[bufferIndex] = std::move(store);
//				return &memory[bufferItemIndex];
//			}
//
//			return &pStore->mappedMemory()[bufferItemIndex];
//		}

		StoreItemLocation<T> AddItem() {
			if (!AvailableLocations.empty()) {
				auto genericLocation = AvailableLocations.front();
				AvailableLocations.pop();
				StoreItemLocation<T> storeItemLocation = {
					.pStore = reinterpret_cast<StoreBuffer<T>*>(genericLocation.pStore),
					.index = genericLocation.index,
					.pItem = reinterpret_cast<T*>(genericLocation.pItem)
				};
				return storeItemLocation;
			}

			auto location = NextLocation;

			if (!NextLocation.pStore->hasAvailableIndex()) {
				auto store = storeAllocator.allocate<T>(ItemPerBuffer);
				auto index = store->add();
				auto mappedMemory = store->mappedMemory();
				NextLocation = {
					.pStore = store.get(),
					.index = index,
					.pItem = mappedMemory
				};
				stores.push_back(std::move(store));
			}

			NextLocation.index = NextLocation.pStore->add();
			NextLocation.pItem = &NextLocation.pStore->mappedMemory()[NextLocation.index];

			StoreItemLocation<T> storeItemLocation = {
				.pStore = reinterpret_cast<StoreBuffer<T>*>(location.pStore),
				.index = location.index,
				.pItem = reinterpret_cast<T*>(location.pItem)
			};
			return storeItemLocation;
		}
	};


	struct StoreBufferAllocatorOwner {
		StoreBufferAllocatorOwner(const Devices::DeviceContext& deviceContext, const vk::DescriptorSet& descriptorSet)
			: storeBufferAllocator(deviceContext, descriptorSet) {}
		StoreBufferAllocator storeBufferAllocator;

		StoreBufferAllocatorOwner(StoreBufferAllocatorOwner&& storeBufferAllocatorOwner)
			: storeBufferAllocator(std::move(storeBufferAllocatorOwner.storeBufferAllocator)) {}
	};

	template<typename TModel>
	class UniformStore : public StoreBufferAllocatorOwner, public Store<TModel> {
	protected:
		const Devices::DeviceContext& deviceContext;
	public:
		vk::DescriptorSet descriptorSet;
		UniformStore(const Devices::DeviceContext& deviceContext, const vk::DescriptorSet& descriptorSet)
			: StoreBufferAllocatorOwner(deviceContext, descriptorSet),
			  Store<TModel>(storeBufferAllocator), deviceContext(deviceContext), descriptorSet(descriptorSet) {
		}
		UniformStore(UniformStore&& uniformStore)
			: StoreBufferAllocatorOwner(std::move(uniformStore)), Store<TModel>(std::move(uniformStore)),
			  deviceContext(uniformStore.deviceContext),
			  descriptorSet(uniformStore.descriptorSet) {
		}
		const vk::DescriptorSet& getDescriptorSet() const {
			return descriptorSet;
		}
	};
}
