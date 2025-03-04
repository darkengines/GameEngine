include(FetchContent)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY "https://github.com/ocornut/imgui"
    GIT_TAG origin/docking
)

FetchContent_GetProperties(imgui)
if (NOT imgui_POPULATED)
    FetchContent_Populate(imgui)
    set(IMGUI_INCLUDE_DIR ${imgui_SOURCE_DIR}/ PARENT_SCOPE)
    file(GLOB IMGUI_SOURCES ${imgui_SOURCE_DIR}/*.cpp)
    file(GLOB IMGUI_HEADERS ${imgui_SOURCE_DIR}/*.h)
    add_library(imgui ${IMGUI_SOURCES} ${IMGUI_SOURCES})
endif ()

FetchContent_Declare(
    imgui-filebrowser
    GIT_REPOSITORY https://github.com/AirGuanZ/imgui-filebrowser
    GIT_TAG origin/master
)

FetchContent_MakeAvailable(imgui-filebrowser)

set(imgui-filebrowser ${imgui-filebrowser_SOURCE_DIR} PARENT_SCOPE)
set(imgui_SOURCE_DIR ${imgui_SOURCE_DIR} PARENT_SCOPE)

FetchContent_Declare(
    imguizmo
    GIT_REPOSITORY "https://github.com/CedricGuillemet/imguizmo"
    GIT_TAG origin/master  # Use latest master branch
)

FetchContent_GetProperties(imguizmo)
if (NOT imguizmo_POPULATED)
    FetchContent_Populate(imguizmo)
    set(IMGUIZMO_INCLUDE_DIR ${imguizmo_SOURCE_DIR}/ PARENT_SCOPE)
    file(GLOB IMGUIZMO_SOURCES ${imguizmo_SOURCE_DIR}/*.cpp)
    file(GLOB IMGUIZMO_HEADERS ${imguizmo_SOURCE_DIR}/*.h)
    add_library(imguizmo ${IMGUIZMO_SOURCES} ${IMGUIZMO_SOURCES})

    target_include_directories(imguizmo PUBLIC 
        ${imguizmo_SOURCE_DIR}
        ${imgui_SOURCE_DIR}  # Ensure imguizmo uses locally built ImGui
    )

    target_link_libraries(imguizmo PUBLIC imgui)
endif ()

set(imguizmo_INCLUDE_DIR ${imguizmo_SOURCE_DIR} PARENT_SCOPE)
