#version 460
#extension GL_EXT_nonuniform_qualifier: enable

#include "Draw.glsl"
#include "Global.glsl"
#include "Point.glsl"
#include "StoreItemLocation.glsl"

#include "../../Materials/shaders/Material.glsl"
#include "../../Meshes/shaders/Mesh.glsl"
#include "../../Spatials/shaders/Spatial.glsl"
#include "../../Objects/shaders/Object.glsl"
#include "../../Cameras/shaders/Camera.glsl"

layout (set = 1, binding = 0) readonly buffer drawLayout {
    Draw[] draws;
} drawBuffer[];
layout(set = 2, binding = 0) uniform globalLayout {
    Global global;
} globalBuffer;
layout (set = 3, binding = 0) readonly buffer materialLayout {
    Material[] materials;
} materialBuffer[];
layout (set = 3, binding = 0) readonly buffer meshLayout {
    Mesh[] meshes;
} meshBuffer[];
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
layout(location = 1) in vec4 inNormal;
layout(location = 2) in vec4 inTangent;
layout(location = 3) in vec4 inBitangent;
layout(location = 4) in vec4 inColor;
layout(location = 5) in vec2 inTexCoord;

layout(location = 0) out Point point;
layout(location = 10) flat out StoreItemLocation drawItemLocation;

void main() {
    uint drawBufferIndex = gl_InstanceIndex / 1024u;
    uint drawItemIndex = gl_InstanceIndex % 1024u;
    Draw draw = drawBuffer[drawBufferIndex].draws[drawItemIndex];
    Mesh mesh = meshBuffer[draw.meshItemLocation.storeIndex].meshes[draw.meshItemLocation.itemIndex];
    Object object = objectBuffer[draw.objectItemLocation.storeIndex].objects[draw.objectItemLocation.itemIndex];
    Material material = materialBuffer[mesh.materialItemLocation.storeIndex].materials[mesh.materialItemLocation.itemIndex];
    Spatial spatial = spatialBuffer[object.spatialItemLocation.storeIndex].spatials[object.spatialItemLocation.itemIndex];
    Camera camera = cameraBuffer[globalBuffer.global.cameraItemLocation.storeIndex].cameras[globalBuffer.global.cameraItemLocation.itemIndex];

    gl_Position = camera.perspective * camera.view * spatial.absoluteModel * inPosition;

    drawItemLocation = StoreItemLocation(drawBufferIndex, drawItemIndex);

    point.position = spatial.absoluteModel * inPosition;
    point.color = inColor;
    point.texCoord = inTexCoord;

    vec4 T = vec4(normalize(mat3(spatial.absoluteModel) * inTangent.xyz), 0);
    vec4 N = vec4(normalize(mat3(spatial.absoluteModel) * inNormal.xyz), 0);
    vec4 B = vec4(normalize(mat3(spatial.absoluteModel) * inBitangent.xyz), 0);

    point.tangent = T;
    point.bitangent = B;
    point.normal = N;

    point.TBN = mat4(T, B, N, 0, 0, 0, 0);
}