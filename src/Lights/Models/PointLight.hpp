#include <glm/glm.hpp>
#include "../../Stores/Models/StoreItemLocation.hpp"

namespace drk::Lights::Models {
	struct PointLight {
		
		float constantAttenuation;
		float linearAttenuation;
		float quadraticAttenuation;

		Stores::Models::StoreItemLocation perspectiveStoreItemLocation;
		Stores::Models::StoreItemLocation lightStoreItemLocation;
		Stores::Models::StoreItemLocation spatialStoreItemLocation;
	};
}