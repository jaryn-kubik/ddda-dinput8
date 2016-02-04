#include "dinput8.h"
#include "TweakBar.h"
#include "d3d9.h"
#include "include\AntTweakBar.h"
#include <vector>

std::vector<Hooks::TweakBarEntry> vars;
void createTweakBar(LPDIRECT3DDEVICE9 pD3DDevice, D3DPRESENT_PARAMETERS* pParams)
{
	TwInit(TW_DIRECT3D9, pD3DDevice);
	TwBar *bar = TwNewBar("DDDAFix");

	for (int i = 0; i < vars.size(); i++)
	{
		if (vars[i].varType == TW_TYPE_UNDEF)
			vars[i].varType = TwDefineEnum(std::to_string(i).c_str(), nullptr, 0);

		if (vars[i].type == Hooks::TweakBarRW)
			TwAddVarRW(bar, vars[i].name.c_str(), vars[i].varType, vars[i].var, vars[i].params.c_str());
		else if (vars[i].type == Hooks::TweakBarRO)
			TwAddVarRO(bar, vars[i].name.c_str(), vars[i].varType, vars[i].var, vars[i].params.c_str());
		else if (vars[i].type == Hooks::TweakBarCB)
			TwAddVarCB(bar, vars[i].name.c_str(), vars[i].varType, vars[i].cbSet, vars[i].cbGet, vars[i].var, vars[i].params.c_str());
	}
}

bool enabled = false;
void lostTweakBar(LPDIRECT3DDEVICE9 pD3DDevice, D3DPRESENT_PARAMETERS* pParams) { TwWindowSize(0, 0); }
void resetTweakBar(LPDIRECT3DDEVICE9 pD3DDevice, D3DPRESENT_PARAMETERS* pParams) { TwWindowSize(pParams->BackBufferWidth, pParams->BackBufferHeight); }
void drawTweakBar(LPDIRECT3DDEVICE9 pD3DDevice) { if (enabled) TwDraw(); }

void Hooks::TweakBarAdd(TweakBarEntry entry) { vars.push_back(entry); }
void Hooks::TweakBarSwitch() { enabled = !enabled; }
void Hooks::TweakBar()
{
	if (config.getBool(L"d3d9", L"tweakBar", false))
		D3D9Add(createTweakBar, lostTweakBar, resetTweakBar, drawTweakBar);
	else
		logFile << "TweakBar: disabled" << std::endl;
}

LRESULT Hooks::TweakBarEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return enabled ? TwEventWin(hwnd, msg, wParam, lParam) : 0;
}