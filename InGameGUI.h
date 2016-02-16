#pragma once

namespace Hooks
{
	void InGameGUI();
	LRESULT InGameGUIEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void InGameGUIAdd(void(*callback)());
};
