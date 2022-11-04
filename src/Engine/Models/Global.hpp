#pragma once

#include "../../Common/ComponentIndex.hpp"
#include "../../Stores/Models/StoreItemLocation.hpp"

namespace drk::Engine::Models {
	struct Global {
		alignas(16) Stores::Models::StoreItemLocation cameraItemLocation;
	};
}