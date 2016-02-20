#include "stdafx.h"
#include "d3d9.h"
#include "ImGui/imgui_impl_dx9.h"
#include "ImGui/imgui_internal.h"

void createImGui(LPDIRECT3DDEVICE9 pD3DDevice, D3DPRESENT_PARAMETERS* pParams)
{
	ImGui_ImplDX9_Init(pD3DDevice);
	ImGui::GetIO().IniFilename = nullptr;
	ImGui::GetIO().DisplaySize = ImVec2((float)pParams->BackBufferWidth, (float)pParams->BackBufferHeight);
	ImGui::GetStyle().WindowTitleAlign = ImGuiAlign_Center;
	ImGui::GetStyle().WindowFillAlphaDefault = 0.95f;
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

bool inGameUIEnabled = false;
WPARAM inGameUIHotkey;
std::vector<void(*)()> callbacks;
char titleBuffer[64];
void drawImGui(LPDIRECT3DDEVICE9 pD3DDevice)
{
	if (!inGameUIEnabled)
		return;

	ImGui_ImplDX9_NewFrame();
	sprintf_s(titleBuffer, "DDDAFix - %.1f FPS###DDDAFix", ImGui::GetIO().Framerate);
	ImGui::SetNextWindowPos(ImVec2(5, 5), ImGuiSetCond_Once);
	ImGui::Begin(titleBuffer, nullptr, ImVec2(450, 600));
	for (size_t i = 0; i < callbacks.size(); i++)
	{
		ImGui::PushID(i);
		callbacks[i]();
		ImGui::PopID();
	}
	ImGui::End();
	ImGui::Render();
}

LPBYTE pInGameUI, oInGameUI;
SHORT WINAPI HGetAsyncKeyState(int vKey) { return ImGui::IsAnyItemActive() ? 0 : GetAsyncKeyState(vKey); }
void __declspec(naked) HInGameUI()
{
	__asm	mov		ebp, HGetAsyncKeyState;
	__asm	jmp		oInGameUI;
}

void Hooks::InGameUI()
{
	inGameUIHotkey = config.getUInt("hotkeys", "keyUI", VK_F12) & 0xFF;
	D3D9Add(createImGui, lostImGui, resetImGui, drawImGui);

	BYTE sigRun[] = { 0x8B, 0x2D, 0xCC, 0xCC, 0xCC, 0xCC,	//mov	ebp, ds:GetAsyncKeyState
					0x8D, 0x7E, 0x01 };						//lea	edi, [esi+1]
	if (FindSignature("InGameUI", sigRun, &pInGameUI))
	{
		CreateHook("InGameUI", pInGameUI, &HInGameUI, &oInGameUI, inGameUIEnabled);
		oInGameUI += 6;
	}
}

void Hooks::InGameUIAdd(void(*callback)()) { callbacks.push_back(callback); }
LRESULT Hooks::InGameUIEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_KEYDOWN && (HIWORD(lParam) & KF_REPEAT) == 0 && wParam == inGameUIHotkey)
		SwitchHook("InGameUI", pInGameUI, inGameUIEnabled = !inGameUIEnabled);
	return inGameUIEnabled ? ImGui_ImplDX9_WndProcHandler(hwnd, msg, wParam, lParam) : 0;
}

namespace ImGui
{
	bool InputFloatN(const char* label, float* v, int count, float item_width, float min, float max, int precision)
	{
		if (item_width > 0.0f)
			PushItemWidth(item_width * count);
		bool changed = InputFloatN(label, v, count, precision, 0);
		if (item_width > 0.0f)
			PopItemWidth();
		if (changed)
			for (int i = 0; i < count; i++)
			{
				if (v[i] < min)
					v[i] = min;
				if (v[i] > max)
					v[i] = max;
			}
		return changed;
	}

	bool InputFloatEx(const char* label, float* v, float step, float min, float max, int precision)
	{
		if (!InputFloat(label, v, step, 0.0f, precision, 0))
			return false;
		if (*v < min)
			*v = min;
		if (*v > max)
			*v = max;
		return true;
	}
}