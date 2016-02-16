#include "stdafx.h"
#include "InGameGUI.h"
#include "d3d9.h"
#include "include/imgui_impl_dx9.h"

void createImGui(LPDIRECT3DDEVICE9 pD3DDevice, D3DPRESENT_PARAMETERS* pParams)
{
	ImGui_ImplDX9_Init(pD3DDevice);
	ImGui::GetIO().DisplaySize = ImVec2((float)pParams->BackBufferWidth, (float)pParams->BackBufferHeight);
}

void lostImGui(LPDIRECT3DDEVICE9 pD3DDevice, D3DPRESENT_PARAMETERS* pParams)
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	ImGui::GetIO().DisplaySize = ImVec2((float)pParams->BackBufferWidth, (float)pParams->BackBufferHeight);
}

void resetImGui(LPDIRECT3DDEVICE9 pD3DDevice, D3DPRESENT_PARAMETERS* pParams)
{
	ImGui_ImplDX9_CreateDeviceObjects();
	ImGui::GetIO().DisplaySize = ImVec2((float)pParams->BackBufferWidth, (float)(pParams->BackBufferHeight));
}

bool inGameGuiEnabled = false;
WPARAM inGameGuiHotkey;
std::vector<void(*)()> callbacks;
void drawImGui(LPDIRECT3DDEVICE9 pD3DDevice)
{
	if (!inGameGuiEnabled)
		return;

	ImGui_ImplDX9_NewFrame();
	ImGui::Text("Hello, world!");
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	for (size_t i = 0; i < callbacks.size(); i++)
	{
		ImGui::PushID(i);
		callbacks[i]();
		ImGui::PopID();
	}
	ImGui::Render();
}

void Hooks::InGameGUI()
{
	inGameGuiHotkey = config.getUInt("hotkeys", "keyTweakBar", VK_F12) & 0xFF;
	D3D9Add(createImGui, lostImGui, resetImGui, drawImGui);
}

void Hooks::InGameGUIAdd(void(*callback)()) { callbacks.push_back(callback); }
LRESULT Hooks::InGameGUIEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_KEYDOWN && (HIWORD(lParam) & KF_REPEAT) == 0 && wParam == inGameGuiHotkey)
		inGameGuiEnabled = !inGameGuiEnabled;
	return inGameGuiEnabled ? ImGui_ImplDX9_WndProcHandler(hwnd, msg, wParam, lParam) : 0;
}