#include "imgui.h"
#include <iostream>
#include <fmt/format.h>
#include "imgui_impl_nested.h"

#ifndef IMGUI_DISABLE

#include <cfloat>   // For FLT_MAX
#include <cstdio>   // For debug logs if needed
#include <cstring>  // For memset, memcpy

// Forward-declare your NestedWindow functions that you'll need.
// For example, you might have:
float NestedWindow_GetTime(NestedWindow* window)
{
  // For simplicity, return GLFW's global time
  // (Or store a creation time if you need your own reference)
  return (float)glfwGetTime();
}

void NestedWindow_GetSize(NestedWindow* window, int* width, int* height)
{
  int w, h;
  glfwGetWindowSize(window->handle, &w, &h);
  *width = w;
  *height = h;
}
// Possibly: NestedWindow_SetCallback_WindowFocus(...)
// Possibly: NestedWindow_SetCallback_MouseButton(...)
// etc.
//
// If your system doesn’t have such helper functions, you’ll implement them inline
// or replace these placeholders with your own logic.

//-----------------------------------------------------------------------------
// BACKEND DATA
//-----------------------------------------------------------------------------

// “ImGui_ImplNested_Data” parallels “ImGui_ImplGlfw_Data” in the official GLFW backend.
// It holds data we need to operate the backend, including references to the NestedWindow*.
struct ImGui_ImplNested_Data
{
  NestedWindow* Window;
  double Time;
  bool InstalledCallbacks;

  // If chaining user callbacks, store the old ones here:
  // e.g. void (*PrevUserCallbackWindowFocus)(NestedWindow*,bool) = nullptr;
  // etc.

  // Optional: track the last known mouse positions, etc.
  // In official backends, we may store them for multi-viewport or other uses.

  ImGui_ImplNested_Data()
  {
    Window = nullptr;
    Time = 0.0;
    InstalledCallbacks = false;
  }
};

// Helper: retrieve our backend data from ImGui
static ImGui_ImplNested_Data* ImGui_ImplNested_GetBackendData()
{
  return ImGui::GetCurrentContext() ? (ImGui_ImplNested_Data*)ImGui::GetIO().BackendPlatformUserData : nullptr;
}

// Helper: key conversion if you want to fully use 1.87+ style
// (You can expand this as needed)
static ImGuiKey ImGui_ImplNested_TranslateKey(int key)
{
  // Example mapping:
  // if (key == MY_KEY_A) return ImGuiKey_A;
  // if (key == MY_KEY_SPACE) return ImGuiKey_Space;
  // ...
  return ImGuiKey_None;
}

// Forward declare some static functions (for installing/restoring callbacks)
static void ImGui_ImplNested_InitCallbacks(NestedWindow* window);
static void ImGui_ImplNested_RestoreCallbacksImpl(NestedWindow* window);

//-----------------------------------------------------------------------------
// INIT/SHUTDOWN
//-----------------------------------------------------------------------------

static bool ImGui_ImplNested_Init(NestedWindow* window, bool install_callbacks)
{
  ImGuiIO& io = ImGui::GetIO();
  IM_ASSERT(io.BackendPlatformUserData == nullptr && "Already initialized a platform backend?");
  IM_ASSERT(io.BackendPlatformName == nullptr && "Already initialized a platform backend name?");

  // Setup backend capabilities flags
  // e.g. io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
  // e.g. io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
  // If using multi-viewports: io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;

  // Create backend data
  ImGui_ImplNested_Data* bd = IM_NEW(ImGui_ImplNested_Data)();
  bd->Window = window;
  bd->Time = 0.0;

  bd->InstalledCallbacks = install_callbacks;
  io.BackendPlatformUserData = (void*)bd;
  io.Fonts = window->parentContext->IO.Fonts;
  io.BackendPlatformName = "imgui_impl_nested";

  if (install_callbacks)
    ImGui_ImplNested_InitCallbacks(window);

  return true;
}

bool ImGui_ImplNested_InitForOpenGL(NestedWindow* window, bool install_callbacks)
{
  return ImGui_ImplNested_Init(window, install_callbacks);
}

bool ImGui_ImplNested_InitForVulkan(NestedWindow* window, bool install_callbacks)
{
  return ImGui_ImplNested_Init(window, install_callbacks);
}

bool ImGui_ImplNested_InitForOther(NestedWindow* window, bool install_callbacks)
{
  return ImGui_ImplNested_Init(window, install_callbacks);
}

void ImGui_ImplNested_Shutdown()
{
  ImGuiIO& io = ImGui::GetIO();
  ImGui_ImplNested_Data* bd = (ImGui_ImplNested_Data*)io.BackendPlatformUserData;
  if (bd == nullptr)
    return;

  if (bd->InstalledCallbacks && bd->Window)
    ImGui_ImplNested_RestoreCallbacksImpl(bd->Window);

  // Clear backend data
  io.BackendPlatformUserData = nullptr;
  io.BackendPlatformName = nullptr;
  IM_DELETE(bd);
}

