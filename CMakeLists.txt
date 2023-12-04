cmake_minimum_required(VERSION 3.14)

project(drk CXX)
set(CMAKE_CXX_STANDARD 20)
add_subdirectory(dependencies)
file(
    GLOB SOURCES
    src/main.cpp
    src/Devices/Device.cpp
    src/Meshes/Vertex.cpp
    src/Meshes/Vertex.cpp
    src/Textures/ImageInfo.cpp
    src/Devices/Texture.cpp
    src/Windows/Window.cpp
    src/Geometries/Plane.cpp
    src/Geometries/AxisAlignedBoundingBox.cpp
    src/GlmExtensions.cpp
    src/Graphics/Graphics.cpp
    src/Devices/DeviceContext.cpp
    src/Common/Common.cpp
    src/Engine/EngineState.cpp
    src/Engine/FrameState.cpp
    src/Stores/StoreBuffer.cpp
    src/Engine/DescriptorSetLayoutCreateInfoEqualityComparer.cpp
    src/Engine/DescriptorSetLayoutCache.cpp
    src/Engine/DescriptorSetAllocator.cpp
    src/Applications/Application.cpp
    src/Loaders/AssimpLoader.cpp
    src/Common/IndexGenerator.cpp
    src/Textures/TextureSystem.cpp
    src/Devices/Image.cpp
    src/Meshes/MeshSystem.cpp
    src/Materials/MaterialSystem.cpp
    src/Spatials/SpatialSystem.cpp
    src/Objects/ObjectSystem.cpp
    src/Graphics/SynchronizationState.cpp
    src/Stores/Store.cpp
    src/Stores/StoreBufferAllocator.cpp
    src/Objects/Dirty.cpp
    src/Cameras/CameraSystem.cpp
    src/Relationships/RelationshipSystem.cpp
    src/Lights/SpotlightSystem.cpp
    src/Stores/TextureStore.cpp
    src/Graphics/GlobalSystem.cpp
    src/Controllers/FlyCamController.cpp
    src/Lights/DirectionalLightSystem.cpp
    src/Lights/LightSystem.cpp
    src/Lights/PointLightSystem.cpp
    src/ImGui/imgui_impl_vulkan.cpp
    src/ImGui/imgui_impl_glfw.cpp
    src/UserInterfaces/UserInterface.cpp
    src/Devices/VulkanInstanceConfiguration.cpp
    src/Points/PointPrimitivePipeline.cpp
    src/Points/Models/PointVertex.cpp
    src/Systems/System.cpp
    src/Points/PointSystem.cpp
    src/Engine/DescriptorSetLayouts.cpp
    src/Meshes/Pipelines/MeshPipeline.cpp
    src/Renderers/Renderer.cpp
    src/Scenes/Renderers/SceneRenderer.cpp
    src/Scenes/SceneSystem.cpp 
    src/Draws/DrawVertexBufferInfo.cpp 
    src/UserInterfaces/Renderers/UserInterfaceRenderer.cpp 
    src/Spatials/Components/SpatialEditor.cpp 
    src/UserInterfaces/AssetExplorer.cpp 
    src/ImGui/imgui_stdlib.cpp
    src/Lines/LinePipeline.cpp
    src/Lines/Models/LineVertex.cpp
    src/Lines/LineSystem.cpp
)

