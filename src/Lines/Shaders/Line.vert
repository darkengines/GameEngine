#version 460
#extension GL_EXT_nonuniform_qualifier: enable

#include "LineDraw.glsl"
#include "LineVertex.glsl"
#include "Line.glsl"

#include "../../Graphics/Shaders/StoreItemLocation.glsl"
#include "../../Graphics/Shaders/Global.glsl"
#include "../../Materials/shaders/Material.glsl"
#include "../../Spatials/shaders/Spatial.glsl"
#include "../../Objects/shaders/Object.glsl"
#include "../../Cameras/shaders/Camera.glsl"

layout (set = 1, binding = 0) readonly buffer drawLayout {
    LineDraw[] lineDraws;
} lineDrawBuffer[];
layout(set = 2, binding = 0) uniform globalLayout {
    Global global;
} globalBuffer;
layout (set = 3, binding = 0) readonly buffer materialLayout {
    Material[] materials;
} materialBuffer[];
layout (set = 3, binding = 0) readonly buffer lineLayout {
    Line[] lines;
} lineBuffer[];
layout (set = 3, binding = 0) readonly buffer spatialLayout {
    Spatial[] spatials;
} spatialBuffer[];
layout (set = 3, binding = 0) readonly buffer objectLayout {
    Object[] objects;
} objectBuffer[];
layout (set = 3, binding = 0) readonly buffer cameraLayout {
    Camera[] cameras;
} cameraBuffer[];

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out LineVertex fragment;
layout(location = 10) flat out StoreItemLocation drawItemLocation;

void main() {
    uint lineDrawBufferIndex = gl_InstanceIndex / 131072u;
    uint drawItemIndex = gl_InstanceIndex % 131072u;
    LineDraw draw = lineDrawBuffer[lineDrawBufferIndex].lineDraws[drawItemIndex];
    Line line = lineBuffer[draw.lineItemLocation.storeIndex].lines[draw.lineItemLocation.itemIndex];
    Object object = objectBuffer[draw.objectItemLocation.storeIndex].objects[draw.objectItemLocation.itemIndex];
    Material material = materialBuffer[line.materialItemLocation.storeIndex].materials[line.materialItemLocation.itemIndex];
    Spatial spatial = spatialBuffer[object.spatialItemLocation.storeIndex].spatials[object.spatialItemLocation.itemIndex];
    Camera camera = cameraBuffer[globalBuffer.global.cameraStoreIndex].cameras[globalBuffer.global.cameraItemIndex];

    gl_Position = camera.perspective * camera.view * spatial.model * inPosition;

    drawItemLocation = StoreItemLocation(lineDrawBufferIndex, drawItemIndex);

    fragment.position = spatial.model * inPosition;
    fragment.color = inColor;
    fragment.texCoord = inTexCoord;
}