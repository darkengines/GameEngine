#pragma once

#include "../../Stores/Models/StoreItemLocation.hpp"

namespace drk::Lines::Models {
	struct LineDraw {
		alignas(8)  Stores::Models::StoreItemLocation lineItemLocation;
		alignas(8) Stores::Models::StoreItemLocation objectItemLocation;
	};
}
