#include "../../Stores/Models/StoreItemLocation.hpp"

namespace drk::BoundingVolumes::Models {
	struct BoundingVolumeDraw {
		Stores::Models::StoreItemLocation boundingVolumeStoreItemLocation;
		Stores::Models::StoreItemLocation cameraStoreItemLocation;
	};
}