void ImGui_ImplNested_NewFrame()
{
  ImGui_ImplNested_Data* bd = ImGui_ImplNested_GetBackendData();
  IM_ASSERT(bd != nullptr && "Did you call ImGui_ImplNested_InitXXX()?");
  IM_ASSERT(bd->Window != nullptr && "NestedWindow handle is null?");

  ImGuiIO& io = ImGui::GetIO();

  // 1) Update Display Size
  int w, h;
  NestedWindow_GetSize(bd->Window, &w, &h);
  if (w > 0 && h > 0)
    io.DisplaySize = ImVec2((float)w, (float)h);
  else
    io.DisplaySize = ImVec2(640.f, 480.f);  // fallback or 0,0

  // 2) Update Delta Time
  double current_time = NestedWindow_GetTime(bd->Window);
  if (bd->Time > 0.0)
    io.DeltaTime = (float)(current_time - bd->Time);
  else
    io.DeltaTime = (float)(1.0f / 60.0f);
  bd->Time = current_time;

  // 3) [Optional] Update Mouse Pos, etc. here if your system lacks callbacks or you need extra checks.

  // 4) Start the frame
//  ImGui::NewFrame();
}

//-----------------------------------------------------------------------------
// CALLBACKS INSTALL/RESTORE
//-----------------------------------------------------------------------------

static void ImGui_ImplNested_InitCallbacks(NestedWindow* window)
{
  // Example pseudo-code if your NestedWindow has set-callback functions:
  // ImGui_ImplNested_Data* bd = ImGui_ImplNested_GetBackendData();
  // bd->PrevUserCallbackWindowFocus = NestedWindow_SetWindowFocusCallback(window, ImGui_ImplNested_WindowFocusCallback);
  // bd->PrevUserCallbackMouseButton = NestedWindow_SetMouseButtonCallback(window, ImGui_ImplNested_MouseButtonCallback);
  // ...
  // If you want to chain them, you store the old callbacks in bd->PrevUserCallbackXYZ
  // and in your new callback, you call them after handling ImGui.
}

static void ImGui_ImplNested_RestoreCallbacksImpl(NestedWindow* window)
{
  // e.g.
  // ImGui_ImplNested_Data* bd = ImGui_ImplNested_GetBackendData();
  // NestedWindow_SetWindowFocusCallback(window, bd->PrevUserCallbackWindowFocus);
  // ...
}

// Provide these for user convenience if they decide to install callbacks manually
void ImGui_ImplNested_InstallCallbacks(NestedWindow* window)
{
  // Essentially the same as ImGui_ImplNested_InitCallbacks, unless you want to do something special
  ImGui_ImplNested_InitCallbacks(window);
}

void ImGui_ImplNested_RestoreCallbacks(NestedWindow* window)
{
  ImGui_ImplNested_RestoreCallbacksImpl(window);
}

//-----------------------------------------------------------------------------
// CALLBACKS IMPLEMENTATION (the actual event handlers to feed ImGui)
//-----------------------------------------------------------------------------

void ImGui_ImplNested_WindowFocusCallback(NestedWindow* window, bool focused)
{
  ImGuiIO& io = ImGui::GetIO();
  io.AddFocusEvent(focused);

  // If chaining to user’s callback:
  // ImGui_ImplNested_Data* bd = ImGui_ImplNested_GetBackendData();
  // if (bd->PrevUserCallbackWindowFocus)
  //     bd->PrevUserCallbackWindowFocus(window, focused);
}

void ImGui_ImplNested_CursorEnterCallback(NestedWindow* window, bool entered)
{
  // You might not need to do anything. Or you could track mouse inside/outside.
  (void)window;
  (void)entered;
}

void ImGui_ImplNested_CursorPosCallback(NestedWindow* window, float x, float y)
{
  std::cout << fmt::format("ImGui_ImplNested_CursorPosCallback {:f} {:f}", x, y) << std::endl;
  // Forward absolute mouse coords. If you need to offset them (for a sub-region),
  // subtract your region's top-left from x,y first.
  ImGuiIO& io = ImGui::GetIO();
  io.AddMousePosEvent(x, y);

  // Chain user callback if needed
}

void ImGui_ImplNested_MouseButtonCallback(NestedWindow* window, int button, bool pressed)
{
  std::cout << fmt::format("ImGui_ImplNested_MouseButtonCallback {:d} {:d}", button, pressed) << std::endl;
  ImGuiIO& io = ImGui::GetIO();
  if (button >= 0 && button < IM_ARRAYSIZE(io.MouseDown))
    io.AddMouseButtonEvent(button, pressed);
}

void ImGui_ImplNested_ScrollCallback(NestedWindow* window, float xoffset, float yoffset)
{
  ImGuiIO& io = ImGui::GetIO();
  io.AddMouseWheelEvent(xoffset, yoffset);
}

void ImGui_ImplNested_KeyCallback(NestedWindow* window, int key, bool pressed, int mods)
{
  // 1) Translate your key to an ImGuiKey
  ImGuiKey imgui_key = ImGui_ImplNested_TranslateKey(key);
  if (imgui_key != ImGuiKey_None)
  {
    ImGuiIO& io = ImGui::GetIO();
    io.AddKeyEvent(imgui_key, pressed);
  }

  // 2) Mods
  //   Typically, we handle SHIFT, CTRL, ALT, SUPER:
  //   bool shift = (mods & MY_MOD_SHIFT) != 0;
  //   io.AddKeyEvent(ImGuiKey_ModShift, shift);
  //   ...
}

void ImGui_ImplNested_CharCallback(NestedWindow* window, unsigned int c)
{
  ImGuiIO& io = ImGui::GetIO();
  io.AddInputCharacter(c);
}

#endif  // #ifndef IMGUI_DISABLE
