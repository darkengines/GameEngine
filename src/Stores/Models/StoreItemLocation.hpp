#pragma once

#include <cstdint>

namespace drk::Stores::Models {
	struct StoreItemLocation {
		uint32_t storeIndex;
		uint32_t itemIndex;
	};
}