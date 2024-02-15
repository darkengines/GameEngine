cmake_minimum_required(VERSION 3.14)
execute_process(
    COMMAND ${CMAKE_COMMAND} -E environment
)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")

add_compile_definitions(VULKAN_HPP_NO_CONSTRUCTORS)
project(drk CXX)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(dependencies)
file(
    GLOB HEADERS
    H:/GameEngine/src/Extensions.hpp
    H:/GameEngine/src/GlmExtensions.hpp
    H:/GameEngine/src/implementations.hpp
    H:/GameEngine/src/system.hpp
    H:/GameEngine/src/Animations/Extensions.hpp
    H:/GameEngine/src/Animations/Components/Animation.hpp
    H:/GameEngine/src/Animations/Components/AnimationBehavior.hpp
    H:/GameEngine/src/Animations/Components/AnimationReference.hpp
    H:/GameEngine/src/Animations/Components/AnimationState.hpp
    H:/GameEngine/src/Animations/Components/Bone.hpp
    H:/GameEngine/src/Animations/Components/BoneCollection.hpp
    H:/GameEngine/src/Animations/Components/BoneInstanceWeightBufferView.hpp
    H:/GameEngine/src/Animations/Components/BoneReference.hpp
    H:/GameEngine/src/Animations/Components/MeshAnimation.hpp
    H:/GameEngine/src/Animations/Components/MeshBoneCollection.hpp
    H:/GameEngine/src/Animations/Components/MeshMorphAnimation.hpp
    H:/GameEngine/src/Animations/Components/NodeAnimation.hpp
    H:/GameEngine/src/Animations/Components/QuatKey.hpp
    H:/GameEngine/src/Animations/Components/RootBoneInstanceReference.hpp
    H:/GameEngine/src/Animations/Components/Skinned.hpp
    H:/GameEngine/src/Animations/Components/SkinnedBufferView.hpp
    H:/GameEngine/src/Animations/Components/SkinnedMeshInstance.hpp
    H:/GameEngine/src/Animations/Components/SkinnedVertexRange.hpp
    H:/GameEngine/src/Animations/Components/SkinnedVertexRangeBufferView.hpp
    H:/GameEngine/src/Animations/Components/VectorKey.hpp
    H:/GameEngine/src/Animations/Components/VertexWeight.hpp
    H:/GameEngine/src/Animations/Components/VertexWeightInstance.hpp
    H:/GameEngine/src/Animations/Models/AnimationBehavior.hpp
    H:/GameEngine/src/Animations/Models/Bone.hpp
    H:/GameEngine/src/Animations/Models/BoneInstanceWeight.hpp
    H:/GameEngine/src/Animations/Models/BoneSpatial.hpp
    H:/GameEngine/src/Animations/Models/SkinnedVertexRange.hpp
    H:/GameEngine/src/Animations/Models/VertexWeight.hpp
    H:/GameEngine/src/Animations/Models/VertexWeightPipelineOptions.hpp
    H:/GameEngine/src/Animations/Pipelines/SkinningPipeline.hpp
    H:/GameEngine/src/Animations/Resources/AnimationFrameResource.hpp
    H:/GameEngine/src/Animations/Resources/AnimationResourceManager.hpp
    H:/GameEngine/src/Animations/Systems/AnimationSystem.hpp
    H:/GameEngine/src/Animations/Systems/BoneSpatialSystem.hpp
    H:/GameEngine/src/Animations/Systems/BoneSystem.hpp
    H:/GameEngine/src/Animations/Systems/SkinnedMeshInstanceSystem.hpp
    H:/GameEngine/src/Animations/Systems/VertexWeightSystem.hpp
    H:/GameEngine/src/Applications/Application.hpp
    H:/GameEngine/src/Applications/Extentions.hpp
    H:/GameEngine/src/BoundingVolumes/Extensions.hpp
    H:/GameEngine/src/BoundingVolumes/Components/AxisAlignedBoundingBox.hpp
    H:/GameEngine/src/BoundingVolumes/Components/Draw.hpp
    H:/GameEngine/src/BoundingVolumes/Components/HasDraw.hpp
    H:/GameEngine/src/BoundingVolumes/Models/AxisAlignedBoundingBox.hpp
    H:/GameEngine/src/BoundingVolumes/Models/BoundingVolumeDraw.hpp
    H:/GameEngine/src/BoundingVolumes/Models/Vertex.hpp
    H:/GameEngine/src/BoundingVolumes/Pipelines/BoundingVolumePipeline.hpp
    H:/GameEngine/src/BoundingVolumes/Systems/AxisAlignedBoundingBoxSystem.hpp
    H:/GameEngine/src/Buffers/Resources/BufferResourceManager.hpp
    H:/GameEngine/src/Cameras/Extensions.hpp
    H:/GameEngine/src/Cameras/Components/Camera.hpp
    H:/GameEngine/src/Cameras/Components/CameraRefrence.hpp
    H:/GameEngine/src/Cameras/Editors/CameraEditor.hpp
    H:/GameEngine/src/Cameras/Models/Camera.hpp
    H:/GameEngine/src/Cameras/Systems/CameraSystem.hpp
    H:/GameEngine/src/Common/Common.hpp
    H:/GameEngine/src/Common/ComponentIndex.hpp
    H:/GameEngine/src/Common/IndexGenerator.hpp
    H:/GameEngine/src/Common/KGuillotineAllocator.hpp
    H:/GameEngine/src/Common/Components/Dirty.hpp
    H:/GameEngine/src/Common/Components/MultiBufferView.hpp
    H:/GameEngine/src/Common/Components/Name.hpp
    H:/GameEngine/src/Configuration/Extensions.hpp
    H:/GameEngine/src/Controllers/Extensions.hpp
    H:/GameEngine/src/Controllers/FlyCamController.hpp
    H:/GameEngine/src/Debugging/Components/DebugDraw.hpp
    H:/GameEngine/src/Debugging/Renderers/DebugRenderer.hpp
    H:/GameEngine/src/Devices/Buffer.hpp
    H:/GameEngine/src/Devices/BufferStateUploadResult.hpp
    H:/GameEngine/src/Devices/BufferUploadResult.hpp
    H:/GameEngine/src/Devices/BufferView.hpp
    H:/GameEngine/src/Devices/BufferViewState.hpp
    H:/GameEngine/src/Devices/Device.hpp
    H:/GameEngine/src/Devices/DeviceContext.hpp
    H:/GameEngine/src/Devices/DeviceImageInfo.hpp
    H:/GameEngine/src/Devices/Extensions.hpp
    H:/GameEngine/src/Devices/Image.hpp
    H:/GameEngine/src/Devices/ImageInfo.hpp
    H:/GameEngine/src/Devices/QueueFamilyIndices.hpp
    H:/GameEngine/src/Devices/Swapchain.hpp
    H:/GameEngine/src/Devices/SwapChainSupportDetails.hpp
    H:/GameEngine/src/Devices/Texture.hpp
    H:/GameEngine/src/Devices/VulkanInstanceConfiguration.hpp
    H:/GameEngine/src/Devices/VulkanLogicalDeviceInfo.hpp
    H:/GameEngine/src/Draws/Extentions.hpp
    H:/GameEngine/src/Draws/Components/DrawVertexBufferInfo.hpp
    H:/GameEngine/src/Draws/Models/Draw.hpp
    H:/GameEngine/src/Draws/Systems/DrawSystem.hpp
    H:/GameEngine/src/Engine/DescriptorSetAllocator.hpp
    H:/GameEngine/src/Engine/DescriptorSetLayoutCache.hpp
    H:/GameEngine/src/Engine/DescriptorSetLayoutCreateInfoEqualityComparer.hpp
    H:/GameEngine/src/Engine/DescriptorSetLayouts.hpp
    H:/GameEngine/src/Engine/EngineState.hpp
    H:/GameEngine/src/Engine/Extensions.hpp
    H:/GameEngine/src/Engine/FrameState.hpp
    H:/GameEngine/src/Engine/Models/Global.hpp
    H:/GameEngine/src/FreeList/FreeBlock.hpp
    H:/GameEngine/src/FreeList/FreeList.hpp
    H:/GameEngine/src/Frustums/Extensions.hpp
    H:/GameEngine/src/Frustums/Components/Draw.hpp
    H:/GameEngine/src/Frustums/Components/Frustum.hpp
    H:/GameEngine/src/Frustums/Components/HasDraw.hpp
    H:/GameEngine/src/Frustums/Models/Frustum.hpp
    H:/GameEngine/src/Frustums/Models/FrustumDraw.hpp
    H:/GameEngine/src/Frustums/Models/FrustumVertex.hpp
    H:/GameEngine/src/Frustums/Models/Vertex.hpp
    H:/GameEngine/src/Frustums/Pipelines/FrustumPipeline.hpp
    H:/GameEngine/src/Frustums/Systems/FrustumSystem.hpp
    H:/GameEngine/src/Graphics/Draw.hpp
    H:/GameEngine/src/Graphics/Extentions.hpp
    H:/GameEngine/src/Graphics/GlobalSystem.hpp
    H:/GameEngine/src/Graphics/Graphics.hpp
    H:/GameEngine/src/Graphics/SynchronizationState.hpp
    H:/GameEngine/src/Graphics/Models/Draw.hpp
    H:/GameEngine/src/Lights/Extensions.hpp
    H:/GameEngine/src/Lights/Components/DirectionalLight.hpp
    H:/GameEngine/src/Lights/Components/Light.hpp
    H:/GameEngine/src/Lights/Components/LightPerspective.hpp
    H:/GameEngine/src/Lights/Components/LightPerspectiveCollection.hpp
    H:/GameEngine/src/Lights/Components/PointLight.hpp
    H:/GameEngine/src/Lights/Components/Spotlight.hpp
    H:/GameEngine/src/Lights/Models/DirectionalLight.hpp
    H:/GameEngine/src/Lights/Models/Light.hpp
    H:/GameEngine/src/Lights/Models/LightPerspective.hpp
    H:/GameEngine/src/Lights/Models/PointLight.hpp
    H:/GameEngine/src/Lights/Models/Spotlight.hpp
    H:/GameEngine/src/Lights/Systems/DirectionalLightSystem.hpp
    H:/GameEngine/src/Lights/Systems/LightPerspectiveSystem.hpp
    H:/GameEngine/src/Lights/Systems/LightSystem.hpp
    H:/GameEngine/src/Lights/Systems/PointLightSystem.hpp
    H:/GameEngine/src/Lights/Systems/ShadowMappingSystem.hpp
    H:/GameEngine/src/Lights/Systems/SpotlightSystem.hpp
    H:/GameEngine/src/Lines/Extensions.hpp
    H:/GameEngine/src/Lines/Components/Line.hpp
    H:/GameEngine/src/Lines/Components/LineDraw.hpp
    H:/GameEngine/src/Lines/Models/Line.hpp
    H:/GameEngine/src/Lines/Models/LineDraw.hpp
    H:/GameEngine/src/Lines/Models/LineVertex.hpp
    H:/GameEngine/src/Lines/Pipelines/LinePipeline.hpp
    H:/GameEngine/src/Lines/Systems/LineSystem.hpp
    H:/GameEngine/src/Loaders/AssimpLoader.hpp
    H:/GameEngine/src/Loaders/Extensions.hpp
    H:/GameEngine/src/Loaders/LoadResult.hpp
    H:/GameEngine/src/Materials/Extensions.hpp
    H:/GameEngine/src/Materials/Components/Material.hpp
    H:/GameEngine/src/Materials/Components/MaterialCollection.hpp
    H:/GameEngine/src/Materials/Components/MaterialReference.hpp
    H:/GameEngine/src/Materials/Models/Material.hpp
    H:/GameEngine/src/Materials/Systems/MaterialSystem.hpp
    H:/GameEngine/src/Meshes/Extensions.hpp
    H:/GameEngine/src/Meshes/Vertex.hpp
    H:/GameEngine/src/Meshes/Components/HasShadowDraw.hpp
    H:/GameEngine/src/Meshes/Components/Mesh.hpp
    H:/GameEngine/src/Meshes/Components/MeshBufferView.hpp
    H:/GameEngine/src/Meshes/Components/MeshDraw.hpp
    H:/GameEngine/src/Meshes/Components/MeshReference.hpp
    H:/GameEngine/src/Meshes/Components/MeshResource.hpp
    H:/GameEngine/src/Meshes/Components/ShadowMeshDraw.hpp
    H:/GameEngine/src/Meshes/Models/Mesh.hpp
    H:/GameEngine/src/Meshes/Models/MeshDraw.hpp
    H:/GameEngine/src/Meshes/Models/ShadowMeshDraw.hpp
    H:/GameEngine/src/Meshes/Pipelines/MeshPipeline.hpp
    H:/GameEngine/src/Meshes/Pipelines/ShadowMeshPipeline.hpp
    H:/GameEngine/src/Meshes/Resources/MeshResourceManager.hpp
    H:/GameEngine/src/Meshes/Resources/MeshUploadResult.hpp
    H:/GameEngine/src/Meshes/Systems/MeshShadowSystem.hpp
    H:/GameEngine/src/Meshes/Systems/MeshSystem.hpp
    H:/GameEngine/src/Meshes/Systems/MeshSystemOperation.hpp
    H:/GameEngine/src/Nodes/Components/Node.hpp
    H:/GameEngine/src/Objects/Extensions.hpp
    H:/GameEngine/src/Objects/Components/ObjectMeshCollection.hpp
    H:/GameEngine/src/Objects/Components/ObjectMeshReference.hpp
    H:/GameEngine/src/Objects/Components/ObjectReference.hpp
    H:/GameEngine/src/Objects/Models/Object.hpp
    H:/GameEngine/src/Objects/Systems/ObjectSystem.hpp
    H:/GameEngine/src/Pipelines/ComputePipeline.hpp
    H:/GameEngine/src/Pipelines/GraphicsPipeline.hpp
    H:/GameEngine/src/Planes/Components/Plane.hpp
    H:/GameEngine/src/Planes/Models/Plane.hpp
    H:/GameEngine/src/Planes/Systems/PlaneSystem.hpp
    H:/GameEngine/src/Points/Extensions.hpp
    src/Points/Components/Point.hpp
    src/Points/Components/PointDraw.hpp
    src/Points/Models/Point.hpp
    src/Points/Models/PointDraw.hpp
    src/Points/Models/PointVertex.hpp
    src/Points/Pipelines/PointPrimitivePipeline.hpp
    src/Points/Systems/PointSystem.hpp
    src/Relationships/Extensions.hpp
    src/Relationships/Systems/RelationshipSystem.hpp
    src/Renderers/Renderer.hpp
    src/Renderers/RenderOperation.hpp
    src/Resources/Allocator.hpp
    src/Resources/ResourceManager.hpp
    src/Resources/Configuration/ResourcesConfiguration.hpp
    src/Scenes/Extensions.hpp
    src/Scenes/Draws/SceneDraw.hpp
    src/Scenes/Draws/ShadowSceneDraw.hpp
    src/Scenes/Renderers/SceneRenderer.hpp
    src/Scenes/Renderers/ShadowSceneRenderer.hpp
    src/Scenes/Systems/SceneSystem.hpp
    src/Spatials/Extensions.hpp
    src/Spatials/Components/Spatial.hpp
    src/Spatials/Components/SpatialEditor.hpp
    src/Spatials/Models/RelativeSpatial.hpp
    src/Spatials/Models/Spatial.hpp
    src/Spatials/Systems/RelativeSpatialSystem.hpp
    src/Spatials/Systems/SpatialSystem.hpp
    src/Stores/Extensions.hpp
    src/Stores/Store.hpp
    src/Stores/StoreBuffer.hpp
    src/Stores/StoreBufferAllocator.hpp
    src/Stores/StoreItem.hpp
    src/Stores/StoreItemLocation.hpp
    src/Stores/TextureStore.hpp
    src/Stores/Models/StoreItemLocation.hpp
    src/Systems/System.hpp
    src/Textures/Extensions.hpp
    src/Textures/ImageInfo.hpp
    src/Textures/TextureType.hpp
    src/Textures/Systems/TextureSystem.hpp
    src/UserInterfaces/AssetExplorer.hpp
    src/UserInterfaces/Extensions.hpp
    src/UserInterfaces/UserInterface.hpp
    src/UserInterfaces/Renderers/UserInterfaceRenderer.hpp
    src/Windows/Extensions.hpp
    src/Windows/Window.hpp
    src/Windows/WindowConfiguration.hpp
)

