#version 460
#extension GL_EXT_nonuniform_qualifier: enable

#include "MeshDraw.glsl"
#include "MeshVertex.glsl"
#include "Mesh.glsl"

#include "../../Graphics/Shaders/StoreItemLocation.glsl"
#include "../../Graphics/Shaders/Global.glsl"
#include "../../Materials/shaders/Material.glsl"
#include "../../Spatials/shaders/Spatial.glsl"
#include "../../Nodes/shaders/Node.glsl"
#include "../../Cameras/shaders/Camera.glsl"
#include "../../Lights/Shaders/PointLight.glsl"
#include "../../Lights/Shaders/DirectionalLight.glsl"
#include "../../Lights/Shaders/Spotlight.glsl"
#include "../../Lights/Shaders/LightPerspective.glsl"

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
layout (std430, set = 3, binding = 0) readonly buffer pointLightLayout {
    PointLight[] pointLights;
} pointLightBuffer[];
layout (std430, set = 3, binding = 0) readonly buffer directionalLightLayout {
    DirectionalLight[] directionalLights;
} directionalLightBuffer[];
layout (std430, set = 3, binding = 0) readonly buffer spotlightLayout {
    Spotlight[] spotlights;
} spotlightBuffer[];
layout (std430, set = 3, binding = 0) readonly buffer lightPerspectiveLayout {
    LightPerspective[] lightPerspectives;
} lightPerspectiveBuffer[];

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
    Node node = nodeBuffer[draw.nodeItemLocation.storeIndex].nodes[draw.nodeItemLocation.itemIndex];
    Material material = materialBuffer[mesh.materialItemLocation.storeIndex].materials[mesh.materialItemLocation.itemIndex];
    Spatial spatial = spatialBuffer[node.spatialItemLocation.storeIndex].spatials[node.spatialItemLocation.itemIndex];
    Camera camera = cameraBuffer[draw.cameraItemLocation.storeIndex].cameras[draw.cameraItemLocation.itemIndex];

    gl_Position = camera.perspective * camera.view * spatial.model * inPosition;

    drawItemLocation = StoreItemLocation(meshDrawBufferIndex, drawItemIndex);

    fragment.position = spatial.model * inPosition;
    fragment.color = inColor;
    fragment.texCoord = inTexCoord;

    vec4 T = vec4(normalize(mat3(spatial.model) * inTangent.xyz), 0);
    vec4 N = vec4(normalize(mat3(spatial.model) * inNormal.xyz), 0);
    vec4 B = vec4(normalize(mat3(spatial.model) * inBitangent.xyz), 0);

    fragment.tangent = T;
    fragment.bitangent = B;
    fragment.normal = N;

    fragment.TBN = mat4(T, B, N, 0, 0, 0, 0);
}