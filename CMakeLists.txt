cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
project(drk CXX)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(dependencies)
file(
    GLOB SOURCES
    src/GlmExtensions.hpp
    src/implemantations.hpp
    src/system.hpp
    src/Applications/Application.hpp
    src/Applications/Extentions.hpp
    src/BoundingVolumes/Components/AxisAlignedBoundingBox.hpp
    src/BoundingVolumes/Systems/AxisAlignedBoundingBoxSystem.hpp
    src/BoundingVolumes/Models/Vertex.hpp
    src/Cameras/Systems/CameraSystem.hpp
    src/Cameras/Extensions.hpp
    src/Cameras/Components/Camera.hpp
    src/Cameras/Models/Camera.hpp
    src/Common/Common.hpp
    src/Common/ComponentIndex.hpp
    src/Common/IndexGenerator.hpp
    src/Common/KGuillotineAllocator.hpp
    src/Configuration/Extensions.hpp
    src/Controllers/Extensions.hpp
    src/Controllers/FlyCamController.hpp
    src/Devices/Buffer.hpp
    src/Devices/BufferUploadResult.hpp
    src/Devices/BufferView.hpp
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
    src/Draws/Systems/DrawSystem.hpp
    src/Draws/Components/DrawVertexBufferInfo.hpp
    src/Draws/Extentions.hpp
    src/Engine/DescriptorSetAllocator.hpp
    src/Engine/DescriptorSetLayoutCache.hpp
    src/Engine/DescriptorSetLayoutCreateInfoEqualityComparer.hpp
    src/Engine/DescriptorSetLayouts.hpp
    src/Engine/EngineState.hpp
    src/Engine/Extensions.hpp
    src/Engine/FrameState.hpp
    src/Engine/MeshUploadResult.hpp
    src/Engine/Models/Global.hpp
    src/Frustums/Components/Frustum.hpp
    src/Frustums/Models/Frustum.hpp
    src/Frustums/Systems/FrustumSystem.hpp
    src/Geometries/Extensions.hpp
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
    src/Lights/Systems/SpotlightSystem.hpp
    src/Lights/Systems/ShadowMappingSystem.hpp
    src/Lines/Extensions.hpp
    src/Lines/Pipelines/LinePipeline.hpp
    src/Lines/Systems/LineSystem.hpp
    src/Lines/Components/Line.hpp
    src/Lines/Components/LineDraw.hpp
    src/Lines/Models/Line.hpp
    src/Lines/Models/LineDraw.hpp
    src/Lines/Models/LineVertex.hpp
    src/Loaders/AssimpLoader.hpp
    src/Loaders/Extensions.hpp
    src/Loaders/LoadResult.hpp
    src/Materials/Extensions.hpp
    src/Materials/Systems/MaterialSystem.hpp
    src/Materials/Components/Material.hpp
    src/Materials/Components/MaterialCollection.hpp
    src/Materials/Models/Material.hpp
    src/Meshes/Extensions.hpp
    src/Meshes/Systems/MeshSystem.hpp
    src/Meshes/Vertex.hpp
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
    src/Objects/Components/Dirty.hpp
    src/Objects/Extensions.hpp
    src/Objects/Components/Object.hpp
    src/Objects/Components/ObjectMesh.hpp
    src/Objects/Systems/ObjectSystem.hpp
    src/Objects/Systems/ObjectMeshSystem.hpp
    src/Objects/Components/Relationship.hpp
    src/Objects/Models/Object.hpp
    src/Pipelines/Pipeline.hpp
    src/Planes/Components/Plane.hpp
    src/Planes/Models/Plane.hpp
    src/Planes/Systems/PlaneSystem.hpp
    src/Points/Extensions.hpp
    src/Points/Pipelines/PointPrimitivePipeline.hpp
    src/Points/Systems/PointSystem.hpp
    src/Points/Components/Point.hpp
    src/Points/Components/PointDraw.hpp
    src/Points/Models/Point.hpp
    src/Points/Models/PointDraw.hpp
    src/Points/Models/PointVertex.hpp
    src/Relationships/Extensions.hpp
    src/Relationships/Systems/RelationshipSystem.hpp
    src/Renderers/Renderer.hpp
    src/Scenes/Extensions.hpp
    src/Scenes/Systems/SceneSystem.hpp
    src/Scenes/Draws/SceneDraw.hpp
    src/Scenes/Draws/ShadowSceneDraw.hpp
    src/Scenes/Renderers/SceneRenderer.hpp
    src/Scenes/Renderers/SceneRenderOperation.hpp
    src/Scenes/Renderers/ShadowSceneRenderer.hpp
    src/Spatials/Extensions.hpp
    src/Spatials/Systems/SpatialSystem.hpp
    src/Spatials/Components/Spatial.hpp
    src/Spatials/Components/SpatialEditor.hpp
    src/Spatials/Models/Spatial.hpp
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
    src/Textures/Systems/TextureSystem.hpp
    src/Textures/TextureType.hpp
    src/UserInterfaces/AssetExplorer.hpp
    src/UserInterfaces/Extensions.hpp
    src/UserInterfaces/UserInterface.hpp
    src/UserInterfaces/Renderers/UserInterfaceRenderer.hpp
    src/Windows/Extensions.hpp
    src/Windows/Window.hpp
    src/Windows/WindowConfiguration.hpp
    src/GlmExtensions.cpp
    src/main.cpp
    src/Applications/Application.cpp
    src/Applications/Application.UserInterface.cpp
    src/BoundingVolumes/Systems/AxisAlignedBoundingBoxSystem.cpp
    src/BoundingVolumes/Models/Vertex.cpp
    src/Cameras/Systems/CameraSystem.cpp
    src/Common/Common.cpp
    src/Common/IndexGenerator.cpp
    src/Common/KGuillotineAllocator.cpp
    src/Controllers/FlyCamController.cpp
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
    src/Lights/Systems/SpotlightSystem.cpp
    src/Lights/Systems/ShadowMappingSystem.cpp
    src/Lines/Pipelines/LinePipeline.cpp
    src/Lines/Systems/LineSystem.cpp
    src/Lines/Models/LineVertex.cpp
    src/Loaders/AssimpLoader.cpp
    src/Materials/Systems/MaterialSystem.cpp
    src/Meshes/Systems/MeshSystem.cpp
    src/Meshes/Vertex.cpp
    src/Meshes/Pipelines/MeshPipeline.cpp
    src/Meshes/Pipelines/ShadowMeshPipeline.cpp
    src/Objects/Systems/ObjectSystem.cpp
    src/Objects/Systems/ObjectMeshSystem.cpp
    src/Planes/Systems/PlaneSystem.cpp
    src/Points/Pipelines/PointPrimitivePipeline.cpp
    src/Points/Systems/PointSystem.cpp
    src/Points/Models/PointVertex.cpp
    src/Relationships/Systems/RelationshipSystem.cpp
    src/Renderers/Renderer.cpp
    src/Scenes/Systems/SceneSystem.cpp
    src/Scenes/Renderers/SceneRenderer.cpp
    src/Scenes/Renderers/ShadowSceneRenderer.cpp
    src/Spatials/Systems/SpatialSystem.cpp
    src/Spatials/Components/SpatialEditor.cpp
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
    src/BoundingVolumes/Components/AxisAlignedBoundingBox.cpp 
    src/Planes/Components/Plane.cpp
    src/Frustums/Components/Frustum.cpp
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
    "src/Lines/Shaders/BoundingVolume.vert"
    "src/Lines/Shaders/BoundingVolume.frag"
)

foreach (GLSL ${GLSL_SOURCES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${PROJECT_BINARY_DIR}/shaders/spv/${FILE_NAME}.spv")
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/shaders/spv/"
        COMMAND glslc -g --target-env=vulkan1.3 ${GLSL} -o ${SPIRV}
        COMMAND
        DEPENDS ${GLSL})
    list(APPEND SPIRV_BINARIES ${SPIRV})
endforeach (GLSL)

add_custom_target(
    Shaders
    DEPENDS ${SPIRV_BINARIES}
)

add_executable(drk ${SOURCES} "src/BoundingVolumes/Pipelines/BoundingVolumePipeline.hpp" "src/BoundingVolumes/Pipelines/BoundingVolumePipeline.cpp" "src/Meshes/Systems/MeshShadowSystem.cpp" "src/Meshes/Systems/MeshShadowSystem.hpp" "src/Debugging/Renderers/DebugRenderer.hpp" "src/Debugging/Renderers/DebugRenderer.cpp")
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