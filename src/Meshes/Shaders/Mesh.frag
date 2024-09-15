#version 460
#extension GL_EXT_nonuniform_qualifier: enable

const float PI = 3.14159265359;

#include "MeshDraw.glsl"
#include "MeshVertex.glsl"
#include "Mesh.glsl"

#include "../../Graphics/Shaders/StoreItemLocation.glsl"
#include "../../Graphics/Shaders/Global.glsl"
#include "../../Materials/shaders/Material.glsl"
#include "../../Spatials/shaders/Spatial.glsl"
#include "../../Nodes/shaders/Node.glsl"
#include "../../Cameras/shaders/Camera.glsl"
#include "../../Lights/Shaders/Light.glsl"
#include "../../Lights/Shaders/PointLight.glsl"
#include "../../Lights/Shaders/DirectionalLight.glsl"
#include "../../Lights/Shaders/Spotlight.glsl"
#include "../../Lights/Shaders/Lights.glsl"
#include "../../Lights/Shaders/LightPerspective.glsl"

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
layout (set = 3, binding = 0) readonly buffer nodeLayout {
    Node[] nodes;
} nodeBuffer[];
layout (set = 3, binding = 0) readonly buffer cameraLayout {
    Camera[] cameras;
} cameraBuffer[];
layout (std430, set = 3, binding = 0) readonly buffer lightLayout {
    Light[] lights;
} lightBuffer[];
layout (std430, set = 3, binding = 0) readonly buffer pointLightLayout {
    PointLight[] pointLights;
} pointLightBuffer[];
layout (set = 3, binding = 0) readonly buffer directionalLightLayout {
    DirectionalLight[] directionalLights;
} directionalLightBuffer[];
layout (std430, set = 3, binding = 0) readonly buffer spotlightLayout {
    Spotlight[] spotlights;
} spotlightBuffer[];
layout (std430, set = 3, binding = 0) readonly buffer lightPerspectiveLayout {
    LightPerspective[] lightPerspectives;
} lightPerspectiveBuffer[];

layout(location = 0) in MeshVertex point;
layout(location = 10) flat in StoreItemLocation drawItemLocation;

layout(location = 0) out vec4 outColor;

float sampleCube(vec3 lightToNode, PointLight pointLight, out LightPerspective lightPerspective) {
    vec3 lightToNodeMagnitude = abs(lightToNode);
    vec4 shadowMapRect;
    vec2 shadowMapUv;

    if (lightToNodeMagnitude.x > lightToNodeMagnitude.y && lightToNodeMagnitude.x > lightToNodeMagnitude.z) {
        if (lightToNode.x < 0) {
            lightPerspective = lightPerspectiveBuffer[pointLight.leftLightPerspectiveStoreItemLocation.storeIndex]
            .lightPerspectives[pointLight.leftLightPerspectiveStoreItemLocation.itemIndex];
            shadowMapRect = lightPerspective.shadowMapRect;
            shadowMapUv.x = ((-lightToNode.z / lightToNodeMagnitude.x) + 1) * 0.5f;
            shadowMapUv.y = ((-lightToNode.y / lightToNodeMagnitude.x) + 1) * 0.5f;
        } else {
            lightPerspective = lightPerspectiveBuffer[pointLight.rightLightPerspectiveStoreItemLocation.storeIndex]
            .lightPerspectives[pointLight.rightLightPerspectiveStoreItemLocation.itemIndex];
            shadowMapRect = lightPerspective.shadowMapRect;
            shadowMapUv.x = ((lightToNode.z / lightToNodeMagnitude.x) + 1) * 0.5f;
            shadowMapUv.y = ((-lightToNode.y / lightToNodeMagnitude.x) + 1) * 0.5f;
        }
    } else {
        if (lightToNodeMagnitude.y > lightToNodeMagnitude.z) {
            if (lightToNode.y < 0) {
                lightPerspective = lightPerspectiveBuffer[pointLight.downLightPerspectiveStoreItemLocation.storeIndex]
                .lightPerspectives[pointLight.downLightPerspectiveStoreItemLocation.itemIndex];
                shadowMapRect = lightPerspective.shadowMapRect;
                shadowMapUv.x = ((-lightToNode.x / lightToNodeMagnitude.y) + 1) * 0.5f;
                shadowMapUv.y = ((-lightToNode.z / lightToNodeMagnitude.y) + 1) * 0.5f;
            } else {
                lightPerspective = lightPerspectiveBuffer[pointLight.topLightPerspectiveStoreItemLocation.storeIndex]
                .lightPerspectives[pointLight.topLightPerspectiveStoreItemLocation.itemIndex];
                shadowMapRect = lightPerspective.shadowMapRect;
                shadowMapUv.x = ((-lightToNode.x / lightToNodeMagnitude.y) + 1) * 0.5f;
                shadowMapUv.y = ((lightToNode.z / lightToNodeMagnitude.y) + 1) * 0.5f;
            }
        } else {
            if (lightToNode.z < 0) {
                lightPerspective = lightPerspectiveBuffer[pointLight.backLightPerspectiveStoreItemLocation.storeIndex]
                .lightPerspectives[pointLight.backLightPerspectiveStoreItemLocation.itemIndex];
                shadowMapRect = lightPerspective.shadowMapRect;
                shadowMapUv.x = ((lightToNode.x / lightToNodeMagnitude.z) + 1) * 0.5f;
                shadowMapUv.y = ((-lightToNode.y / lightToNodeMagnitude.z) + 1) * 0.5f;
            } else {
                lightPerspective = lightPerspectiveBuffer[pointLight.frontLightPerspectiveStoreItemLocation.storeIndex]
                .lightPerspectives[pointLight.frontLightPerspectiveStoreItemLocation.itemIndex];
                shadowMapRect = lightPerspective.shadowMapRect;
                shadowMapUv.x = ((-lightToNode.x / lightToNodeMagnitude.z) + 1) * 0.5f;
                shadowMapUv.y = ((-lightToNode.y / lightToNodeMagnitude.z) + 1) * 0.5f;
            }
        }
    }
    shadowMapUv.x = shadowMapRect.x + clamp(shadowMapUv.x * shadowMapRect.z, 0, shadowMapRect.z);
    shadowMapUv.y = shadowMapRect.y + clamp(shadowMapUv.y * shadowMapRect.w, 0, shadowMapRect.w);

    return texture(textures[0], shadowMapUv).r;
}

