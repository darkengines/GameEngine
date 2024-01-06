#pragma once

#include <glm/glm.hpp>
#include "../../Stores/Models/StoreItemLocation.hpp"

namespace drk::Lights::Models {
	struct PointLight {
		Stores::Models::StoreItemLocation lightStoreItemLocation;
		Stores::Models::StoreItemLocation spatialStoreItemLocation;
		Stores::Models::StoreItemLocation frontLightPerspectiveStoreItemLocation;
		Stores::Models::StoreItemLocation backLightPerspectiveStoreItemLocation;
		Stores::Models::StoreItemLocation leftLightPerspectiveStoreItemLocation;
		Stores::Models::StoreItemLocation rightLightPerspectiveStoreItemLocation;
		Stores::Models::StoreItemLocation topLightPerspectiveStoreItemLocation;
		Stores::Models::StoreItemLocation downLightPerspectiveStoreItemLocation;

		float constantAttenuation;
		float linearAttenuation;
		float quadraticAttenuation;
	};
}