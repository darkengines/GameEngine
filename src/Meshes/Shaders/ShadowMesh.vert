#version 460
#extension GL_EXT_nonuniform_qualifier: enable
#extension GL_KHR_vulkan_glsl: enable

#include "ShadowMeshDraw.glsl"
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
#include "../../Lights/Shaders/LightPerspective.glsl"

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

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec4 inNormal;
layout(location = 2) in vec4 inTangent;
layout(location = 3) in vec4 inBitangent;
layout(location = 4) in vec4 inColor;
layout(location = 5) in vec2 inTexCoord;

void main() {
    uint meshDrawBufferIndex = gl_InstanceIndex / 131072u;
    uint drawItemIndex = gl_InstanceIndex % 131072u;
    ShadowMeshDraw draw = meshDrawBuffer[meshDrawBufferIndex].meshDraws[drawItemIndex];
    Mesh mesh = meshBuffer[draw.meshItemLocation.storeIndex].meshes[draw.meshItemLocation.itemIndex];
    Object object = objectBuffer[draw.objectItemLocation.storeIndex].objects[draw.objectItemLocation.itemIndex];
    Material material = materialBuffer[mesh.materialItemLocation.storeIndex].materials[mesh.materialItemLocation.itemIndex];
    Spatial spatial = spatialBuffer[object.spatialItemLocation.storeIndex].spatials[object.spatialItemLocation.itemIndex];
    Camera camera = cameraBuffer[draw.cameraItemLocation.storeIndex].cameras[draw.cameraItemLocation.itemIndex];
    LightPerspective lightPerspective = lightPerspectiveBuffer[draw.lightPerspectiveItemLocation.storeIndex].lightPerspectives[draw.lightPerspectiveItemLocation.itemIndex];

    gl_Position = lightPerspective.perspective * lightPerspective.view * spatial.absoluteModel * inPosition;
}