#pragma once

#include "../../Stores/Models/StoreItemLocation.hpp"

namespace drk::Points::Models {
	struct PointDraw {
		alignas(8)  Stores::Models::StoreItemLocation pointItemLocation;
		alignas(8)  Stores::Models::StoreItemLocation objectItemLocation;
	};
}
