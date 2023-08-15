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

layout (set = 0, binding = 0) uniform sampler2D textures[];
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

layout(location = 0) in MeshVertex point;
layout(location = 10) flat in StoreItemLocation drawItemLocation;

layout(location = 0) out vec4 outColor;

void main() {

    MeshDraw draw = meshDrawBuffer[drawItemLocation.storeIndex].meshDraws[drawItemLocation.itemIndex];
    Mesh mesh = meshBuffer[draw.meshItemLocation.storeIndex].meshes[draw.meshItemLocation.itemIndex];
    Object object = objectBuffer[draw.objectItemLocation.storeIndex].objects[draw.objectItemLocation.itemIndex];
    Material material = materialBuffer[mesh.materialItemLocation.storeIndex].materials[mesh.materialItemLocation.itemIndex];
    Spatial spatial = spatialBuffer[object.spatialItemLocation.storeIndex].spatials[object.spatialItemLocation.itemIndex];
    Camera camera = cameraBuffer[globalBuffer.global.cameraItemLocation.storeIndex].cameras[globalBuffer.global.cameraItemLocation.itemIndex];

    vec4 normal = point.normal;
    if (material.hasNormalMap) {
        vec4 normalMap = vec4(texture(textures[material.normalMapIndex], point.texCoord).xyz, 0) * 2.0f - 1.0;
        normal = vec4(normalize(mat3(point.TBN) * normalMap.xyz), 0);
    }

    float roughness = 0.0;
    float metallic = 0.0;

    if (material.hasMetallicRoughnessTexture) {
        vec4 metallicRoughnessMap = vec4(texture(textures[material.metallicRoughnessTextureIndex], point.texCoord).xyz, 0);
        roughness = metallicRoughnessMap.g;
        metallic = metallicRoughnessMap.r;
    }

    vec3 viewDirection = normalize(camera.absolutePosition.xyz - point.position.xyz);

    vec3 color = vec3(0,0,0);
    vec4 albedo = material.diffuseColor;
    if (material.hasDiffuseColorTexture) {
        albedo = texture(textures[material.diffuseColorTextureIndex], point.texCoord);
    }

    if (albedo.a < 0.33f) {
        discard;
    }
    outColor = albedo;
}