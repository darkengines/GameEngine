#pragma once

#include "../Models/PointVertex.hpp"
#include "../../Spatials/Components/Spatial.hpp"
#include "../../Stores/Models/StoreItemLocation.hpp"

namespace drk::Points::Components {
	struct PointDraw {
		Spatials::Components::Spatial spatial;
		bool hasTransparency;
	};
}