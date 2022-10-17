#version 460
#extension GL_EXT_nonuniform_qualifier: enable

#include "Draw.glsl"
#include "../../Materials/shaders/Material.glsl"
#include "../../Meshes/shaders/Mesh.glsl"
#include "../../Spatials/shaders/Spatial.glsl"
#include "../../Objects/shaders/Object.glsl"
#include "../../Cameras/shaders/Camera.glsl"

layout (set = 0, binding = 0) uniform sampler2D textures[];
layout (std140, set = 1, binding = 0) readonly buffer materialLayout {
    Material[] materials;
} materialBuffer;
layout (std140, set = 1, binding = 0) readonly buffer meshLayout {
    Mesh[] meshes;
} meshBuffer;
layout (std140, set = 1, binding = 0) readonly buffer spatialLayout {
    Spatial[] spatials;
} spatialBuffer;
layout (std140, set = 1, binding = 0) readonly buffer objectLayout {
    Object[] objects;
} objectBuffer;
layout (std140, set = 1, binding = 0) readonly buffer cameraLayout {
    Object[] cameras;
} cameraBuffer[];
layout (std140, set = 2, binding = 0) readonly buffer drawLayout {
    Draw[] draws;
} drawBuffer[];

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(0, 1, 0, 1);
}