#include "dinput8.h"
#include "InGameClock.h"
#include "d3d9.h"

DWORD **pClock;
RECT rect;
LPD3DXFONT pFont;
DWORD clockTimebase, clockColor, clockSize;
void onCreateDevice(LPDIRECT3DDEVICE9 pD3DDevice, D3DPRESENT_PARAMETERS* pParams)
{
	rect = { 0, 0, (LONG)pParams->BackBufferWidth, 0 };
	D3DXCreateFont(pD3DDevice, clockSize, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial", &pFont);
}

void onLostDevice(LPDIRECT3DDEVICE9 pD3DDevice, D3DPRESENT_PARAMETERS* pParams)
{
	rect = { 0, 0, (LONG)pParams->BackBufferWidth, 0 };
	pFont->OnLostDevice();
}

void onResetDevice(LPDIRECT3DDEVICE9 pD3DDevice, D3DPRESENT_PARAMETERS* pParams)
{
	rect = { 0, 0, (LONG)pParams->BackBufferWidth, 0 };
	pFont->OnResetDevice();
}

void onEndScene(LPDIRECT3DDEVICE9 pD3DDevice)
{
	if (pClock && *pClock)
	{
		DWORD t = (*pClock)[0xB876C / 4] * 60 + (*pClock)[0xB8770 / 4];
		t -= t % clockTimebase;

		WCHAR buf[9];
		wsprintf(buf, L"%02d:%02d", t / 60, t % 60);
		pFont->DrawText(nullptr, buf, -1, &rect, DT_NOCLIP | DT_RIGHT, clockColor);
	}
}

void Hooks::InGameClock()
{
	if (config.GetBool("d3d9", "inGameClock", false))
	{
		clockTimebase = config.GetUInt("d3d9", "inGameClockTimebase", 1);
		clockSize = config.GetUInt("d3d9", "inGameClockSize", 30);
		clockColor = config.GetUInt("d3d9", "inGameClockColor", 0xDF00FF00);
		if (clockTimebase < 1 || clockTimebase > 60 * 24)
			clockTimebase = 1;

		BYTE sig[] = { 0x8B, 0x15, 0xCC, 0xCC, 0xCC, 0xCC, 0x33, 0xDB, 0x8B, 0xF8 };
		BYTE *pOffset;
		if (FindSignature("InGameClock", sig, &pOffset))
			pClock = (DWORD**)*(LPDWORD)(pOffset + 2);
		else
			pClock = nullptr;

		D3D9Add(onCreateDevice, onLostDevice, onResetDevice, onEndScene);
	}
	else
		logFile << "InGameClock: disabled" << std::endl;
}