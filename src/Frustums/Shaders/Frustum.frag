#version 460
#extension GL_EXT_nonuniform_qualifier: enable
#extension GL_KHR_vulkan_glsl: enable

#include "FrustumDraw.glsl"
#include "Vertex.glsl"
#include "Frustum.glsl"

#include "../../Graphics/Shaders/StoreItemLocation.glsl"
#include "../../Graphics/Shaders/Global.glsl"
#include "../../Spatials/shaders/Spatial.glsl"
#include "../../Cameras/shaders/Camera.glsl"

layout (set = 1, binding = 0) readonly buffer drawLayout {
    FrustumDraw[] FrustumDraws;
} FrustumDrawBuffer[];
layout(set = 2, binding = 0) uniform globalLayout {
    Global global;
} globalBuffer;
layout (set = 3, binding = 0) readonly buffer FrustumLayout {
    Frustum[] frustums;
} frustumBuffer[];
layout (set = 3, binding = 0) readonly buffer spatialLayout {
    Spatial[] spatials;
} spatialBuffer[];
layout (set = 3, binding = 0) readonly buffer cameraLayout {
    Camera[] cameras;
} cameraBuffer[];

layout(location = 0) in FragmentVertex point;
layout(location = 2) flat in StoreItemLocation drawItemLocation;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = point.color;
}