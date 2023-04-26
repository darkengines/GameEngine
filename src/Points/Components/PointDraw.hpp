#pragma once

#include "../Models/PointVertex.hpp"
#include "../../Spatials/Spatial.hpp"
#include "../../Stores/Models/StoreItemLocation.hpp"

namespace drk::Points::Components {
	struct PointDraw {
		Spatials::Spatial spatial;
		bool hasTransparency;
	};
}