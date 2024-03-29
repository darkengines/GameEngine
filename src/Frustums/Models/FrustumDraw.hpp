#include "../../Stores/Models/StoreItemLocation.hpp"

namespace drk::Frustums::Models {
	struct FrustumDraw {
		Stores::Models::StoreItemLocation frustumStoreItemLocation;
		Stores::Models::StoreItemLocation spatialStoreItemLocation;
		Stores::Models::StoreItemLocation cameraStoreItemLocation;
	};
}