file(
    GLOB SOURCES
    H:/GameEngine/src/GlmExtensions.cpp
    H:/GameEngine/src/main.cpp
    H:/GameEngine/src/Animations/Pipelines/SkinningPipeline.cpp
    H:/GameEngine/src/Animations/Resources/AnimationFrameResource.cpp
    H:/GameEngine/src/Animations/Resources/AnimationResourceManager.cpp
    H:/GameEngine/src/Animations/Systems/AnimationSystem.cpp
    H:/GameEngine/src/Animations/Systems/BoneSpatialSystem.cpp
    H:/GameEngine/src/Animations/Systems/BoneSystem.cpp
    H:/GameEngine/src/Animations/Systems/VertexWeightSystem.cpp
    H:/GameEngine/src/Applications/Application.cpp
    H:/GameEngine/src/Applications/Application.UserInterface.cpp
    H:/GameEngine/src/BoundingVolumes/Components/AxisAlignedBoundingBox.cpp
    H:/GameEngine/src/BoundingVolumes/Models/Vertex.cpp
    H:/GameEngine/src/BoundingVolumes/Pipelines/BoundingVolumePipeline.cpp
    H:/GameEngine/src/BoundingVolumes/Systems/AxisAlignedBoundingBoxSystem.cpp
    H:/GameEngine/src/Cameras/Systems/CameraSystem.cpp
    H:/GameEngine/src/Common/Common.cpp
    H:/GameEngine/src/Common/IndexGenerator.cpp
    H:/GameEngine/src/Common/KGuillotineAllocator.cpp
    H:/GameEngine/src/Controllers/FlyCamController.cpp
    H:/GameEngine/src/Debugging/Renderers/DebugRenderer.cpp
    H:/GameEngine/src/Devices/Device.cpp
    H:/GameEngine/src/Devices/DeviceContext.cpp
    H:/GameEngine/src/Devices/Image.cpp
    H:/GameEngine/src/Devices/Texture.cpp
    H:/GameEngine/src/Devices/VulkanInstanceConfiguration.cpp
    H:/GameEngine/src/Engine/DescriptorSetAllocator.cpp
    H:/GameEngine/src/Engine/DescriptorSetLayoutCache.cpp
    H:/GameEngine/src/Engine/DescriptorSetLayoutCreateInfoEqualityComparer.cpp
    H:/GameEngine/src/Engine/DescriptorSetLayouts.cpp
    H:/GameEngine/src/Engine/EngineState.cpp
    H:/GameEngine/src/Engine/FrameState.cpp
    H:/GameEngine/src/Frustums/Components/Frustum.cpp
    H:/GameEngine/src/Frustums/Models/Vertex.cpp
    H:/GameEngine/src/Frustums/Pipelines/FrustumPipeline.cpp
    H:/GameEngine/src/Frustums/Systems/FrustumSystem.cpp
    H:/GameEngine/src/Graphics/GlobalSystem.cpp
    src/Graphics/Graphics.cpp
    src/Graphics/SynchronizationState.cpp
    src/ImGui/imgui_impl_glfw.cpp
    src/ImGui/imgui_impl_vulkan.cpp
    src/ImGui/imgui_stdlib.cpp
    src/Lights/Systems/DirectionalLightSystem.cpp
    src/Lights/Systems/LightPerspectiveSystem.cpp
    src/Lights/Systems/LightSystem.cpp
    src/Lights/Systems/PointLightSystem.cpp
    src/Lights/Systems/ShadowMappingSystem.cpp
    src/Lights/Systems/SpotlightSystem.cpp
    src/Lines/Models/LineVertex.cpp
    src/Lines/Pipelines/LinePipeline.cpp
    src/Lines/Systems/LineSystem.cpp
    src/Loaders/AssimpLoader.cpp
    src/Materials/Systems/MaterialSystem.cpp
    src/Meshes/Vertex.cpp
    src/Meshes/Pipelines/MeshPipeline.cpp
    src/Meshes/Pipelines/ShadowMeshPipeline.cpp
    src/Meshes/Systems/MeshShadowSystem.cpp
    src/Meshes/Systems/MeshSystem.cpp
    src/Nodes/Systems/NodeSystem.cpp
    src/Planes/Components/Plane.cpp
    src/Planes/Systems/PlaneSystem.cpp
    src/Points/Models/PointVertex.cpp
    src/Points/Pipelines/PointPrimitivePipeline.cpp
    src/Points/Systems/PointSystem.cpp
    src/Relationships/Systems/RelationshipSystem.cpp
    src/Renderers/Renderer.cpp
    src/Resources/ResourceManager.cpp
    src/Resources/Configuration/ResourcesConfiguration.cpp
    src/Scenes/Renderers/SceneRenderer.cpp
    src/Scenes/Renderers/ShadowSceneRenderer.cpp
    src/Scenes/Systems/SceneSystem.cpp
    src/Spatials/Components/SpatialEditor.cpp
    src/Spatials/Systems/SpatialSystem.cpp
    src/Stores/Store.cpp
    src/Stores/StoreBuffer.cpp
    src/Stores/StoreBufferAllocator.cpp
    src/Stores/TextureStore.cpp
    src/Systems/System.cpp
    src/Textures/ImageInfo.cpp
    src/Textures/Systems/TextureSystem.cpp
    src/UserInterfaces/AssetExplorer.cpp
    src/UserInterfaces/UserInterface.cpp
    src/UserInterfaces/Renderers/UserInterfaceRenderer.cpp
    src/Windows/Window.cpp
)

