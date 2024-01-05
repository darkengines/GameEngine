#version 460
#extension GL_EXT_nonuniform_qualifier: enable
#extension GL_KHR_vulkan_glsl: enable

#include "BoundingVolumeDraw.glsl"
#include "Vertex.glsl"
#include "AxisAlignedBoundingBox.glsl"

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
layout (set = 3, binding = 0) readonly buffer spatialLayout {
    Spatial[] spatials;
} spatialBuffer[];
layout (set = 3, binding = 0) readonly buffer cameraLayout {
    Camera[] cameras;
} cameraBuffer[];
layout (set = 3, binding = 0) readonly buffer axisAlignedBoundingBoxLayout {
    AxisAlignedBoundingBox[] axisAlignedBoundingBoxes;
} axisAlignedBoundingBoxBuffer[];

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec4 inColor;

layout(location = 0) out Vertex fragment;
layout(location = 2) flat out StoreItemLocation drawItemLocation;

void main() {
    uint BoundingVolumeDrawBufferIndex = gl_InstanceIndex / 131072u;
    uint drawItemIndex = gl_InstanceIndex % 131072u;
    BoundingVolumeDraw draw = BoundingVolumeDrawBuffer[BoundingVolumeDrawBufferIndex].BoundingVolumeDraws[drawItemIndex];
    AxisAlignedBoundingBox axisAlignedBoundingBox = axisAlignedBoundingBoxBuffer[draw.boundingVolumeItemLocation.storeIndex].axisAlignedBoundingBoxes[draw.boundingVolumeItemLocation.itemIndex];
    Camera camera = cameraBuffer[draw.cameraItemLocation.storeIndex].cameras[draw.cameraItemLocation.itemIndex];

    gl_Position = camera.perspective * camera.view * (inPosition * axisAlignedBoundingBox.extent + axisAlignedBoundingBox.center);

    drawItemLocation = StoreItemLocation(BoundingVolumeDrawBufferIndex, drawItemIndex);

    fragment.position = gl_Position;
    fragment.color = inColor;
}