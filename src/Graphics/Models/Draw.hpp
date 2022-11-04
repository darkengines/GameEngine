#pragma once

#include "../../Common/ComponentIndex.hpp"
#include "../../Stores/Models/StoreItemLocation.hpp"

namespace drk::Graphics::Models {
	struct Draw {
		alignas(8) Stores::Models::StoreItemLocation meshItemLocation;
		alignas(8) Stores::Models::StoreItemLocation objectItemLocation;
	};
}