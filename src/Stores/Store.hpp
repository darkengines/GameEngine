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
		StoreBufferAllocator *const StoreAllocator;
		std::vector<std::unique_ptr<GenericStoreBuffer>> Stores;
		uint32_t ItemPerBuffer = 1024;

	public:
		GenericStore(StoreBufferAllocator *const storeBufferAllocator) :
			StoreAllocator(storeBufferAllocator) {

		}

		GenericStore(GenericStore &&genericStore) :
			StoreAllocator(genericStore.StoreAllocator), Stores(std::move(genericStore.Stores)) {
		}
	};

	template<typename T>
	class Store : public GenericStore {
	protected:
		StoreItemLocation<T> NextLocation;
		std::queue<StoreItemLocation<T>> AvailableLocations;
	public:
		Store(StoreBufferAllocator *const storeBufferAllocator) : GenericStore(storeBufferAllocator) {
			auto store = StoreAllocator->Allocate<T>(ItemPerBuffer);
			auto index = store->add();
			NextLocation = {
				.pStore = store.get(),
				.index = index,
				.pItem = store->mappedMemory()
			};
			Stores.push_back(std::move(store));
		}

		Store(Store &&store)
			: GenericStore(store), AvailableLocations(std::move(store.AvailableLocations)),
			  NextLocation(store.NextLocation) {}

		StoreItemLocation<T> Get(int index) {
			auto bufferIndex = index / ItemPerBuffer;
			auto bufferItemIndex = index % ItemPerBuffer;

			if (Stores.size() <= bufferIndex) {
				Stores.resize(bufferIndex + 1);
				auto store = StoreAllocator->Allocate<T>(ItemPerBuffer);
				auto memory = store->mappedMemory();

				StoreItemLocation<T> location = {
					.pStore = store.get(),
					.index = bufferItemIndex,
					.pItem = memory
				};

				Stores[bufferIndex] = std::move(store);

				return location;
			}
			auto pStore = reinterpret_cast<StoreBuffer<T> *>(Stores[bufferIndex].get());
			if (pStore == nullptr) {
				auto store = StoreAllocator->Allocate<T>(ItemPerBuffer);
				auto memory = store->mappedMemory();

				StoreItemLocation<T> location = {
					.pStore = store.get(),
					.index = bufferItemIndex,
					.pItem = memory
				};

				Stores[bufferIndex] = std::move(store);

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
//				auto store = StoreAllocator->Allocate<T>(ItemPerBuffer);
//				auto memory = store->mappedMemory();
//				Stores[bufferIndex] = std::move(store);
//				return &memory[bufferItemIndex];
//			}
//			auto pStore = reinterpret_cast<StoreBuffer<T>>(Stores[bufferIndex].get());
//			if (pStore == nullptr) {
//				auto store = StoreAllocator->Allocate<T>(ItemPerBuffer);
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
					.pStore = reinterpret_cast<StoreBuffer<T> *>(genericLocation.pStore),
					.index = genericLocation.index,
					.pItem = reinterpret_cast<T *>(genericLocation.pItem)
				};
				return storeItemLocation;
			}

			auto location = NextLocation;

			if (!NextLocation.pStore->hasAvailableIndex()) {
				auto store = StoreAllocator->Allocate<T>(ItemPerBuffer);
				auto index = store->add();
				auto mappedMemory = store->mappedMemory();
				NextLocation = {
					.pStore = store.get(),
					.index = index,
					.pItem = mappedMemory
				};
				Stores.push_back(std::move(store));
			}

			NextLocation.index = NextLocation.pStore->add();
			NextLocation.pItem = &NextLocation.pStore->mappedMemory()[NextLocation.index];

			StoreItemLocation<T> storeItemLocation = {
				.pStore = reinterpret_cast<StoreBuffer<T> *>(location.pStore),
				.index = location.index,
				.pItem = reinterpret_cast<T *>(location.pItem)
			};
			return storeItemLocation;
		}
	};
}
