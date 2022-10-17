#version 460
#extension GL_EXT_nonuniform_qualifier: enable

#include "Draw.glsl"
#include "../../Materials/shaders/Material.glsl"
#include "../../Meshes/shaders/Mesh.glsl"
#include "../../Spatials/shaders/Spatial.glsl"
#include "../../Objects/shaders/Object.glsl"
#include "../../Cameras/shaders/Camera.glsl"

layout (set = 1, binding = 0) readonly buffer materialLayout {
    Material[] materials;
} materialBuffer[];
layout (set = 1, binding = 0) readonly buffer meshLayout {
    Mesh[] meshes;
} meshBuffer[];
layout (set = 1, binding = 0) readonly buffer spatialLayout {
    Spatial[] spatials;
} spatialBuffer[];
layout (set = 1, binding = 0) readonly buffer objectLayout {
    Object[] objects;
} objectBuffer[];
layout (set = 1, binding = 0) readonly buffer cameraLayout {
    Object[] cameras;
} cameraBuffer[];
layout (set = 2, binding = 0) readonly buffer drawLayout {
    Draw[] draws;
} drawBuffer[];

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec4 inNormal;
layout(location = 2) in vec4 inTangent;
layout(location = 3) in vec4 inBitangent;
layout(location = 4) in vec4 inColor;
layout(location = 5) in vec2 inTexCoord;

void main() {
    uint drawBufferIndex = gl_InstanceIndex / 1024u;
    uint drawItemIndex = gl_InstanceIndex % 1024u;
    Draw draw = drawBuffer[drawBufferIndex].draws[drawItemIndex];
    Mesh mesh = meshBuffer[draw.meshItemLocation.storeIndex].meshes[draw.meshItemLocation.itemIndex];
    Object object = objectBuffer[draw.objectItemLocation.storeIndex].objects[draw.objectItemLocation.itemIndex];
    Material material = materialBuffer[mesh.materialItemLocation.storeIndex].materials[mesh.materialItemLocation.itemIndex];
    Spatial spatial = spatialBuffer[object.spatialItemLocation.storeIndex].spatials[object.spatialItemLocation.itemIndex];

    gl_Position = inPosition;
}