vec3 sampleOffsetDirections[20] = vec3[]
(
vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
);

float delinearize_depth(float d, float zNear, float zFar)
{
    return d * (zFar - zNear) + zNear;
}

float samplePointLightShadow(vec3 lightToNode, PointLight pointLight, float diskRadius, vec3 normal, vec3 lightDirection) {
    float shadow = 0.0;
    int samples  = 20;
    float d = length(lightToNode);
    float closestDepth;
    LightPerspective lightPerspective;
    for (int i = 0; i < samples; ++i)
    {
        float bias = max(0.1 * (1.0 - dot(point.normal.xyz, normalize(lightDirection + sampleOffsetDirections[i] * diskRadius))), 0.5);
        float closestDepth = sampleCube(lightToNode + sampleOffsetDirections[i] * diskRadius, pointLight, lightPerspective);
        closestDepth = delinearize_depth(closestDepth, lightPerspective.near, lightPerspective.far);

        if (d - bias > closestDepth)
        shadow += 1.0;
    }
    shadow /= float(samples);

    return shadow;
}

float sampleSpotlightShadow(
vec3 lightToNode,
Spotlight spotlight,
float diskRadius,
LightPerspective lightPerspective
) {
    float shadow = 0.0f;
    int samples  = 20;
    float uDenominator = cos(PI/2.0f - spotlight.outerConeAngle);
    float vDenominator = sin(spotlight.outerConeAngle);
    vec3 lightDirection = normalize(lightToNode);
    float fragmentLightDistance = length(lightToNode);

    for (int i = 0; i < samples; ++i)
    {
        vec3 sampleLightDirection = normalize(lightDirection + sampleOffsetDirections[i] * diskRadius);
        float bias = max(0.1 * (1.0 - dot(point.normal.xyz, -sampleLightDirection)), 0.5);
        vec2 preShadowMapUv = (vec2(
        -dot(sampleLightDirection, cross(lightPerspective.absoluteFront.xyz, lightPerspective.absoluteUp.xyz)) / uDenominator,
        dot(sampleLightDirection, lightPerspective.absoluteUp.xyz) / vDenominator
        ) + 1.0f) / 2.0f;
        vec2 shadowMapUv;
        shadowMapUv.x = lightPerspective.shadowMapRect.x + preShadowMapUv.x * lightPerspective.shadowMapRect.z;
        shadowMapUv.y = lightPerspective.shadowMapRect.y + preShadowMapUv.y * lightPerspective.shadowMapRect.w;
        float closestDepth = texture(textures[0], shadowMapUv).r;
        closestDepth = delinearize_depth(closestDepth, lightPerspective.near, lightPerspective.far);

        if (fragmentLightDistance - bias > closestDepth) {
            shadow += 1.0f;
        }
    }
    shadow /= float(samples);

    return shadow;
}

