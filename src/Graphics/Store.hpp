#pragma once

#include <unordered_map>
#include <vector>
#include <queue>
#include "StoreBuffer.hpp"
#include "StoreBufferAllocator.hpp"

namespace drk::Graphics {
	struct GenericStoreItemLocation {
		GenericStoreBuffer *pStore;
		uint32_t index;
		void *pItem;
	};

	template<typename T>
	struct StoreItemLocation : public GenericStoreItemLocation {
		StoreBuffer<T> *pStore;
		T *pItem;
	};

	class GenericStore {
	protected:
		StoreBufferAllocator *const StoreAllocator;
		std::vector<GenericStoreBuffer> Stores;
		std::queue<GenericStoreItemLocation> AvailableLocations;
		GenericStoreItemLocation NextLocation;
	public:
		GenericStore(StoreBufferAllocator *const storeBufferAllocator) :
			StoreAllocator(storeBufferAllocator) {
		}

		GenericStore(GenericStore &&genericStore) :
			StoreAllocator(genericStore.StoreAllocator), Stores(std::move(genericStore.Stores)),
			AvailableLocations(std::move(genericStore.AvailableLocations)), NextLocation(genericStore.NextLocation) {
		}

		template<typename T>
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
				Stores.push_back(std::move(store));
				NextLocation.pStore = &Stores.back();
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