file(
    GLOB GLSL_SOURCES
    "src/Meshes/Shaders/Mesh.vert"
    "src/Meshes/Shaders/Mesh.frag"
    "src/Meshes/Shaders/ShadowMesh.vert"
    "src/Meshes/Shaders/ShadowMesh.frag"
    "src/Points/Shaders/Point.vert"
    "src/Points/Shaders/Point.frag"
    "src/Lines/Shaders/Line.vert"
    "src/Lines/Shaders/Line.frag"
    "src/BoundingVolumes/Shaders/BoundingVolume.vert"
    "src/BoundingVolumes/Shaders/BoundingVolume.frag"
    "src/Frustums/Shaders/Frustum.vert"
    "src/Frustums/Shaders/Frustum.frag"
    "src/Animations/Shaders/Skinning.comp"
)

foreach (GLSL ${GLSL_SOURCES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${PROJECT_BINARY_DIR}/shaders/spv/${FILE_NAME}.spv")
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/shaders/spv/"
        COMMAND $ENV{VULKAN_SDK}/Bin/glslc.exe -g --target-env=vulkan1.3 ${GLSL} -o ${SPIRV}
        COMMAND
        DEPENDS ${GLSL})
    list(APPEND SPIRV_BINARIES ${SPIRV})
endforeach (GLSL)

add_custom_target(
    Shaders
    DEPENDS ${SPIRV_BINARIES}
)

add_executable(drk ${HEADERS} ${SOURCES})
add_dependencies(drk Shaders)
file (GENERATE OUTPUT settings.json INPUT ${PROJECT_SOURCE_DIR}/settings.json)

find_package(Vulkan REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
find_package(entt REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(assimp REQUIRED)
find_package(stb REQUIRED)
find_package(fmt REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(nameof REQUIRED)

target_include_directories(drk SYSTEM PUBLIC ${Vulkan_INCLUDE_DIRS})
target_include_directories(drk SYSTEM PUBLIC ${stb_INCLUDE_DIRS})
target_include_directories(drk SYSTEM PUBLIC ${IMGUI_INCLUDE_DIR} ${IMGUI_INCLUDE_DIR}/backends)
target_include_directories(drk SYSTEM PUBLIC ${imgui-filebrowser})
target_include_directories(drk SYSTEM PUBLIC ${nlohmann_json_INCLUDE_DIRS})
target_include_directories(drk SYSTEM PUBLIC ${nameof_INCLUDE_DIRS})

target_link_libraries(drk PRIVATE ${Vulkan_LIBRARIES})
target_link_libraries(drk PRIVATE GPUOpen::VulkanMemoryAllocator)
target_link_libraries(drk PRIVATE glfw)
target_link_libraries(drk PRIVATE EnTT::EnTT)
target_link_libraries(drk PRIVATE glm::glm)
target_link_libraries(drk PRIVATE assimp::assimp)
target_link_libraries(drk PRIVATE imgui)
target_link_libraries(drk PRIVATE fmt::fmt-header-only)
target_link_libraries(drk PRIVATE nlohmann_json)