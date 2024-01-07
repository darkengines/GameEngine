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
#include "FrustumVertex.glsl"

layout (set = 1, binding = 0) readonly buffer drawLayout {
    FrustumDraw[] frustumDraws;
} frustumDrawBuffer[];
layout(set = 2, binding = 0) uniform globalLayout {
    Global global;
} globalBuffer;
layout (set = 3, binding = 0) readonly buffer spatialLayout {
    Spatial[] spatials;
} spatialBuffer[];
layout (set = 3, binding = 0) readonly buffer cameraLayout {
    Camera[] cameras;
} cameraBuffer[];
layout (set = 3, binding = 0) readonly buffer frustumLayout {
    Frustum[] frustum;
} frustumBuffer[];

layout(location = 0) in uint inPosition;
layout(location = 1) in vec4 inColor;

layout(location = 0) out FragmentVertex fragment;
layout(location = 2) flat out StoreItemLocation drawItemLocation;

void main() {
    uint frustumDrawBufferIndex = gl_InstanceIndex / 131072u;
    uint drawItemIndex = gl_InstanceIndex % 131072u;
    FrustumDraw draw = frustumDrawBuffer[frustumDrawBufferIndex].frustumDraws[drawItemIndex];
    Frustum frustum = frustumBuffer[draw.frustumItemLocation.storeIndex].frustum[draw.frustumItemLocation.itemIndex];
    Camera camera = cameraBuffer[draw.cameraItemLocation.storeIndex].cameras[draw.cameraItemLocation.itemIndex];

    vec4 position;

    switch (inPosition) {
        case (Near | Top | Left): {
            position = frustum.leftTopNear;
            break;   
        }
        case (Near | Top | Right): {
            position = frustum.rightTopNear;
            break;
        }
        case (Near | Bottom | Left): {
            position = frustum.leftBottomNear;
            break;
        }
        case (Near | Bottom | Right): {
            position = frustum.rightBottomNear;
            break;
        }
        case (Far | Top | Left): {
            position = frustum.leftTopFar;
            break;   
        }
        case (Far | Top | Right): {
            position = frustum.rightTopFar;
            break;
        }
        case (Far | Bottom | Left): {
            position = frustum.leftBottomFar;
            break;
        }
        case (Far | Bottom | Right): {
            gl_Position = frustum.rightBottomFar;
            break;
        }
    }
    gl_Position = camera.perspective * camera.view * position;

    drawItemLocation = StoreItemLocation(frustumDrawBufferIndex, drawItemIndex);

    fragment.position = position;
    fragment.color = inColor;
}