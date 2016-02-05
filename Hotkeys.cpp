#include "Hotkeys.h"
#include "dinput8.h"
#include "InGameClock.h"
#include "TweakBar.h"

INPUT keyInput = { INPUT_KEYBOARD, {} };
DWORD menuPause;
UINT keyInventory, keySave, keyMap, keyJournal, keyEquipment, keyStatus;
UINT keyClock, keyClockHourInc, keyClockHourDec, keyClockMinInc, keyClockMinDec;
UINT keyConfig;
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
	SendKeyPress(keyInventory);
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

void hotkeyMenu(LPTHREAD_START_ROUTINE func)
{
	SendKeyPress(VK_ESCAPE);
	QueueUserWorkItem(func, nullptr, WT_EXECUTEDEFAULT);
}

WNDPROC oWndProc;
LRESULT CALLBACK HWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (Hooks::TweakBarEvent(hwnd, msg, wParam, lParam))
		return 0;
	if (msg != WM_KEYDOWN || (HIWORD(lParam) & KF_REPEAT) != 0)
		return oWndProc(hwnd, msg, wParam, lParam);

	if (wParam == keySave)
		hotkeyMenu(hotkeySave);
	else if (wParam == keyMap)
		hotkeyMenu(hotkeyMap);
	else if (wParam == keyJournal)
		hotkeyMenu(hotkeyJournal);
	else if (wParam == keyEquipment)
		hotkeyMenu(hotkeyEquipment);
	else if (wParam == keyStatus)
		hotkeyMenu(hotkeyStatus);
	else if (wParam == keyClock)
		Hooks::InGameClockSwitch();
	else if (wParam == keyClockMinDec)
		Hooks::InGameClockDec(1);
	else if (wParam == keyClockMinInc)
		Hooks::InGameClockInc(1);
	else if (wParam == keyClockHourDec)
		Hooks::InGameClockDec(60);
	else if (wParam == keyClockHourInc)
		Hooks::InGameClockInc(60);
	else if (wParam == keyConfig)
		Hooks::TweakBarSwitch();
	else
		return oWndProc(hwnd, msg, wParam, lParam);
	return 0;
}

void Hooks::Hotkeys()
{
	if (config.getBool(L"hotkeys", L"enabled", false))
	{
		menuPause = config.getUInt(L"hotkeys", L"menuPause", 500);
		keyConfig = config.getUInt(L"hotkeys", L"keyTweakBar", VK_OEM_3);
		keyInventory = config.getUInt(L"hotkeys", L"keyInventory", 'I');
		keySave = config.getUInt(L"hotkeys", L"keySave", VK_F5);
		keyMap = config.getUInt(L"hotkeys", L"keyMap", 'M');
		keyJournal = config.getUInt(L"hotkeys", L"keyJournal", 'J');
		keyEquipment = config.getUInt(L"hotkeys", L"keyEquipment", 'U');
		keyStatus = config.getUInt(L"hotkeys", L"keyStatus", 'K');
		keyClock = config.getUInt(L"hotkeys", L"keyClock", VK_NUMPAD5);
		keyClockMinDec = config.getUInt(L"hotkeys", L"keyClockMinDec", VK_NUMPAD4);
		keyClockMinInc = config.getUInt(L"hotkeys", L"keyClockMinInc", VK_NUMPAD6);
		keyClockHourDec = config.getUInt(L"hotkeys", L"keyClockHourDec", VK_NUMPAD2);
		keyClockHourInc = config.getUInt(L"hotkeys", L"keyClockHourInc", VK_NUMPAD8);

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