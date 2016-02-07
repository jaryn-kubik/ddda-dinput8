#include "dinput8.h"
#include "TweakBar.h"
#include "d3d9.h"
#include <vector>

std::vector<std::function<void(TwBar*)>> functions;
void createTweakBar(LPDIRECT3DDEVICE9 pD3DDevice, D3DPRESENT_PARAMETERS* pParams)
{
	TwInit(TW_DIRECT3D9, pD3DDevice);
	TwBar *bar = TwNewBar("DDDAFix");
	TwDefine("DDDAFix refresh=0.5 size='400 400' valueswidth=225");
	for (auto &func : functions)
		func(bar);
}

bool enabled = false;
void lostTweakBar(LPDIRECT3DDEVICE9 pD3DDevice, D3DPRESENT_PARAMETERS* pParams) { TwWindowSize(0, 0); }
void resetTweakBar(LPDIRECT3DDEVICE9 pD3DDevice, D3DPRESENT_PARAMETERS* pParams) { TwWindowSize(pParams->BackBufferWidth, pParams->BackBufferHeight); }
void drawTweakBar(LPDIRECT3DDEVICE9 pD3DDevice) { if (enabled) TwDraw(); }

LRESULT Hooks::TweakBarEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) { return enabled ? TwEventWin(hwnd, msg, wParam, lParam) : 0; }
void Hooks::TweakBarSwitch() { enabled = !enabled; }
void Hooks::TweakBar()
{
	if (config.getBool(L"d3d9", L"tweakBar", false))
		D3D9Add(createTweakBar, lostTweakBar, resetTweakBar, drawTweakBar);
	else
		logFile << "TweakBar: disabled" << std::endl;
}

void Hooks::TweakBarAdd(std::function<void(TwBar*)> func) { functions.push_back(func); }
void Hooks::TweakBarDefine(const char* def) { TweakBarAdd([def](TwBar *b) { TwDefine(def); }); }
void Hooks::TweakBarAddRW(const char* name, TwType type, void* var, const char* def)
{
	TweakBarAdd([name, type, var, def](TwBar *b) { TwAddVarRW(b, name, type, var, def); });
}

void Hooks::TweakBarAddRO(const char* name, TwType type, const void* var, const char* def)
{
	TweakBarAdd([name, type, var, def](TwBar *b) { TwAddVarRO(b, name, type, var, def); });
}

void Hooks::TweakBarAddCB(const char* name, TwType type, TwSetVarCallback setCallback, TwGetVarCallback getCallback, void* clientData, const char* def)
{
	TweakBarAdd([name, type, setCallback, getCallback, clientData, def](TwBar *b)
	{
		TwAddVarCB(b, name, type, setCallback, getCallback, clientData, def);
	});
}