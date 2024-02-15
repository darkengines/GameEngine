#include "../../Graphics/Shaders/StoreItemLocation.glsl"

struct PointLight {

    StoreItemLocation lightStoreItemLocation;
    StoreItemLocation spatialStoreItemLocation;
    StoreItemLocation frontLightPerspectiveStoreItemLocation;
    StoreItemLocation backLightPerspectiveStoreItemLocation;
    StoreItemLocation leftLightPerspectiveStoreItemLocation;
    StoreItemLocation rightLightPerspectiveStoreItemLocation;
    StoreItemLocation topLightPerspectiveStoreItemLocation;
    StoreItemLocation downLightPerspectiveStoreItemLocation;

    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;

};