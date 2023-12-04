#pragma once

#include "../Models/LineVertex.hpp"
#include "../../Spatials/Components/Spatial.hpp"
#include "../../Stores/Models/StoreItemLocation.hpp"

namespace drk::Lines::Components {
	struct LineDraw {
		alignas(8) Stores::Models::StoreItemLocation lineItemLocation;
		alignas(8) Stores::Models::StoreItemLocation objectItemLocation;
	};
}