#version 460
#extension GL_EXT_nonuniform_qualifier: enable
#extension GL_KHR_vulkan_glsl: enable

const float PI = 3.14159265359;

#include "MeshDraw.glsl"
#include "MeshVertex.glsl"
#include "Mesh.glsl"

#include "../../Graphics/Shaders/StoreItemLocation.glsl"
#include "../../Graphics/Shaders/Global.glsl"
#include "../../Materials/shaders/Material.glsl"
#include "../../Spatials/shaders/Spatial.glsl"
#include "../../Objects/shaders/Object.glsl"
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
layout (set = 3, binding = 0) readonly buffer objectLayout {
    Object[] objects;
} objectBuffer[];
layout (set = 3, binding = 0) readonly buffer cameraLayout {
    Camera[] cameras;
} cameraBuffer[];
layout (set = 3, binding = 0) readonly buffer lightLayout {
    Light[] lights;
} lightBuffer[];
layout (set = 3, binding = 0) readonly buffer pointLightLayout {
    PointLight[] pointLights;
} pointLightBuffer[];
layout (set = 3, binding = 0) readonly buffer directionalLightLayout {
    DirectionalLight[] directionalLights;
} directionalLightBuffer[];
layout (set = 3, binding = 0) readonly buffer spotlightLayout {
    Spotlight[] spotlights;
} spotlightBuffer[];
layout (set = 3, binding = 0) readonly buffer lightPerspectiveLayout {
    LightPerspective[] lightPerspectives;
} lightPerspectiveBuffer[];

layout(location = 0) in MeshVertex point;
layout(location = 10) flat in StoreItemLocation drawItemLocation;

layout(location = 0) out vec4 outColor;

