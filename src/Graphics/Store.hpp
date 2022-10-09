#pragma once

#include <unordered_map>
#include <vector>
#include <queue>
#include "StoreBuffer.hpp"
#include "StoreBufferAllocator.hpp"

namespace drk::Graphics {

	template<typename T>
	struct StoreItemLocation {
		StoreBuffer<T> *pStore;
		uint32_t index;
		T *pItem;
	};

	class GenericStore {
	protected:
		StoreBufferAllocator *const StoreAllocator;
		std::vector<std::unique_ptr<GenericStoreBuffer>> Stores;

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
			auto store = StoreAllocator->Allocate<T>(1024);
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
				auto store = StoreAllocator->Allocate<T>(1024);
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

			StoreItemLocation<T> storeItemLocation = {
				.pStore = reinterpret_cast<StoreBuffer<T> *>(location.pStore),
				.index = location.index,
				.pItem = reinterpret_cast<T *>(location.pItem)
			};
			return storeItemLocation;
		}
	};
}
