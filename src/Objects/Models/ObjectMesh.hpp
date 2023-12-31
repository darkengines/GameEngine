#pragma once

#include "../../Stores/Models/StoreItemLocation.hpp"

namespace drk::Objects::Models {
	struct ObjectMesh {
		Stores::Models::StoreItemLocation objectStoreItemLocation;
		Stores::Models::StoreItemLocation meshStoreItemLocation;
		Stores::Models::StoreItemLocation axisAlignedBoundingBoxStoreItemLocation;
	};
}