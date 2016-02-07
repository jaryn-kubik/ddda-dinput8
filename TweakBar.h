#pragma once
#include "include/AntTweakBar.h"
#include <functional>

namespace Hooks
{
	void TweakBar();
	void TweakBarSwitch();
	LRESULT TweakBarEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void TweakBarAdd(std::function<void(TwBar*)> func);
	void TweakBarAddRW(const char* name, TwType type, void *var, const char *def);
	void TweakBarAddRO(const char* name, TwType type, const void *var, const char* def);
	void TweakBarAddCB(const char* name, TwType type, TwSetVarCallback setCallback, TwGetVarCallback getCallback, void* clientData, const char* def);
	void TweakBarDefine(const char *def);
};