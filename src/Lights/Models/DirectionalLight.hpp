
#pragma once

#include "../../Stores/Models/StoreItemLocation.hpp"

namespace drk::Lights::Models {
	struct DirectionalLight {
		Stores::Models::StoreItemLocation lightStoreItemLocation;
		Stores::Models::StoreItemLocation spatialStoreItemLocation;
		Stores::Models::StoreItemLocation lightPerspectiveStoreItemLocation;
	};
}