vec3 hashFloatToRgb(float value) {
    // Create pseudo-random values using sin() and fract() functions.
    // Multiplying by large constants helps in generating randomness.
    float x = fract(sin(value * 12.9898) * 43758.5453);
    float y = fract(sin(value * 78.233) * 43758.5453);
    float z = fract(sin(value * 93.233) * 43758.5453);

    // Construct an RGB color from the random values
    return vec3(x, y, z);
}

void main() {

    MeshDraw draw = meshDrawBuffer[drawItemLocation.storeIndex].meshDraws[drawItemLocation.itemIndex];
    Mesh mesh = meshBuffer[draw.meshItemLocation.storeIndex].meshes[draw.meshItemLocation.itemIndex];
    Node node = nodeBuffer[draw.nodeItemLocation.storeIndex].nodes[draw.nodeItemLocation.itemIndex];
    Material material = materialBuffer[mesh.materialItemLocation.storeIndex].materials[mesh.materialItemLocation.itemIndex];

    if (globalBuffer.global.renderStyle == uv_style) {
        outColor = vec4(point.texCoord.x, point.texCoord.y, 0, 1);
        return;
    }

    if (globalBuffer.global.renderStyle == material_style) {
        outColor = vec4(hashFloatToRgb(drawItemLocation.itemIndex), 1.f);
        return;
    }
    Spatial spatial = spatialBuffer[node.spatialItemLocation.storeIndex].spatials[node.spatialItemLocation.itemIndex];
    Camera camera = cameraBuffer[draw.cameraItemLocation.storeIndex].cameras[draw.cameraItemLocation.itemIndex];
    Spatial cameraAbsoluteSpatial = spatialBuffer[camera.spatialItemLocation.storeIndex].spatials[camera.spatialItemLocation.itemIndex];

    vec4 normal = point.normal;
    if (material.hasNormalMap) {
        vec4 normalMap = vec4(texture(textures[material.normalMapIndex], point.texCoord).xyz, 0) * 2.0f - 1.0;
        normal = vec4(normalize(mat3(point.TBN) * normalMap.xyz), 0);
    }

    if (globalBuffer.global.renderStyle == normal_style) {
        outColor = normal;
        outColor.a = 1.0f;
        return;
    }

    float roughness = 0.0;
    float metallic = 0.0;

    if (material.hasMetallicRoughnessTexture) {
        vec4 metallicRoughnessMap = vec4(texture(textures[material.metallicRoughnessTextureIndex], point.texCoord).xyz, 0);
        roughness = metallicRoughnessMap.g;
        metallic = metallicRoughnessMap.b;
    }
    if (material.hasMetallicTexture) {
        metallic = texture(textures[material.metallicTextureIndex], point.texCoord).b;
    }
    if (material.hasRoughnessTexture) {
        roughness = texture(textures[material.roughnessTextureIndex], point.texCoord).g;
    }

    vec3 viewDirection = normalize(cameraAbsoluteSpatial.position.xyz - point.position.xyz);

    vec3 color = vec3(0, 0, 0);
    vec4 albedo = material.diffuseColor;
    if (material.hasDiffuseColorTexture) {
        albedo = texture(textures[material.diffuseColorTextureIndex], point.texCoord);
    }

    if (globalBuffer.global.renderStyle == albedo_style) {
        outColor = albedo;
        return;
    }

    for (uint pointLightIndex = 0;pointLightIndex < globalBuffer.global.pointLightCount; pointLightIndex++) {
        PointLight pointLight = pointLightBuffer[globalBuffer.global.pointLightArrayIndex].pointLights[pointLightIndex];
        Light light = lightBuffer[pointLight.lightStoreItemLocation.storeIndex].lights[pointLight.lightStoreItemLocation.itemIndex];
        Spatial pointLightSpatial = spatialBuffer[pointLight.spatialStoreItemLocation.storeIndex].spatials[pointLight.spatialStoreItemLocation.itemIndex];

        vec3 lightPosition = pointLightSpatial.position.xyz;
        float fragmentLightDistance = distance(lightPosition, point.position.xyz);
        vec3 lightDirection = normalize(lightPosition - point.position.xyz);
        float attenuation = 1.0 / (pointLight.constantAttenuation + pointLight.linearAttenuation * fragmentLightDistance + pointLight.quadraticAttenuation * fragmentLightDistance * fragmentLightDistance);

        vec3 lightToNode = point.position.xyz - lightPosition;
        float shadow = samplePointLightShadow(lightToNode, pointLight, 0.01, normal.xyz, lightDirection);
        float shadowFactor = 1.0f - shadow;

        color += computeMetallicColor(
        albedo.rgb,
        roughness,
        metallic,
        normal.xyz,
        light.diffuseColor.rgb,
        lightDirection.xyz,
        attenuation,
        viewDirection.xyz
        ) * shadowFactor;
    }

    for (uint directionalLightIndex = 0;directionalLightIndex < globalBuffer.global.directionalLightCount; directionalLightIndex++) {
        DirectionalLight directionalLight = directionalLightBuffer[globalBuffer.global.directionalLightArrayIndex].directionalLights[directionalLightIndex];
        Light light = lightBuffer[directionalLight.lightStoreItemLocation.storeIndex].lights[directionalLight.lightStoreItemLocation.itemIndex];
        Spatial directionalLightSpatial = spatialBuffer[directionalLight.spatialStoreItemLocation.storeIndex].spatials[directionalLight.spatialStoreItemLocation.itemIndex];
        LightPerspective lightPerspective = lightPerspectiveBuffer[directionalLight.lightPerspectiveStoreItemLocation.storeIndex].lightPerspectives[directionalLight.lightPerspectiveStoreItemLocation.itemIndex];

        //vec4 directionalLightPosition = directionalLight.projection * directionalLight.view * point.position;
        //vec4 shadowMap = texture(textures[0], lightPointPosition.xy * 0.5 + 0.5);

        vec3 lightOrientation = normalize(lightPerspective.absoluteFront).xyz;
        color += computeMetallicColor(albedo.rgb, roughness, metallic, normal.xyz, light.diffuseColor.rgb, -lightOrientation.xyz, 1, viewDirection.xyz);
        //float shadowFactor = 1;

        //if (lightPointPosition.z > shadowMap.r) {
        //	shadowFactor *= 0.1;
        //}
    }

    for (uint spotlightIndex = 0;spotlightIndex < globalBuffer.global.spotlightCount; spotlightIndex++) {
        Spotlight spotlight = spotlightBuffer[globalBuffer.global.spotlightArrayIndex].spotlights[spotlightIndex];
        Light light = lightBuffer[spotlight.lightStoreItemLocation.storeIndex].lights[spotlight.lightStoreItemLocation.itemIndex];
        Spatial spotlightSpatial = spatialBuffer[spotlight.spatialStoreItemLocation.storeIndex].spatials[spotlight.spatialStoreItemLocation.itemIndex];
        LightPerspective lightPerspective = lightPerspectiveBuffer[spotlight.lightPerspectiveStoreItemLocation.storeIndex].lightPerspectives[spotlight.lightPerspectiveStoreItemLocation.itemIndex];

        vec3 lightPosition = spotlightSpatial.position.xyz;
        vec3 lightOrientation = normalize(lightPerspective.absoluteFront).xyz;
        vec3 lightToNode = lightPosition - point.position.xyz;
        vec3 lightDirection = normalize(lightToNode);

        float orientationDotDirection = -dot(lightOrientation, lightDirection);
        if (orientationDotDirection > cos(spotlight.outerConeAngle)) {
            float attenuation = 1;
            if (orientationDotDirection <= cos(spotlight.innerConeAngle)) {
                attenuation = 1 - (acos(orientationDotDirection) - spotlight.innerConeAngle) / (spotlight.outerConeAngle - spotlight.innerConeAngle);
            }

            float fragmentLightDistance = distance(lightPosition, point.position.xyz);
            attenuation *= 1.0 / (spotlight.constantAttenuation + spotlight.linearAttenuation * fragmentLightDistance + spotlight.quadraticAttenuation * fragmentLightDistance * fragmentLightDistance);

            float shadow = sampleSpotlightShadow(
            lightToNode,
            spotlight,
            0.001,
            lightPerspective
            );

            float shadowFactor = 1.0f - shadow;

            color += computeMetallicColor(albedo.rgb, roughness, metallic, normal.xyz, light.diffuseColor.rgb, lightDirection.xyz, attenuation, viewDirection.xyz) * shadowFactor;
        }
    }

    //    if (albedo.a < 0.33f) {
    //        discard;
    //    }
    //outColor = albedo;
    float exposure = 1;
    const float gamma = 1;
    color = vec3(1.0) - exp(-color * exposure);
    outColor = vec4(pow(color, vec3(1/gamma)), albedo.a);
    //outColor = vec4(color, albedo.a);
}