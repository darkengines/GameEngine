#pragma once

#include <cstdint>
#include "StoreBuffer.hpp"
#include "Models/StoreItemLocation.hpp"

namespace drk::Stores {
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
