#pragma once

#include "../../Common/ComponentIndex.hpp"
#include "StoreItemLocation.hpp"

namespace drk::Graphics::Models {
	struct Global {
		alignas(16) StoreItemLocation cameraItemLocation;
	};
}