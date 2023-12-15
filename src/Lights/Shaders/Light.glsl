#include "../../Graphics/Shaders/StoreItemLocation.glsl"

struct Light {
	vec4 ambientColor;
	vec4 diffuseColor;
	vec4 specularColor;
	uint objectStateId;
};