file(
    GLOB HEADERS
    src/implemantations.hpp
    src/Devices/DeviceContext.hpp
    src/Devices/QueueFamilyIndices.hpp
    src/Devices/SwapChainSupportDetails.hpp
    src/Devices/VulkanLogicalDeviceInfo.hpp
    src/Devices/Device.hpp
    src/system.hpp
    src/Meshes/Vertex.hpp
    src/Meshes/Vertex.hpp
    src/Textures/ImageInfo.hpp
    src/Textures/TextureType.hpp
    src/Common/ComponentIndex.hpp
    src/Meshes/Components/MeshResource.hpp
    src/Materials/Components/Material.hpp
    src/Devices/Texture.hpp
    src/Devices/BufferView.hpp
    src/Devices/Buffer.hpp
    src/Devices/Swapchain.hpp
    src/Windows/Window.hpp
    src/Lights/Light.hpp
    src/Cameras/Components/Camera.hpp
    src/Spatials/Components/Spatial.hpp
    src/Lights/Models/Spotlight.hpp
    src/Lights/Components/Spotlight.hpp
    src/Lights/PointLight.hpp
    src/Lights/Models/DirectionalLight.hpp
    src/Geometries/Plane.hpp
    src/Geometries/AxisAlignedBoundingBox.hpp
    src/Geometries/Frustum.cpp src/Geometries/Frustum.hpp
    src/GlmExtensions.hpp
    src/Graphics/Graphics.hpp
    src/Common/Common.hpp
    src/Engine/EngineState.hpp
    src/Engine/FrameState.hpp
    src/Stores/StoreBuffer.hpp
    src/Engine/DescriptorSetLayoutCreateInfoEqualityComparer.hpp
    src/Engine/DescriptorSetLayoutCache.hpp
    src/Engine/DescriptorSetAllocator.hpp
    src/Applications/Application.hpp
    src/Loaders/AssimpLoader.hpp
    src/Loaders/LoadResult.hpp
    src/Common/IndexGenerator.hpp
    src/Meshes/MeshGroup.hpp
    src/Objects/Relationship.hpp
    src/Textures/TextureSystem.hpp
    src/Devices/Image.hpp
    src/Meshes/MeshSystem.hpp
    src/Materials/MaterialSystem.hpp
    src/Devices/BufferUploadResult.hpp
    src/Engine/MeshUploadResult.hpp
    src/Meshes/Components/MeshBufferView.hpp
    src/Graphics/SynchronizationState.hpp
    src/Materials/Models/Material.hpp
    src/Stores/StoreBufferAllocator.hpp
    src/Stores/Store.hpp
    src/Stores/StoreItem.hpp
    src/Graphics/Draw.hpp
    src/Graphics/Models/Draw.hpp
    src/Meshes/Models/MeshDraw.hpp
    src/Stores/Models/StoreItemLocation.hpp
    src/Objects/Object.hpp
    src/Draws/DrawCommand.hpp
    src/Meshes/Models/Mesh.hpp
    src/Objects/Models/Object.hpp
    src/Objects/ObjectSystem.hpp
    src/Spatials/SpatialSystem.hpp
    src/Spatials/Models/Spatial.hpp
    src/Objects/Dirty.hpp
    src/Graphics/DrawSet.hpp
    src/Graphics/DrawContext.hpp
    src/Cameras/CameraSystem.hpp
    src/Cameras/Models/Camera.hpp
    src/Relationships/RelationshipSystem.hpp
    src/Stores/StoreItemLocation.hpp
    src/Engine/Models/Global.hpp
    src/Lights/SpotlightSystem.hpp
    src/Stores/TextureStore.hpp
    src/Graphics/GlobalSystem.hpp
    src/Controllers/FlyCamController.hpp
    src/Lights/DirectionalLightSystem.hpp
    src/Lights/LightSystem.hpp
    src/Lights/PointLightSystem.hpp
    src/UserInterfaces/UserInterface.hpp
    src/Devices/Extensions.hpp
    src/Windows/Extensions.hpp
    src/Configuration/Extensions.hpp
    src/Windows/WindowConfiguration.hpp
    src/Devices/VulkanInstanceConfiguration.hpp
    src/Graphics/Extentions.hpp
    src/Applications/Extentions.hpp
    src/Cameras/Extensions.hpp
    src/Lights/Extensions.hpp
    src/Loaders/Extensions.hpp
    src/Materials/Extensions.hpp
    src/Meshes/Extensions.hpp
    src/Objects/Extensions.hpp
    src/Relationships/Extensions.hpp
    src/Spatials/Extensions.hpp
    src/Textures/Extensions.hpp
    src/Controllers/Extensions.hpp
    src/UserInterfaces/Extensions.hpp
    src/Points/PointPrimitivePipeline.hpp
    src/Points/Models/PointVertex.hpp
    src/Points/Components/PointDraw.hpp
    src/Points/Components/Point.hpp
    src/Geometries/Primitives.hpp
    src/Systems/System.hpp
    src/Points/PointSystem.hpp
    src/Points/Models/Point.hpp
    src/Draws/Draw.hpp
    src/Points/Models/PointDraw.hpp
    src/Draws/Extentions.hpp
    src/Stores/Extensions.hpp
    src/Engine/DescriptorSetLayouts.hpp
    src/Engine/Extensions.hpp
    src/Meshes/Components/Draw.hpp
    src/Meshes/Pipelines/MeshPipeline.hpp
    src/Draws/DrawSet.hpp
    src/Scenes/Renderers/SceneRenderer.hpp
    src/Pipelines/Pipeline.hpp
    src/Renderers/Renderer.hpp
    src/Scenes/SceneSystem.hpp 
    src/Scenes/Draws/SceneDraw.hpp 
    src/Meshes/Components/MeshDraw.hpp 
    src/Draws/DrawSystem.hpp 
    src/Draws/DrawVertexBufferInfo.hpp 
    src/Devices/ImageInfo.hpp 
    src/UserInterfaces/Renderers/UserInterfaceRenderer.hpp 
    src/Devices/DeviceImageInfo.hpp 
    src/Scenes/Renderers/SceneRenderOperation.hpp 
    src/Spatials/Components/SpatialEditor.hpp 
    src/UserInterfaces/AssetExplorer.hpp 
    src/Materials/Components/MaterialCollection.hpp 
    src/Meshes/Components/Mesh.hpp
    src/ImGui/imgui_stdlib.h
)

file(
    GLOB GLSL_SOURCES
    "src/Meshes/Shaders/Mesh.vert"
    "src/Meshes/Shaders/Mesh.frag"
    "src/Points/Shaders/Point.vert"
    "src/Points/Shaders/Point.frag"
    "src/Lines/Shaders/Line.vert"
    "src/Lines/Shaders/Line.frag"
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

add_executable(drk ${HEADERS} ${SOURCES} src/Lights/Components/DirectionalLight.hpp)
add_dependencies(drk Shaders)
file (GENERATE OUTPUT settings.json INPUT ${PROJECT_SOURCE_DIR}/settings.json)

find_package(Vulkan REQUIRED)
find_package(VulkanMemoryAllocator REQUIRED)
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