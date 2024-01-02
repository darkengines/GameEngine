#include "../../Stores/Models/StoreItemLocation.hpp"

namespace drk::BoundingVolumes::Components {
	struct Draw {
		Stores::Models::StoreItemLocation boundingVolumeStoreItemLocation;
		Stores::Models::StoreItemLocation cameraStoreItemLocation;
	};
}