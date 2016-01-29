#include "dinput8.h"
#include "d3d9Hook.h"
#include "d3d9.h"
#include <d3dx9.h>
#include "utils.h"

DWORD **pClock;
RECT rect;
LPD3DXFONT pFont;
DWORD fontColor, fontSize;
void OnCreateDevice(LPDIRECT3DDEVICE9 pD3DDevice, D3DPRESENT_PARAMETERS* pParams)
{
	rect = { 0, 0, (LONG)pParams->BackBufferWidth, 0 };
	D3DXCreateFont(pD3DDevice, fontSize, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial", &pFont);
}

void OnLostDevice(LPDIRECT3DDEVICE9 pD3DDevice, D3DPRESENT_PARAMETERS* pParams)
{
	rect = { 0, 0, (LONG)pParams->BackBufferWidth, 0 };
	pFont->OnLostDevice();
}

void OnResetDevice(LPDIRECT3DDEVICE9 pD3DDevice, D3DPRESENT_PARAMETERS* pParams)
{
	rect = { 0, 0, (LONG)pParams->BackBufferWidth, 0 };
	pFont->OnResetDevice();
}

void OnEndScene(LPDIRECT3DDEVICE9 pD3DDevice)
{
	if (pClock && *pClock)
	{
		LPDWORD h = *pClock + 0xB876C / 4;
		LPDWORD m = h + 1;

		WCHAR buf[9];
		wsprintf(buf, L"%02d:%02d", *h, *m);
		pFont->DrawText(nullptr, buf, -1, &rect, DT_NOCLIP | DT_RIGHT, fontColor);
	}
}

typedef LPDIRECT3D9(WINAPI *tDirect3DCreate9)(UINT SDKVersion);
tDirect3DCreate9 oDirect3DCreate9;
LPDIRECT3D9 WINAPI HDirect3DCreate9(UINT SDKVersion)
{
	logFile << "DEBUG: Direct3DCreate9 called" << std::endl;
	return new fIDirect3D9(oDirect3DCreate9(SDKVersion), OnEndScene, OnCreateDevice, OnLostDevice, OnResetDevice);
}

void d3d9Hook::Init()
{
	if (!config.GetBool("d3d9", "enabled", false))
	{
		logFile << "D3D9: skipped" << std::endl;
		return;
	}

	fontSize = config.GetUInt("d3d9", "fontSize", 25);
	fontColor = config.GetUInt("d3d9", "fontColor", 0xDF00FF00);

	HMODULE hMod = LoadLibrary(L"d3d9.dll");
	oDirect3DCreate9 = (tDirect3DCreate9)GetProcAddress(hMod, "Direct3DCreate9");

	logStatus("D3D9 hook", MH_CreateHookEx(Direct3DCreate9, &HDirect3DCreate9, &oDirect3DCreate9));
	logStatus("D3D9 enable", MH_EnableHook(Direct3DCreate9));

	BYTE sig[] = { 0x8B, 0x15, 0xCC, 0xCC, 0xCC, 0xCC, 0x33, 0xDB, 0x8B, 0xF8 };
	BYTE *pOffset;
	if (utils::FindSignature(sig, &pOffset, "InGameClock signature"))
		pClock = (DWORD**)*(LPDWORD)(pOffset + 2);
	else
		pClock = nullptr;
}

void d3d9Hook::Uninit()
{
	if (config.GetBool("d3d9", "enabled", false))
		logStatus("D3D9 disable", MH_DisableHook(Direct3DCreate9));
}