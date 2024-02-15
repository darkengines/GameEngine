#include "../../Graphics/Shaders/StoreItemLocation.glsl"

struct Spotlight {

    StoreItemLocation lightStoreItemLocation;
    StoreItemLocation spatialStoreItemLocation;
    StoreItemLocation lightPerspectiveStoreItemLocation;

    float innerConeAngle;
    float outerConeAngle;
    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
};