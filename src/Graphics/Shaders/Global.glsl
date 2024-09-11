#include "StoreItemLocation.glsl"

const uint albedo_style = 1<<1;
const uint normal_style = 1<<2;
const uint uv_style = 1<<3;
const uint material_style = 1<<4;
const uint vertexColor_style = 1<<5;


struct Global {
    uint cameraStoreIndex;
    uint cameraItemIndex;
    uint pointLightArrayIndex;
    uint pointLightCount;
    uint directionalLightArrayIndex;
    uint directionalLightCount;
    uint spotlightArrayIndex;
    uint spotlightCount;
    uint renderStyle;
};