#include "dinput8.h"
#include "d3d9Hook.h"
#include "d3d9.h"
#include <d3dx9.h>

void OnCreateDevice(LPDIRECT3D9 pD3D9) { }
void OnEndScene(LPDIRECT3DDEVICE9 pD3DDevice)
{
}

typedef LPDIRECT3D9(WINAPI *tDirect3DCreate9)(UINT SDKVersion);
tDirect3DCreate9 oDirect3DCreate9;
LPDIRECT3D9 WINAPI HDirect3DCreate9(UINT SDKVersion) { return new fIDirect3D9(oDirect3DCreate9(SDKVersion), OnCreateDevice, OnEndScene); }

void d3d9Hook::Init()
{
	HMODULE hMod = LoadLibrary(L"d3d9.dll");
	oDirect3DCreate9 = (tDirect3DCreate9)GetProcAddress(hMod, "Direct3DCreate9");

	logStatus("D3D9 hook", MH_CreateHookEx(Direct3DCreate9, &HDirect3DCreate9, &oDirect3DCreate9));
	logStatus("D3D9 enable", MH_EnableHook(Direct3DCreate9));
}

void d3d9Hook::Uninit()
{
	logStatus("D3D9 disable", MH_DisableHook(Direct3DCreate9));
}