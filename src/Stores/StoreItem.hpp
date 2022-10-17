#pragma once

#include <vector>
#include "Store.hpp"

namespace drk::Stores {
	template<typename T>
	struct StoreItem {
		std::vector<StoreItemLocation<T>> frameStoreItems;
	};
}