void main() {

    MeshDraw draw = meshDrawBuffer[drawItemLocation.storeIndex].meshDraws[drawItemLocation.itemIndex];
    Mesh mesh = meshBuffer[draw.meshItemLocation.storeIndex].meshes[draw.meshItemLocation.itemIndex];
    Object object = objectBuffer[draw.objectItemLocation.storeIndex].objects[draw.objectItemLocation.itemIndex];
    Material material = materialBuffer[mesh.materialItemLocation.storeIndex].materials[mesh.materialItemLocation.itemIndex];
    Spatial spatial = spatialBuffer[object.spatialItemLocation.storeIndex].spatials[object.spatialItemLocation.itemIndex];
    Camera camera = cameraBuffer[draw.cameraItemLocation.storeIndex].cameras[draw.cameraItemLocation.itemIndex];

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

    vec3 color = vec3(0, 0, 0);
    vec4 albedo = material.diffuseColor;
    if (material.hasDiffuseColorTexture) {
        albedo = texture(textures[material.diffuseColorTextureIndex], point.texCoord);
    }

    for(uint pointLightIndex = 0;pointLightIndex < globalBuffer.global.pointLightCount; pointLightIndex++) {
        PointLight pointLight = pointLightBuffer[globalBuffer.global.pointLightArrayIndex].pointLights[pointLightIndex];
        Light light = lightBuffer[pointLight.lightStoreItemLocation.storeIndex].lights[pointLight.lightStoreItemLocation.itemIndex];
        Spatial pointLightSpatial = spatialBuffer[pointLight.spatialStoreItemLocation.storeIndex].spatials[pointLight.spatialStoreItemLocation.itemIndex];

        vec3 lightPosition = pointLightSpatial.absolutePosition.xyz;
		float fragmentLightDistance = distance(lightPosition, point.position.xyz);
		vec3 lightDirection = normalize(lightPosition - point.position.xyz);
		float attenuation = 1.0 / (pointLight.constantAttenuation + pointLight.linearAttenuation * fragmentLightDistance + pointLight.quadraticAttenuation * fragmentLightDistance * fragmentLightDistance);
				
		color += computeColor(albedo.rgb, roughness, metallic, normal.xyz, light.diffuseColor.rgb, lightDirection.xyz, attenuation, viewDirection.xyz);
    }

    for(uint directionalLightIndex = 0;directionalLightIndex < globalBuffer.global.directionalLightCount; directionalLightIndex++) {
        DirectionalLight directionalLight = directionalLightBuffer[globalBuffer.global.directionalLightArrayIndex].directionalLights[directionalLightIndex];
        Light light = lightBuffer[directionalLight.lightStoreItemLocation.storeIndex].lights[directionalLight.lightStoreItemLocation.itemIndex];
        Spatial directionalLightSpatial = spatialBuffer[directionalLight.spatialStoreItemLocation.storeIndex].spatials[directionalLight.spatialStoreItemLocation.itemIndex];
        LightPerspective lightPerspective = lightPerspectiveBuffer[directionalLight.lightPerspectiveStoreItemLocation.storeIndex].lightPerspectives[directionalLight.lightPerspectiveStoreItemLocation.itemIndex];

        //vec4 directionalLightPosition = directionalLight.projection * directionalLight.view * point.position;
		//vec4 shadowMap = texture(textures[0], lightPointPosition.xy * 0.5 + 0.5);

		vec3 lightOrientation = normalize(lightPerspective.absoluteFront).xyz;
		color += computeColor(albedo.rgb, roughness, metallic, normal.xyz, light.diffuseColor.rgb, -lightOrientation.xyz, 1, viewDirection.xyz);
		//float shadowFactor = 1;

		//if (lightPointPosition.z > shadowMap.r) {
		//	shadowFactor *= 0.1;
		//}

		//color += computeColor(albedo.rgb, roughness, metallic, normal.xyz, lightState.diffuseColor.rgb, -lightOrientation.xyz, 1, viewDirection.xyz) * shadowFactor;
    }

    for(uint spotlightIndex = 0;spotlightIndex < globalBuffer.global.spotlightCount; spotlightIndex++) {
        Spotlight spotlight = spotlightBuffer[globalBuffer.global.spotlightArrayIndex].spotlights[spotlightIndex];
        Light light = lightBuffer[spotlight.lightStoreItemLocation.storeIndex].lights[spotlight.lightStoreItemLocation.itemIndex];
        Spatial spotlightSpatial = spatialBuffer[spotlight.spatialStoreItemLocation.storeIndex].spatials[spotlight.spatialStoreItemLocation.itemIndex];
        LightPerspective lightPerspective = lightPerspectiveBuffer[spotlight.lightPerspectiveStoreItemLocation.storeIndex].lightPerspectives[spotlight.lightPerspectiveStoreItemLocation.itemIndex];

        vec3 lightPosition = spotlightSpatial.absolutePosition.xyz;
		vec3 lightOrientation = normalize(lightPerspective.absoluteFront).xyz;
		vec3 lightDirection = normalize(lightPosition - point.position.xyz);

		float orientationDotDirection = -dot(lightOrientation, lightDirection);
		if (orientationDotDirection > cos(spotlight.outerConeAngle)) {
			float attenuation = 1;
			if (orientationDotDirection <= cos(spotlight.innerConeAngle)) {
				attenuation = 1 - (acos(orientationDotDirection) - spotlight.innerConeAngle) / (spotlight.outerConeAngle - spotlight.innerConeAngle);
			}

			float fragmentLightDistance = distance(lightPosition, point.position.xyz);
			attenuation *= 1.0 / (spotlight.constantAttenuation + spotlight.linearAttenuation * fragmentLightDistance + spotlight.quadraticAttenuation * fragmentLightDistance * fragmentLightDistance);
			//vec3 radiance = attenuation * pointLightState.diffuseColor.rgb;

			color += computeColor(albedo.rgb, roughness, metallic, normal.xyz, light.diffuseColor.rgb, lightDirection.xyz, attenuation, viewDirection.xyz);
		}
    }

//    if (albedo.a < 0.33f) {
//        discard;
//    }
    //outColor = albedo;
    float exposure = 1;

	color = vec3(1.0) - exp(-color * exposure);
    outColor = vec4(pow(color.rgb, vec3(1/gamma)), albedo.a);
    //outColor = vec4(color, albedo.a);
}