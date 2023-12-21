#include "../../Graphics/Shaders/StoreItemLocation.glsl"

struct Spotlight {
	
	float innerConeAngle;
	float outerConeAngle;
	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;

	StoreItemLocation lightStoreItemLocation;
	StoreItemLocation spatialStoreItemLocation;
	StoreItemLocation lightPerspectiveStoreItemLocation;
};