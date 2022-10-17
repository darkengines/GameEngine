#pragma once

#include "../../Common/ComponentIndex.hpp"
#include "StoreItemLocation.hpp"

namespace drk::Graphics::Models {
	struct Draw {
		alignas(8) StoreItemLocation meshItemLocation;
		alignas(8) StoreItemLocation objectItemLocation;
	};
}