#include "../../Graphics/Shaders/StoreItemLocation.glsl"

struct BoneInstanceWeight {
    StoreItemLocation boneInstanceStoreItemLocation;
    StoreItemLocation boneInstanceSpatialStoreItemLocation;
    StoreItemLocation boneMeshStoreItemLocation;
    float weight;
};
