#pragma once

#include <vector>
#include "Store.hpp"

namespace drk::Graphics {
	template<typename T>
	struct StoreItem {
		std::vector<StoreItemLocation<T>> frameStoreItems;
	};
}