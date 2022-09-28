cmake_minimum_required(VERSION 3.14)

project(drk CXX)
set(CMAKE_CXX_STANDARD 20)

file(
    GLOB SOURCES
    src/main.cpp
    src/Devices/Device.cpp
    src/Meshes/Vertex.cpp
    src/Meshes/Vertex.cpp
    src/Textures/Image.cpp
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
    src/Graphics/Store.cpp
    src/Graphics/DescriptorSetLayoutCreateInfoEqualityComparer.cpp
    src/Graphics/DescriptorSetLayoutCache.cpp
    src/Graphics/DescriptorSetAllocator.cpp
    src/Applications/Application.cpp
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
    src/Textures/Image.hpp
    src/Textures/TextureType.hpp
    src/Indexation.hpp
    src/Meshes/Mesh.hpp
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
    src/Graphics/Store.hpp
    src/Graphics/DescriptorSetLayoutCreateInfoEqualityComparer.hpp
    src/Graphics/DescriptorSetLayoutCache.hpp
    src/Graphics/DescriptorSetAllocator.hpp
    src/Applications/Application.hpp
)

add_executable(drk ${HEADERS} ${SOURCES})

find_package(Vulkan REQUIRED)
find_package(unofficial-vulkan-memory-allocator REQUIRED)
find_package(entt REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(assimp REQUIRED)
find_package(imgui REQUIRED)
find_package(stb REQUIRED)

target_include_directories(drk PUBLIC ${Vulkan_INCLUDE_DIRS})
target_include_directories(drk PUBLIC ${stb_INCLUDE_DIRS})

target_link_libraries(drk PRIVATE ${Vulkan_LIBRARIES})
target_link_libraries(drk PRIVATE unofficial::vulkan-memory-allocator::vulkan-memory-allocator)
target_link_libraries(drk PRIVATE glfw)
target_link_libraries(drk PRIVATE EnTT::EnTT)
target_link_libraries(drk PRIVATE glm::glm)
target_link_libraries(drk PRIVATE assimp::assimp)
target_link_libraries(drk PRIVATE imgui::imgui)