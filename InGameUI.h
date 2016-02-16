#pragma once

namespace Hooks
{
	void InGameUI();
	LRESULT InGameUIEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void InGameUIAdd(void(*callback)());
};
