cmake_minimum_required(VERSION 3.14)

project(drk CXX)
add_subdirectory(dependencies)
set(CMAKE_CXX_STANDARD 20)

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
    src/Graphics/EngineState.cpp
    src/Graphics/FrameState.cpp
    src/Stores/StoreBuffer.cpp
    src/Graphics/DescriptorSetLayoutCreateInfoEqualityComparer.cpp
    src/Graphics/DescriptorSetLayoutCache.cpp
    src/Graphics/DescriptorSetAllocator.cpp
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
    src/Meshes/MeshInfo.hpp
    src/Materials/Material.hpp
    src/Devices/Texture.hpp
    src/Devices/BufferView.hpp
    src/Devices/Buffer.hpp
    src/Devices/Swapchain.hpp
    src/Windows/Window.hpp
    src/Lights/Light.hpp
    src/Cameras/Camera.hpp
    src/Spatials/Spatial.hpp
    src/Lights/Spotlight.hpp
    src/Lights/PointLight.hpp
    src/Lights/DirectionalLight.hpp
    src/Geometries/Plane.hpp
    src/Geometries/AxisAlignedBoundingBox.hpp
    src/Geometries/Frustum.cpp src/Geometries/Frustum.hpp
    src/GlmExtensions.hpp
    src/Graphics/Graphics.hpp
    src/Common/Common.hpp
    src/Graphics/EngineState.hpp
    src/Graphics/FrameState.hpp
    src/Stores/StoreBuffer.hpp
    src/Graphics/DescriptorSetLayoutCreateInfoEqualityComparer.hpp
    src/Graphics/DescriptorSetLayoutCache.hpp
    src/Graphics/DescriptorSetAllocator.hpp
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
    src/Graphics/MeshUploadResult.hpp
    src/Meshes/Mesh.hpp
    src/Graphics/SynchronizationState.hpp
    src/Materials/Models/Material.hpp
    src/Stores/StoreBufferAllocator.hpp
    src/Stores/Store.hpp
    src/Stores/StoreItem.hpp
    src/Graphics/Draw.hpp
    src/Graphics/Models/Draw.hpp
    src/Graphics/Models/StoreItemLocation.hpp
    src/Objects/Object.hpp
    src/Graphics/DrawCommand.hpp
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
    src/Graphics/Models/Global.hpp
    src/Lights/SpotlightSystem.hpp
    src/Stores/TextureStore.hpp
    src/Graphics/GlobalSystem.hpp
    src/Controllers/FlyCamController.hpp
    src/Lights/DirectionalLightSystem.hpp
    src/Lights/LightSystem.hpp
    src/Lights/PointLightSystem.hpp
)

file(
    GLOB GLSL_SOURCES
    "src/Graphics/Shaders/main.vert"
    "src/Graphics/Shaders/main.frag"
)

foreach(GLSL ${GLSL_SOURCES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${PROJECT_BINARY_DIR}/shaders/spv/${FILE_NAME}.spv")
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/shaders/spv/"
        #COMMAND glslc -g --target-env=vulkan1.3 ${GLSL} -o ${SPIRV}
        COMMAND glslc --target-env=vulkan1.3 ${GLSL} -o ${SPIRV}
        DEPENDS ${GLSL})
    list(APPEND SPIRV_BINARIES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    Shaders
    DEPENDS ${SPIRV_BINARIES}
)

add_executable(drk ${HEADERS} ${SOURCES} src/UserInterfaces/UserInterface.cpp src/UserInterfaces/UserInterface.hpp)
add_dependencies(drk Shaders)

find_package(Vulkan REQUIRED)
find_package(unofficial-vulkan-memory-allocator REQUIRED)
find_package(entt REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(assimp REQUIRED)
find_package(imgui REQUIRED)
find_package(stb REQUIRED)
find_package(fmt REQUIRED)

target_include_directories(drk PUBLIC ${Vulkan_INCLUDE_DIRS})
target_include_directories(drk PUBLIC ${stb_INCLUDE_DIRS})
target_include_directories(drk PUBLIC ${imgui-filebrowser})

target_link_libraries(drk PRIVATE ${Vulkan_LIBRARIES})
target_link_libraries(drk PRIVATE unofficial::vulkan-memory-allocator::vulkan-memory-allocator)
target_link_libraries(drk PRIVATE glfw)
target_link_libraries(drk PRIVATE EnTT::EnTT)
target_link_libraries(drk PRIVATE glm::glm)
target_link_libraries(drk PRIVATE assimp::assimp)
target_link_libraries(drk PRIVATE imgui::imgui)
target_link_libraries(drk PRIVATE fmt::fmt)