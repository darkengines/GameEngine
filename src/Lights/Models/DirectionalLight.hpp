
#pragma once

#include "../../Stores/Models/StoreItemLocation.hpp"

namespace drk::Lights::Models {
	struct DirectionalLight {
		Stores::Models::StoreItemLocation perspectiveStoreItemLocation;
		Stores::Models::StoreItemLocation lightStoreItemLocation;
		Stores::Models::StoreItemLocation spatialStoreItemLocation;
	};
}