#pragma once

#include "../../Stores/Models/StoreItemLocation.hpp"

namespace drk::Nodes::Models {
	struct Object {
		alignas(8) Stores::Models::StoreItemLocation spatialItemLocation;
		alignas(8) Stores::Models::StoreItemLocation relativeSpatialItemLocation;
	};
}