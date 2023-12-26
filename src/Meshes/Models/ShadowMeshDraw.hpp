#pragma once

#include "../../Common/ComponentIndex.hpp"
#include "../../Stores/Models/StoreItemLocation.hpp"

namespace drk::Meshes::Models {
	struct ShadowMeshDraw {
		alignas(8) Stores::Models::StoreItemLocation meshItemLocation;
		alignas(8) Stores::Models::StoreItemLocation objectItemLocation;
		alignas(8) Stores::Models::StoreItemLocation cameraItemLocation;
		alignas(8) Stores::Models::StoreItemLocation LightPerspectiveItemLocation;
		alignas(8) Stores::Models::StoreItemLocation LightPerspectiveSpatialItemLocation;
	};
}