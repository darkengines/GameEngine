#pragma once
#include <GLFW/glfw3.h>
#include <imgui_internal.h>

// Minimal wrapper around a GLFWwindow
struct NestedWindow
{
  GLFWwindow* handle = nullptr;
  ImGuiContext* parentContext = nullptr;
};

// Retrieve a time value (in seconds). Usually just wraps glfwGetTime().
float NestedWindow_GetTime(NestedWindow* window);

// Retrieve the window's width & height.
void NestedWindow_GetSize(NestedWindow* window, int* width, int* height);

// The usage is analogous to imgui_impl_glfw:
//   1) ImGui_ImplNested_InitForVulkan(window, install_callbacks) or
//      ImGui_ImplNested_InitForOpenGL(window, install_callbacks), etc.
//   2) ImGui_ImplNested_NewFrame()
//   3) Call your renderer's NewFrame() + ImGui::NewFrame()
//   4) ImGui::Render(), then render the draw data with your chosen renderer.
//   5) On shutdown: ImGui_ImplNested_Shutdown().

IMGUI_IMPL_API bool ImGui_ImplNested_InitForOpenGL(NestedWindow* window, bool install_callbacks);
IMGUI_IMPL_API bool ImGui_ImplNested_InitForVulkan(NestedWindow* window, bool install_callbacks);
IMGUI_IMPL_API bool ImGui_ImplNested_InitForOther(NestedWindow* window, bool install_callbacks);

IMGUI_IMPL_API void ImGui_ImplNested_Shutdown();
IMGUI_IMPL_API void ImGui_ImplNested_NewFrame();

// Callback installation (optional). If you call Init with install_callbacks=true,
// these are called automatically. If you call Init with install_callbacks=false,
// you can manually install them yourself.
IMGUI_IMPL_API void ImGui_ImplNested_InstallCallbacks(NestedWindow* window);
IMGUI_IMPL_API void ImGui_ImplNested_RestoreCallbacks(NestedWindow* window);

// Individual callbacks. If you didn't install them automatically, you can call these manually
// from your own event handlers:
IMGUI_IMPL_API void ImGui_ImplNested_WindowFocusCallback(NestedWindow* window, bool focused);
IMGUI_IMPL_API void ImGui_ImplNested_CursorEnterCallback(NestedWindow* window, bool entered);
IMGUI_IMPL_API void ImGui_ImplNested_CursorPosCallback(NestedWindow* window, float x, float y);
IMGUI_IMPL_API void ImGui_ImplNested_MouseButtonCallback(NestedWindow* window, int button, bool pressed);
IMGUI_IMPL_API void ImGui_ImplNested_ScrollCallback(NestedWindow* window, float xoffset, float yoffset);
IMGUI_IMPL_API void ImGui_ImplNested_KeyCallback(NestedWindow* window, int key, bool pressed, int mods);
IMGUI_IMPL_API void ImGui_ImplNested_CharCallback(NestedWindow* window, unsigned int c);
