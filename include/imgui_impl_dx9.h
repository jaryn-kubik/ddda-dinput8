// ImGui Win32 + DirectX9 binding
// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you use this binding you'll need to call 4 functions: ImGui_ImplXXXX_Init(), ImGui_ImplXXXX_NewFrame(), ImGui::Render() and ImGui_ImplXXXX_Shutdown().
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

#include <d3d9.h>
bool ImGui_ImplDX9_Init(struct IDirect3DDevice9* device);
void ImGui_ImplDX9_NewFrame();

// Use if you want to reset your rendering device without losing ImGui state.
void ImGui_ImplDX9_InvalidateDeviceObjects();
bool ImGui_ImplDX9_CreateDeviceObjects();

LRESULT ImGui_ImplDX9_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);