#include "../../Graphics/Shaders/StoreItemLocation.glsl"

struct Spotlight {
	vec4 relativePosition;
	vec4 relativeDirection;
	vec4 relativeUp;
	vec4 absolutePosition;
	vec4 absoluteDirection;
	vec4 absoluteUp;
	vec4 shadowMapRect;
	mat4 view;
	mat4 perspective;
	float innerConeAngle;
	float outerConeAngle;
	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;
	uint lightStateId;
	StoreItemLocation lightStoreItemLocation;
	StoreItemLocation spatialStoreItemLocation;
};