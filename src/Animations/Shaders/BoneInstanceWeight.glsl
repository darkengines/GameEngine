#include "../../Graphics/Shaders/StoreItemLocation.glsl"

struct BoneInstanceWeight {
    StoreItemLocation boneInstanceStoreItemLocation;
    StoreItemLocation boneInstanceSpatialStoreItemLocation;
    StoreItemLocation boneStoreItemLocation;
    float weight;
};
