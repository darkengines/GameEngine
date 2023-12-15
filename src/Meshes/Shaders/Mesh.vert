#version 460
#extension GL_EXT_nonuniform_qualifier: enable
#extension GL_KHR_vulkan_glsl: enable

#include "MeshDraw.glsl"
#include "MeshVertex.glsl"
#include "Mesh.glsl"

#include "../../Graphics/Shaders/StoreItemLocation.glsl"
#include "../../Graphics/Shaders/Global.glsl"
#include "../../Materials/shaders/Material.glsl"
#include "../../Spatials/shaders/Spatial.glsl"
#include "../../Objects/shaders/Object.glsl"
#include "../../Cameras/shaders/Camera.glsl"
#include "../../Lights/Shaders/PointLight.glsl"
#include "../../Lights/Shaders/DirectionalLight.glsl"
#include "../../Lights/Shaders/Spotlight.glsl"

layout (set = 1, binding = 0) readonly buffer drawLayout {
    MeshDraw[] meshDraws;
} meshDrawBuffer[];
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
layout (set = 3, binding = 0) readonly buffer pointLightLayout {
    PointLight[] pointLights;
} pointLightBuffer[];
layout (set = 3, binding = 0) readonly buffer directionalLightLayout {
    DirectionalLight[] directionalLights;
} directionalLightBuffer[];
layout (set = 3, binding = 0) readonly buffer spotlightLayout {
    Spotlight[] spotlights;
} spotlightBuffer[];

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec4 inNormal;
layout(location = 2) in vec4 inTangent;
layout(location = 3) in vec4 inBitangent;
layout(location = 4) in vec4 inColor;
layout(location = 5) in vec2 inTexCoord;
layout(location = 0) out MeshVertex fragment;
layout(location = 10) flat out StoreItemLocation drawItemLocation;

void main() {
    uint meshDrawBufferIndex = gl_InstanceIndex / 131072u;
    uint drawItemIndex = gl_InstanceIndex % 131072u;
    MeshDraw draw = meshDrawBuffer[meshDrawBufferIndex].meshDraws[drawItemIndex];
    Mesh mesh = meshBuffer[draw.meshItemLocation.storeIndex].meshes[draw.meshItemLocation.itemIndex];
    Object object = objectBuffer[draw.objectItemLocation.storeIndex].objects[draw.objectItemLocation.itemIndex];
    Material material = materialBuffer[mesh.materialItemLocation.storeIndex].materials[mesh.materialItemLocation.itemIndex];
    Spatial spatial = spatialBuffer[object.spatialItemLocation.storeIndex].spatials[object.spatialItemLocation.itemIndex];
    Camera camera = cameraBuffer[globalBuffer.global.cameraStoreIndex].cameras[globalBuffer.global.cameraItemIndex];

    gl_Position = camera.perspective * camera.view * spatial.absoluteModel * inPosition;

    drawItemLocation = StoreItemLocation(meshDrawBufferIndex, drawItemIndex);

    fragment.position = spatial.absoluteModel * inPosition;
    fragment.color = inColor;
    fragment.texCoord = inTexCoord;

    vec4 T = vec4(normalize(mat3(spatial.absoluteModel) * inTangent.xyz), 0);
    vec4 N = vec4(normalize(mat3(spatial.absoluteModel) * inNormal.xyz), 0);
    vec4 B = vec4(normalize(mat3(spatial.absoluteModel) * inBitangent.xyz), 0);

    fragment.tangent = T;
    fragment.bitangent = B;
    fragment.normal = N;

    fragment.TBN = mat4(T, B, N, 0, 0, 0, 0);
}