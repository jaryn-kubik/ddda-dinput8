#include "stdafx.h"
#include "Hotkeys.h"

INPUT keyInput = { INPUT_KEYBOARD, {} };
DWORD menuPause;
void(*keys[0x100])() = { nullptr };
void SendKeyPress(DWORD vKey)
{
	keyInput.ki.wVk = (WORD)vKey;
	keyInput.ki.dwFlags = 0;
	SendInput(1, &keyInput, sizeof(INPUT));
	keyInput.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &keyInput, sizeof(INPUT));
}

DWORD WINAPI hotkeyProc(LPVOID vKey)
{
	Sleep(menuPause);
	if ((DWORD)vKey)
		SendKeyPress((DWORD)vKey);
	SendKeyPress(VK_RETURN);
	return 0;
}

void hotkeyStart(DWORD vKey)
{
	SendKeyPress(VK_ESCAPE);
	QueueUserWorkItem(hotkeyProc, (LPVOID)vKey, WT_EXECUTEDEFAULT);
}

bool borderlessFullscreen = false;
void setBorderlessFullscreen(HWND hwnd)
{
	LONG lStyle = GetWindowLong(hwnd, GWL_STYLE);
	lStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_SYSMENU | WS_MAXIMIZE | WS_MINIMIZE);
	SetWindowLong(hwnd, GWL_STYLE, lStyle);
	LONG lExStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
	lExStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_COMPOSITED | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_LAYERED | WS_EX_STATICEDGE | WS_EX_TOOLWINDOW | WS_EX_APPWINDOW);
	SetWindowLong(hwnd, GWL_EXSTYLE, lExStyle);

	HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO info;
	info.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(monitor, &info);
	int width = info.rcMonitor.right - info.rcMonitor.left;
	int height = info.rcMonitor.bottom - info.rcMonitor.top;
	SetWindowPos(hwnd, nullptr, info.rcMonitor.left, info.rcMonitor.top, width, height, SWP_NOSENDCHANGING | SWP_NOZORDER | SWP_NOOWNERZORDER);
}

BYTE **pSave = nullptr;
WNDPROC oWndProc, wndProcHandler = nullptr;
LRESULT CALLBACK HWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_SIZE && borderlessFullscreen)
		setBorderlessFullscreen(hwnd);

	if (wndProcHandler(hwnd, msg, wParam, lParam))
		return 0;
	if ((msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN) &&
		(HIWORD(lParam) & KF_REPEAT) == 0 &&
		wParam < 0xFF && keys[wParam] != nullptr)
	{
		keys[wParam]();
		return 0;
	}
	return oWndProc(hwnd, msg, wParam, lParam);
}

void Hooks::HotkeysAdd(LPCSTR name, WORD defKey, void(*func)())
{
	unsigned int key = config.getUInt("hotkeys", name, defKey);
	if (key < 0xFF)
		keys[key] = func;
}

void Hooks::HotkeysHandler(WNDPROC proc) { wndProcHandler = proc; }
void Hooks::Hotkeys()
{
	if (config.getBool("hotkeys", "enabled", false))
	{
		borderlessFullscreen = config.getBool("main", "borderlessFullscreen", false);
		menuPause = config.getUInt("hotkeys", "menuPause", 500);
		HotkeysAdd("keySave", VK_F5, []() { if (pSave && *pSave) (*pSave)[0x21AFD6] = 1; });
		HotkeysAdd("keyCheckpoint", VK_F9, []() { if (pSave && *pSave) (*pSave)[0x21AFD5] = 1; });
		HotkeysAdd("keyMap", 'M', []() { hotkeyStart(0); });
		HotkeysAdd("keyJournal", 'J', []() { hotkeyStart(VK_LEFT); });
		HotkeysAdd("keyEquipment", 'U', []() { hotkeyStart(VK_RIGHT); });
		HotkeysAdd("keyStatus", 'K', []() { hotkeyStart(VK_DOWN); });

		BYTE sig[] = { 0x83, 0xEC, 0x50,			//sub	esp, 50h
						0x53,						//push	ebx
						0x8B, 0x5C, 0x24, 0x58,		//mov	ebx, [esp+54h+hWnd]
						0x56,						//push	esi
						0x8B, 0x74, 0x24, 0x60 };	//mov	esi, [esp+58h+Msg]
		BYTE *pOffset;
		if (FindSignature("Hotkeys", sig, &pOffset))
			CreateHook("Hotkeys", pOffset, &HWndProc, &oWndProc);

		BYTE sigSave[] = { 0x8B, 0x15, 0xCC, 0xCC, 0xCC, 0xCC,	//mov	edx, savePointer
							0x0F, 0x95, 0xC0,					//setnz	al
							0x83, 0xC9, 0xFF };					//or	ecx, 0FFFFFFFFh

		if (FindSignature("HotkeysSave", sigSave, &pOffset))
			pSave = (BYTE**)*(LPDWORD)(pOffset + 2);
	}
	else
		logFile << "Hotkeys: disabled" << std::endl;
}