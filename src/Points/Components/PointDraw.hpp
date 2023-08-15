#pragma once

#include "../Models/PointVertex.hpp"
#include "../../Spatials/Components/Spatial.hpp"
#include "../../Stores/Models/StoreItemLocation.hpp"

namespace drk::Points::Components {
	struct PointDraw {
		alignas(8) Stores::Models::StoreItemLocation pointItemLocation;
		alignas(8) Stores::Models::StoreItemLocation objectItemLocation;
	};
}