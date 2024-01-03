#version 460
#extension GL_EXT_nonuniform_qualifier: enable
#extension GL_KHR_vulkan_glsl: enable

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

layout(location = 0) in vec4 inPosition;
layout(location = 4) in vec4 inColor;

layout(location = 0) out Vertex fragment;
layout(location = 2) flat out StoreItemLocation drawItemLocation;

void main() {
    uint BoundingVolumeDrawBufferIndex = gl_InstanceIndex / 131072u;
    uint drawItemIndex = gl_InstanceIndex % 131072u;
    BoundingVolumeDraw draw = BoundingVolumeDrawBuffer[BoundingVolumeDrawBufferIndex].BoundingVolumeDraws[drawItemIndex];
    BoundingVolume boundingVolume = boundingVolumeBuffer[draw.boundingVolumeItemLocation.storeIndex].boundingVolumes[draw.boundingVolumeItemLocation.itemIndex];
    Spatial spatial = spatialBuffer[boundingVolume.spatialItemLocation.storeIndex].spatials[boundingVolume.spatialItemLocation.itemIndex];
    Camera camera = cameraBuffer[draw.cameraItemLocation.storeIndex].cameras[draw.cameraItemLocation.itemIndex];

    gl_Position = camera.perspective * camera.view * spatial.absoluteModel * inPosition;

    drawItemLocation = StoreItemLocation(BoundingVolumeDrawBufferIndex, drawItemIndex);

    fragment.position = spatial.absoluteModel * inPosition;
    fragment.color = inColor;
}