#include "Hotkeys.h"
#include "dinput8.h"

INPUT keyInput = { INPUT_KEYBOARD, {} };
DWORD menuPause;
void SendKeyPress(WORD vKey)
{
	keyInput.ki.wVk = vKey;
	keyInput.ki.dwFlags = 0;
	SendInput(1, &keyInput, sizeof(INPUT));
	keyInput.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &keyInput, sizeof(INPUT));
}

DWORD WINAPI hotkeyMap(LPVOID lpThreadParameter)
{
	Sleep(menuPause);
	SendKeyPress(VK_RETURN);
	return 0;
}

DWORD WINAPI hotkeyJournal(LPVOID lpThreadParameter)
{
	Sleep(menuPause);
	SendKeyPress(VK_LEFT);
	SendKeyPress(VK_RETURN);
	return 0;
}

DWORD WINAPI hotkeySave(LPVOID lpThreadParameter)
{
	Sleep(menuPause);
	SendKeyPress(0x49); // VK_I
	Sleep(menuPause / 5 * 4);
	SendKeyPress(VK_RETURN);
	return 0;
}

WNDPROC oWndProc;
LRESULT CALLBACK HWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_KEYDOWN && (lParam & (1 << 30)) == 0)
	{
		switch (wParam)
		{
		case 0x4D: // VK_M
			SendKeyPress(VK_ESCAPE);
			QueueUserWorkItem(hotkeyMap, nullptr, WT_EXECUTEDEFAULT);
			return 0;
		case 0x4A: // VK_J
			SendKeyPress(VK_ESCAPE);
			QueueUserWorkItem(hotkeyJournal, nullptr, WT_EXECUTEDEFAULT);
			return 0;
		case VK_F5:
			SendKeyPress(VK_ESCAPE);
			QueueUserWorkItem(hotkeySave, nullptr, WT_EXECUTEDEFAULT);
			return 0;
		}
	}
	return oWndProc(hwnd, msg, wParam, lParam);
}

void Hooks::Hotkeys()
{
	if (config.getBool(L"hotkeys", L"enabled", false))
	{
		menuPause = config.getUInt(L"hotkeys", L"menuPause", 500);

		BYTE sig[] = { 0x83, 0xEC, 0x50,			//sub	esp, 50h
						0x53,						//push	ebx
						0x8B, 0x5C, 0x24, 0x58,		//mov	ebx, [esp+54h+hWnd]
						0x56,						//push	esi
						0x8B, 0x74, 0x24, 0x60 };	//mov	esi, [esp+58h+Msg]
		BYTE *pOffset;
		if (FindSignature("Hotkeys", sig, &pOffset))
			CreateHook("Hotkeys", pOffset, &HWndProc, &oWndProc);
	}
	else
		logFile << "Hotkeys: disabled" << std::endl;
}