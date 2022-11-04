#pragma once

#include "../Models/PointVertex.hpp"
#include "../../Spatials/Spatial.hpp"
#include "../../Stores/Models/StoreItemLocation.hpp"

namespace drk::Points::Components {
	struct PointDraw {
		Models::PointVertex point;
		Stores::Models::StoreItemLocation pointStoreItem;
		Stores::Models::StoreItemLocation objectLocation;
		Spatials::Spatial spatial;
		bool hasTransparency;
	};
}