#version 460
#extension GL_EXT_nonuniform_qualifier: enable
#extension GL_KHR_vulkan_glsl: enable

#include "PointDraw.glsl"
#include "PointVertex.glsl"
#include "Point.glsl"

#include "../../Graphics/Shaders/StoreItemLocation.glsl"
#include "../../Graphics/Shaders/Global.glsl"
#include "../../Materials/shaders/Material.glsl"
#include "../../Spatials/shaders/Spatial.glsl"
#include "../../Objects/shaders/Object.glsl"
#include "../../Cameras/shaders/Camera.glsl"

layout (set = 0, binding = 0) uniform sampler2D textures[];
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
layout (set = 3, binding = 0) readonly buffer objectLayout {
    Object[] objects;
} objectBuffer[];
layout (set = 3, binding = 0) readonly buffer cameraLayout {
    Camera[] cameras;
} cameraBuffer[];

layout(location = 0) in PointVertex fragment;
layout(location = 10) flat in StoreItemLocation drawItemLocation;

layout(location = 0) out vec4 outColor;

void main() {

    PointDraw draw = pointDrawBuffer[drawItemLocation.storeIndex].pointDraws[drawItemLocation.itemIndex];
    Point point = pointBuffer[draw.pointItemLocation.storeIndex].points[draw.pointItemLocation.itemIndex];
    Object object = objectBuffer[draw.objectItemLocation.storeIndex].objects[draw.objectItemLocation.itemIndex];
    Material material = materialBuffer[point.materialItemLocation.storeIndex].materials[point.materialItemLocation.itemIndex];
    Spatial spatial = spatialBuffer[object.spatialItemLocation.storeIndex].spatials[object.spatialItemLocation.itemIndex];
    Camera camera = cameraBuffer[globalBuffer.global.cameraItemLocation.storeIndex].cameras[globalBuffer.global.cameraItemLocation.itemIndex];

    vec4 normal = fragment.normal;
    if (material.hasNormalMap) {
        vec4 normalMap = vec4(texture(textures[material.normalMapIndex], fragment.texCoord).xyz, 0) * 2.0f - 1.0;
        normal = vec4(normalize(mat3(fragment.TBN) * normalMap.xyz), 0);
    }

    float roughness = 0.0;
    float metallic = 0.0;

    if (material.hasMetallicRoughnessTexture) {
        vec4 metallicRoughnessMap = vec4(texture(textures[material.metallicRoughnessTextureIndex], fragment.texCoord).xyz, 0);
        roughness = metallicRoughnessMap.g;
        metallic = metallicRoughnessMap.r;
    }

    vec3 viewDirection = normalize(camera.absolutePosition.xyz - fragment.position.xyz);

    vec3 color = vec3(0,0,0);
    vec4 albedo = material.diffuseColor;
    if (material.hasDiffuseColorTexture) {
        albedo = texture(textures[material.diffuseColorTextureIndex], fragment.texCoord);
    }

    if (albedo.a < 0.33f) {
        discard;
    }
    outColor = albedo;
}