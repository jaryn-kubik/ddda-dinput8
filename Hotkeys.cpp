#include "Hotkeys.h"
#include "dinput8.h"
#include "InGameClock.h"

INPUT keyInput = { INPUT_KEYBOARD, {} };
DWORD menuPause;
UINT keySave, keyMap, keyJournal, keyEquipment, keyStatus, keyClock;
void SendKeyPress(WORD vKey)
{
	keyInput.ki.wVk = vKey;
	keyInput.ki.dwFlags = 0;
	SendInput(1, &keyInput, sizeof(INPUT));
	keyInput.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &keyInput, sizeof(INPUT));
}

DWORD WINAPI hotkeySave(LPVOID lpThreadParameter)
{
	Sleep(menuPause);
	SendKeyPress('I');
	Sleep(menuPause / 5 * 4);
	SendKeyPress(VK_RETURN);
	return 0;
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

DWORD WINAPI hotkeyEquipment(LPVOID lpThreadParameter)
{
	Sleep(menuPause);
	SendKeyPress(VK_RIGHT);
	SendKeyPress(VK_RETURN);
	return 0;
}

DWORD WINAPI hotkeyStatus(LPVOID lpThreadParameter)
{
	Sleep(menuPause);
	SendKeyPress(VK_DOWN);
	SendKeyPress(VK_RETURN);
	return 0;
}

WNDPROC oWndProc;
LRESULT CALLBACK HWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_KEYDOWN && (HIWORD(lParam) & KF_REPEAT) == 0)
	{
		LPTHREAD_START_ROUTINE func = nullptr;
		if (wParam == keySave)
			func = hotkeySave;
		else if (wParam == keyMap)
			func = hotkeyMap;
		else if (wParam == keyJournal)
			func = hotkeyJournal;	
		else if (wParam == keyEquipment)
			func = hotkeyEquipment;
		else if (wParam == keyStatus)
			func = hotkeyStatus;
		else if (wParam == keyClock)
		{
			Hooks::InGameClockSwitch();
			return 0;
		}

		if (func)
		{
			SendKeyPress(VK_ESCAPE);
			QueueUserWorkItem(func, nullptr, WT_EXECUTEDEFAULT);
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
		keySave = config.getUInt(L"hotkeys", L"keySave", VK_F5);
		keyMap = config.getUInt(L"hotkeys", L"keyMap", 'M');
		keyJournal = config.getUInt(L"hotkeys", L"keyJournal", 'J');
		keyEquipment = config.getUInt(L"hotkeys", L"keyEquipment", 'U');
		keyStatus = config.getUInt(L"hotkeys", L"keyStatus", 'K');
		keyClock = config.getUInt(L"hotkeys", L"keyClock", 'P');

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