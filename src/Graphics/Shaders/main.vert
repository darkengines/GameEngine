#version 460
#extension GL_EXT_nonuniform_qualifier: enable

#include "Draw.glsl"
#include "../../Materials/shaders/Material.glsl"
#include "../../Meshes/shaders/Mesh.glsl"
#include "../../Spatials/shaders/Spatial.glsl"
#include "../../Objects/shaders/Object.glsl"

layout (std140, set = 1, binding = 0) readonly buffer drawLayout {
    Draw[] draws;
} drawBuffer;
layout (std140, set = 2, binding = 0) readonly buffer materialLayout {
    Material[] materials;
} materialBuffer[];
layout (std140, set = 2, binding = 0) readonly buffer meshLayout {
    Mesh[] meshes;
} meshBuffer[];
layout (std140, set = 2, binding = 0) readonly buffer spatialLayout {
    Spatial[] spatials;
} spatialBuffer[];
layout (std140, set = 2, binding = 0) readonly buffer objectLayout {
    Object[] objects;
} objectBuffer[];

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec4 inNormal;
layout(location = 2) in vec4 inTangent;
layout(location = 3) in vec4 inBitangent;
layout(location = 4) in vec4 inColor;
layout(location = 5) in vec2 inTexCoord;

void main() {
    Draw draw = drawBuffer.draws[gl_InstanceIndex];
    Object object = objectBuffer[draw.objectItemLocation.storeIndex].objects[draw.objectItemLocation.itemIndex];
    gl_Position = inPosition;
}