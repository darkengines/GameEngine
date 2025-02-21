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
    src/Extensions.hpp
	src/GlmExtensions.hpp
	src/implementations.hpp
	src/system.hpp
	src/Animations/Extensions.hpp
	src/Animations/Components/Animation.hpp
	src/Animations/Components/AnimationBehavior.hpp
	src/Animations/Components/AnimationState.hpp
	src/Animations/Components/Bone.hpp
	src/Animations/Components/BoneCollection.hpp
	src/Animations/Components/BoneInstanceWeightBufferView.hpp
	src/Animations/Components/BoneMesh.hpp
	src/Animations/Components/BoneReference.hpp
	src/Animations/Components/MeshAnimation.hpp
	src/Animations/Components/MeshBoneCollection.hpp
	src/Animations/Components/MeshMorphAnimation.hpp
	src/Animations/Components/NodeAnimation.hpp
	src/Animations/Components/NodeBoneMesh.hpp
	src/Animations/Components/QuatKey.hpp
	src/Animations/Components/RootBoneInstanceReference.hpp
	src/Animations/Components/Skinned.hpp
	src/Animations/Components/SkinnedBufferView.hpp
	src/Animations/Components/SkinnedMeshInstance.hpp
	src/Animations/Components/SkinnedVertexRange.hpp
	src/Animations/Components/SkinnedVertexRangeBufferView.hpp
	src/Animations/Components/VectorKey.hpp
	src/Animations/Components/VertexWeight.hpp
	src/Animations/Components/VertexWeightInstance.hpp
	src/Animations/Models/AnimationBehavior.hpp
	src/Animations/Models/BoneInstanceWeight.hpp
	src/Animations/Models/BoneMesh.hpp
	src/Animations/Models/BoneSpatial.hpp
	src/Animations/Models/SkinnedVertexRange.hpp
	src/Animations/Models/VertexWeight.hpp
	src/Animations/Models/VertexWeightPipelineOptions.hpp
	src/Animations/Pipelines/SkinningPipeline.hpp
	src/Animations/Resources/AnimationFrameResource.hpp
	src/Animations/Resources/AnimationResourceManager.hpp
	src/Animations/Systems/AnimationSystem.hpp
	src/Animations/Systems/BoneMeshSystem.hpp
	src/Animations/Systems/BoneSpatialSystem.hpp
	src/Animations/Systems/SkinnedMeshInstanceSystem.hpp
	src/Animations/Systems/VertexWeightSystem.hpp
	src/Applications/Application.hpp
	src/Applications/Extentions.hpp
	src/Applications/Root.hpp
	src/BoundingVolumes/Extensions.hpp
	src/BoundingVolumes/Components/AxisAlignedBoundingBox.hpp
	src/BoundingVolumes/Components/Draw.hpp
	src/BoundingVolumes/Components/HasDraw.hpp
	src/BoundingVolumes/Models/AxisAlignedBoundingBox.hpp
	src/BoundingVolumes/Models/BoundingVolumeDraw.hpp
	src/BoundingVolumes/Models/Vertex.hpp
	src/BoundingVolumes/Pipelines/BoundingVolumePipeline.hpp
	src/BoundingVolumes/Systems/AxisAlignedBoundingBoxSystem.hpp
	src/Buffers/Resources/BufferResourceManager.hpp
	src/Buffers/Extensions.hpp
	src/Cameras/Extensions.hpp
	src/Cameras/Components/Camera.hpp
	src/Cameras/Components/CameraRefrence.hpp
	src/Cameras/Editors/CameraEditor.hpp
	src/Cameras/Models/Camera.hpp
	src/Cameras/Systems/CameraSystem.hpp
	src/Common/Common.hpp
	src/Common/ComponentIndex.hpp
	src/Common/IndexGenerator.hpp
	src/Common/KGuillotineAllocator.hpp
	src/Common/Components/Dirty.hpp
	src/Common/Components/MultiBufferView.hpp
	src/Common/Components/Name.hpp
	src/Configuration/Extensions.hpp
	src/Configuration/Configuration.hpp
	src/Controllers/Extensions.hpp
	src/Controllers/FlyCamController.hpp
	src/Debugging/Components/DebugDraw.hpp
	src/Debugging/Renderers/DebugRenderer.hpp
	src/Devices/Buffer.hpp
	src/Devices/BufferStateUploadResult.hpp
	src/Devices/BufferUploadResult.hpp
	src/Devices/BufferView.hpp
	src/Devices/BufferViewState.hpp
	src/Devices/Device.hpp
	src/Devices/DeviceContext.hpp
	src/Devices/DeviceImageInfo.hpp
	src/Devices/Extensions.hpp
	src/Devices/Image.hpp
	src/Devices/ImageInfo.hpp
	src/Devices/QueueFamilyIndices.hpp
	src/Devices/Swapchain.hpp
	src/Devices/SwapChainSupportDetails.hpp
	src/Devices/Texture.hpp
	src/Devices/VulkanInstanceConfiguration.hpp
	src/Devices/VulkanLogicalDeviceInfo.hpp
	src/Draws/Extentions.hpp
	src/Draws/Components/DrawVertexBufferInfo.hpp
	src/Draws/Models/Draw.hpp
	src/Draws/Systems/DrawSystem.hpp
	src/Engine/DescriptorSetAllocator.hpp
	src/Engine/DescriptorSetLayoutCache.hpp
	src/Engine/DescriptorSetLayoutCreateInfoEqualityComparer.hpp
	src/Engine/DescriptorSetLayouts.hpp
	src/Engine/EngineState.hpp
	src/Engine/Extensions.hpp
	src/Engine/FrameState.hpp
	src/Engine/Models/Global.hpp
	src/FreeList/FreeBlock.hpp
	src/FreeList/FreeList.hpp
	src/Frustums/Extensions.hpp
	src/Frustums/Components/Draw.hpp
	src/Frustums/Components/Frustum.hpp
	src/Frustums/Components/HasDraw.hpp
	src/Frustums/Models/Frustum.hpp
	src/Frustums/Models/FrustumDraw.hpp
	src/Frustums/Models/FrustumVertex.hpp
	src/Frustums/Models/Vertex.hpp
	src/Frustums/Pipelines/FrustumPipeline.hpp
	src/Frustums/Systems/FrustumSystem.hpp
	src/Graphics/Draw.hpp
	src/Graphics/Extentions.hpp
	src/Graphics/GlobalSystem.hpp
	src/Graphics/Graphics.hpp
	src/Graphics/SynchronizationState.hpp
	src/Graphics/Models/Draw.hpp
	src/Lights/Extensions.hpp
	src/Lights/Components/DirectionalLight.hpp
	src/Lights/Components/Light.hpp
	src/Lights/Components/LightPerspective.hpp
	src/Lights/Components/LightPerspectiveCollection.hpp
	src/Lights/Components/PointLight.hpp
	src/Lights/Components/Spotlight.hpp
	src/Lights/Models/DirectionalLight.hpp
	src/Lights/Models/Light.hpp
	src/Lights/Models/LightPerspective.hpp
	src/Lights/Models/PointLight.hpp
	src/Lights/Models/Spotlight.hpp
	src/Lights/Systems/DirectionalLightSystem.hpp
	src/Lights/Systems/LightPerspectiveSystem.hpp
	src/Lights/Systems/LightSystem.hpp
	src/Lights/Systems/PointLightSystem.hpp
	src/Lights/Systems/ShadowMappingSystem.hpp
	src/Lights/Systems/SpotlightSystem.hpp
	src/Lights/Editors/LightEditor.hpp
	src/Lines/Extensions.hpp
	src/Lines/Components/Line.hpp
	src/Lines/Components/LineDraw.hpp
	src/Lines/Models/Line.hpp
	src/Lines/Models/LineDraw.hpp
	src/Lines/Models/LineVertex.hpp
	src/Lines/Pipelines/LinePipeline.hpp
	src/Lines/Systems/LineSystem.hpp
	src/Loaders/AssimpLoader.hpp
	src/Loaders/Extensions.hpp
	src/Loaders/LoadResult.hpp
	src/Materials/Extensions.hpp
	src/Materials/Components/Material.hpp
	src/Materials/Components/MaterialCollection.hpp
	src/Materials/Components/MaterialReference.hpp
	src/Materials/Models/Material.hpp
	src/Materials/Systems/MaterialSystem.hpp
	src/Meshes/Extensions.hpp
	src/Meshes/Vertex.hpp
	src/Meshes/Components/HasShadowDraw.hpp
	src/Meshes/Components/Mesh.hpp
	src/Meshes/Components/MeshBufferView.hpp
	src/Meshes/Components/MeshDraw.hpp
	src/Meshes/Components/MeshResource.hpp
	src/Meshes/Components/ShadowMeshDraw.hpp
	src/Meshes/Models/Mesh.hpp
	src/Meshes/Models/MeshDraw.hpp
	src/Meshes/Models/ShadowMeshDraw.hpp
	src/Meshes/Pipelines/MeshPipeline.hpp
	src/Meshes/Pipelines/ShadowMeshPipeline.hpp
	src/Meshes/Resources/MeshResourceManager.hpp
	src/Meshes/Resources/MeshUploadResult.hpp
	src/Meshes/Systems/MeshShadowSystem.hpp
	src/Meshes/Systems/MeshSystem.hpp
	src/Meshes/Systems/MeshSystemOperation.hpp
	src/Nodes/Extensions.hpp
	src/Nodes/Components/Node.hpp
	src/Nodes/Components/NodeMesh.hpp
	src/Nodes/Components/NodeMeshCollection.hpp
	src/Nodes/Models/Node.hpp
	src/Nodes/Systems/NodeSystem.hpp
	src/Pipelines/ComputePipeline.hpp
	src/Pipelines/GraphicsPipeline.hpp
	src/Planes/Components/Plane.hpp
	src/Planes/Models/Plane.hpp
	src/Planes/Systems/PlaneSystem.hpp
	src/Points/Extensions.hpp
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
	src/Systems/StorageSystem.hpp
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
    src/GlmExtensions.cpp
	src/main.cpp
	src/Animations/Pipelines/SkinningPipeline.cpp
	src/Animations/Resources/AnimationFrameResource.cpp
	src/Animations/Resources/AnimationResourceManager.cpp
	src/Animations/Systems/AnimationSystem.cpp
	src/Animations/Systems/BoneMeshSystem.cpp
	src/Animations/Systems/BoneSpatialSystem.cpp
	src/Animations/Systems/VertexWeightSystem.cpp
	src/Applications/Application.cpp
	src/Applications/Application.UserInterface.cpp
	src/Applications/Root.UserInterface.cpp
	src/Applications/Root.cpp
	src/BoundingVolumes/Components/AxisAlignedBoundingBox.cpp
	src/BoundingVolumes/Models/Vertex.cpp
	src/BoundingVolumes/Pipelines/BoundingVolumePipeline.cpp
	src/BoundingVolumes/Systems/AxisAlignedBoundingBoxSystem.cpp
	src/Cameras/Systems/CameraSystem.cpp
	src/Common/Common.cpp
	src/Common/IndexGenerator.cpp
	src/Common/KGuillotineAllocator.cpp
	src/Configuration/Configuration.cpp
	src/Controllers/FlyCamController.cpp
	src/Debugging/Renderers/DebugRenderer.cpp
	src/Devices/Device.cpp
	src/Devices/DeviceContext.cpp
	src/Devices/Image.cpp
	src/Devices/Texture.cpp
	src/Devices/VulkanInstanceConfiguration.cpp
	src/Engine/DescriptorSetAllocator.cpp
	src/Engine/DescriptorSetLayoutCache.cpp
	src/Engine/DescriptorSetLayoutCreateInfoEqualityComparer.cpp
	src/Engine/DescriptorSetLayouts.cpp
	src/Engine/EngineState.cpp
	src/Engine/FrameState.cpp
	src/Frustums/Components/Frustum.cpp
	src/Frustums/Models/Vertex.cpp
	src/Frustums/Pipelines/FrustumPipeline.cpp
	src/Frustums/Systems/FrustumSystem.cpp
	src/Graphics/GlobalSystem.cpp
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
	src/Lights/Editors/LightEditor.cpp
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

add_executable(drk ${HEADERS} ${SOURCES} src/Applications/ApplicationState.hpp)
add_dependencies(drk Shaders)
file (GENERATE OUTPUT settings.json INPUT ${PROJECT_SOURCE_DIR}/settings.json)
file (GENERATE OUTPUT imgui.ini INPUT ${PROJECT_SOURCE_DIR}/imgui.ini)

find_package(fruit CONFIG)
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
find_package(imguizmo CONFIG REQUIRED)
find_package(Taskflow CONFIG REQUIRED NO_CMAKE_PACKAGE_REGISTRY)

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
target_link_libraries(drk PRIVATE imguizmo::imguizmo)
target_link_libraries(drk PUBLIC Taskflow::Taskflow)
#target_link_libraries(drk PUBLIC fruit)
target_link_libraries(drk PUBLIC ${CMAKE_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/lib/fruit.lib)