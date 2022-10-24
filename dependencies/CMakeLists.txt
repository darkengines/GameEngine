include(FetchContent)

FetchContent_Declare(
    imgui-filebrowser
    GIT_REPOSITORY https://github.com/AirGuanZ/imgui-filebrowser
    GIT_TAG origin/master
)
add_subdirectory(imgui-filebrowser)
message("SET imgui-filebrowser to ${imgui-filebrowser}")
set(imgui-filebrowser ${imgui-filebrowser} PARENT_SCOPE)