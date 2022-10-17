#pragma once

#include <cstdint>
#include "StoreBuffer.hpp"

namespace drk::Stores {
	template<typename T>
	struct StoreItemLocation {
		StoreBuffer<T> *pStore;
		uint32_t index;
		T *pItem;
	};
}
