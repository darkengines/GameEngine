#version 460
#extension GL_EXT_nonuniform_qualifier: enable

const float PI = 3.14159265359;

#include "ShadowMeshDraw.glsl"
#include "ShadowMeshVertex.glsl"
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
    ShadowMeshDraw[] meshDraws;
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

layout(location = 0) in ShadowMeshVertex point;
layout(location = 1) flat in StoreItemLocation drawItemLocation;

layout(location = 0) out vec4 outColor;

float linearize_depth(float d, float zNear, float zFar)
{
    return (d - zNear) / (zFar - zNear);
}

void main() {

    ShadowMeshDraw draw = meshDrawBuffer[drawItemLocation.storeIndex].meshDraws[drawItemLocation.itemIndex];
    Mesh mesh = meshBuffer[draw.meshItemLocation.storeIndex].meshes[draw.meshItemLocation.itemIndex];
    Object object = objectBuffer[draw.objectItemLocation.storeIndex].objects[draw.objectItemLocation.itemIndex];
    Material material = materialBuffer[mesh.materialItemLocation.storeIndex].materials[mesh.materialItemLocation.itemIndex];
    Spatial spatial = spatialBuffer[object.spatialItemLocation.storeIndex].spatials[object.spatialItemLocation.itemIndex];
    Camera camera = cameraBuffer[draw.cameraItemLocation.storeIndex].cameras[draw.cameraItemLocation.itemIndex];
    LightPerspective lightPerspective = lightPerspectiveBuffer[draw.lightPerspectiveItemLocation.storeIndex].lightPerspectives[draw.lightPerspectiveItemLocation.itemIndex];
    Spatial lightPerspectiveSpatial = spatialBuffer[draw.lightPerspectiveSpatialItemLocation.storeIndex].spatials[draw.lightPerspectiveSpatialItemLocation.itemIndex];

    float d = length((lightPerspectiveSpatial.position).xyz - point.position.xyz);
    d = linearize_depth(d, lightPerspective.near, lightPerspective.far);

    gl_FragDepth = d;

}