#include "stdafx.h"
#include "Hotkeys.h"
#include "InGameClock.h"

INPUT keyInput = { INPUT_KEYBOARD, {} };
DWORD menuPause;
std::unordered_map<WPARAM, std::function<void()>> keys;
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

BYTE **pSave = nullptr;
WNDPROC oWndProc;
LRESULT CALLBACK HWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (Hooks::TweakBarEvent(hwnd, msg, wParam, lParam))
		return 0;
	if (msg != WM_KEYDOWN || (HIWORD(lParam) & KF_REPEAT) != 0)
		return oWndProc(hwnd, msg, wParam, lParam);

	auto key = keys.find(wParam);
	if (key == keys.end())
		return oWndProc(hwnd, msg, wParam, lParam);
	key->second();
	return 0;
}

void Hooks::Hotkeys()
{
	if (config.getBool(L"hotkeys", L"enabled", false))
	{
		menuPause = config.getUInt(L"hotkeys", L"menuPause", 500);
		keys[config.getUInt(L"hotkeys", L"keyTweakBar", VK_F12)] = []() { TweakBarSwitch(); };
		keys[config.getUInt(L"hotkeys", L"keySave", VK_F5)] = []() { if (pSave && *pSave) (*pSave)[0x21AFD6] = 1; };
		keys[config.getUInt(L"hotkeys", L"keyCheckpoint", VK_F9)] = []() { if (pSave && *pSave) (*pSave)[0x21AFD5] = 1; };
		keys[config.getUInt(L"hotkeys", L"keyMap", 'M')] = []() { hotkeyMenu(hotkeyMap); };
		keys[config.getUInt(L"hotkeys", L"keyJournal", 'J')] = []() { hotkeyMenu(hotkeyJournal); };
		keys[config.getUInt(L"hotkeys", L"keyEquipment", 'U')] = []() { hotkeyMenu(hotkeyEquipment); };
		keys[config.getUInt(L"hotkeys", L"keyStatus", 'K')] = []() { hotkeyMenu(hotkeyStatus); };
		keys[config.getUInt(L"hotkeys", L"keyClock", VK_NUMPAD5)] = []() { InGameClockSwitch(); };
		keys[config.getUInt(L"hotkeys", L"keyClockMinDec", VK_NUMPAD4)] = []() { InGameClockDec(1); };
		keys[config.getUInt(L"hotkeys", L"keyClockMinInc", VK_NUMPAD6)] = []() { InGameClockInc(1); };
		keys[config.getUInt(L"hotkeys", L"keyClockHourDec", VK_NUMPAD2)] = []() { InGameClockDec(60); };
		keys[config.getUInt(L"hotkeys", L"keyClockHourInc", VK_NUMPAD8)] = []() { InGameClockInc(60); };

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