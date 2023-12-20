#include "../../Graphics/Shaders/StoreItemLocation.glsl"

struct PointLight {

	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;

	StoreItemLocation lightStoreItemLocation;
	StoreItemLocation spatialStoreItemLocation;

};