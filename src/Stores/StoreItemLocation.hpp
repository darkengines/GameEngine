#pragma once

#include <cstdint>
#include "StoreBuffer.hpp"
#include "Models/StoreItemLocation.hpp"

namespace drk::Stores {
	struct GenericStoreItemLocation {
		uint32_t descriptorArrayElement;
		uint32_t itemIndex;
	};

	template<typename T>
	struct StoreItemLocation {
		StoreBuffer<T>* pStore;
		uint32_t index;
		T* pItem;

		operator Models::StoreItemLocation() const {
			return {
				.storeIndex = pStore->descriptorArrayElement,
				.itemIndex = index
			};
		}
	};
}
