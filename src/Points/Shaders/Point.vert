#version 460
#extension GL_EXT_nonuniform_qualifier: enable

#include "PointDraw.glsl"
#include "PointVertex.glsl"
#include "Point.glsl"

#include "../../Graphics/Shaders/StoreItemLocation.glsl"
#include "../../Graphics/Shaders/Global.glsl"
#include "../../Materials/shaders/Material.glsl"
#include "../../Spatials/shaders/Spatial.glsl"
#include "../../Nodes/shaders/Node.glsl"
#include "../../Cameras/shaders/Camera.glsl"

layout (set = 1, binding = 0) readonly buffer drawLayout {
    PointDraw[] pointDraws;
} pointDrawBuffer[];
layout(set = 2, binding = 0) uniform globalLayout {
    Global global;
} globalBuffer;
layout (set = 3, binding = 0) readonly buffer materialLayout {
    Material[] materials;
} materialBuffer[];
layout (set = 3, binding = 0) readonly buffer pointLayout {
    Point[] points;
} pointBuffer[];
layout (set = 3, binding = 0) readonly buffer spatialLayout {
    Spatial[] spatials;
} spatialBuffer[];
layout (set = 3, binding = 0) readonly buffer nodeLayout {
    Node[] nodes;
} nodeBuffer[];
layout (set = 3, binding = 0) readonly buffer cameraLayout {
    Camera[] cameras;
} cameraBuffer[];

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out PointVertex fragment;
layout(location = 10) flat out StoreItemLocation drawItemLocation;

void main() {
    uint pointDrawBufferIndex = gl_InstanceIndex / 131072u;
    uint drawItemIndex = gl_InstanceIndex % 131072u;
    PointDraw draw = pointDrawBuffer[pointDrawBufferIndex].pointDraws[drawItemIndex];
    Point point = pointBuffer[draw.pointItemLocation.storeIndex].points[draw.pointItemLocation.itemIndex];
    Node node = nodeBuffer[draw.nodeItemLocation.storeIndex].nodes[draw.nodeItemLocation.itemIndex];
    Material material = materialBuffer[point.materialItemLocation.storeIndex].materials[point.materialItemLocation.itemIndex];
    Spatial spatial = spatialBuffer[node.spatialItemLocation.storeIndex].spatials[node.spatialItemLocation.itemIndex];
    Camera camera = cameraBuffer[globalBuffer.global.cameraStoreIndex].cameras[globalBuffer.global.cameraItemIndex];

    gl_Position = camera.perspective * camera.view * spatial.model * inPosition;
    gl_PointSize = 1.0 / gl_Position.z;

    drawItemLocation = StoreItemLocation(pointDrawBufferIndex, drawItemIndex);

    fragment.position = spatial.model * inPosition;
    fragment.color = inColor;
    fragment.texCoord = inTexCoord;
}