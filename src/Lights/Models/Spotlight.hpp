#pragma once
#include "../../Stores/Models/StoreItemLocation.hpp"

namespace drk::Lights::Models {
	struct Spotlight {

		float innerConeAngle;
		float outerConeAngle;
		float constantAttenuation;
		float linearAttenuation;
		float quadraticAttenuation;

		Stores::Models::StoreItemLocation lightStoreItemLocation;
		Stores::Models::StoreItemLocation spatialStoreItemLocation;
		Stores::Models::StoreItemLocation lightPerspectiveStoreItemLocation;
	};
}
