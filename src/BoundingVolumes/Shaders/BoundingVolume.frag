#version 460
#extension GL_EXT_nonuniform_qualifier: enable

#include "BoundingVolumeDraw.glsl"
#include "Vertex.glsl"
#include "BoundingVolume.glsl"

#include "../../Graphics/Shaders/StoreItemLocation.glsl"
#include "../../Graphics/Shaders/Global.glsl"
#include "../../Spatials/shaders/Spatial.glsl"
#include "../../Cameras/shaders/Camera.glsl"

layout (set = 1, binding = 0) readonly buffer drawLayout {
    BoundingVolumeDraw[] BoundingVolumeDraws;
} BoundingVolumeDrawBuffer[];
layout(set = 2, binding = 0) uniform globalLayout {
    Global global;
} globalBuffer;
layout (set = 3, binding = 0) readonly buffer BoundingVolumeLayout {
    BoundingVolume[] boundingVolumes;
} boundingVolumeBuffer[];
layout (set = 3, binding = 0) readonly buffer spatialLayout {
    Spatial[] spatials;
} spatialBuffer[];
layout (set = 3, binding = 0) readonly buffer cameraLayout {
    Camera[] cameras;
} cameraBuffer[];

layout(location = 0) in Vertex point;
layout(location = 2) flat in StoreItemLocation drawItemLocation;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = point.color;
}