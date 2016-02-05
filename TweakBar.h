#pragma once
#include "include/AntTweakBar.h"
#include <functional>

namespace Hooks
{
	void TweakBar();
	void TweakBarSwitch();
	LRESULT TweakBarEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void TweakBarAdd(std::function<void(TwBar*)> func);
};
