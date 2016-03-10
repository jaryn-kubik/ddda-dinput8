#pragma once

namespace Hooks
{
	void Hotkeys();
	void HotkeysHandler(WNDPROC proc);
	void HotkeysAdd(LPCSTR name, WORD defVal, void(*func)());
}