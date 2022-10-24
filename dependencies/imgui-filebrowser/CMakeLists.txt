message(STATUS "Fetching imgui-filebrowser...")
FetchContent_MakeAvailable(imgui-filebrowser)
set(imgui-filebrowser ${imgui-filebrowser_SOURCE_DIR} PARENT_SCOPE)