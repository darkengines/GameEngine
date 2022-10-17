#pragma once

#include "../../Common/ComponentIndex.hpp"
#include "StoreItemLocation.hpp"

namespace drk::Graphics::Models {
	struct Draw {
		StoreItemLocation meshItemLocation;
		StoreItemLocation objectItemLocation;
		uint32_t hasAlpha;
	};
}