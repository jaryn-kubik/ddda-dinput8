/*
	Idea taken from CheatEngine table by user Cielos
	http://forum.cheatengine.org/viewtopic.php?p=5641841&sid=74351ba2a0e31e335a5cb0b5cbf3105f#5641841
*/

#include "CharCustomization.h"
#include "dinput8.h"
#include "utils.h"

BYTE *pCharCustomization;
LPVOID oCharCustomization;
bool __stdcall checkKeys()
{
	for (auto key : config["charCustomizationKeys"])
		if (!(GetAsyncKeyState(key.get<int>()) & 0x8000))
			return false;
	return true;
}

void __declspec(naked) HCharCustomization()
{
	__asm
	{
		pushad;
		call	checkKeys;
		cmp		eax, 0;
		popad;

		je		getBack;
		mov		dword ptr[ebx + 284h], 2;

	getBack:
		jmp		oCharCustomization;
	}
}

void CharCustomization::Init()
{
	BYTE sig[] = { 0x83, 0xBB, 0x84, 0x02, 0x00, 0x00, 0x0B };

	if (utils::Find(sig, &pCharCustomization))
	{
		logFile << "CharCustomization signature: " << pCharCustomization << std::endl;
		logStatus("CharCustomization hook", MH_CreateHook(pCharCustomization, &HCharCustomization, &oCharCustomization));
		logStatus("CharCustomization enable", MH_EnableHook(pCharCustomization));
	}
	else
	{
		logFile << "CharCustomization signature: not found!" << std::endl;
		pCharCustomization = nullptr;
	}
}

void CharCustomization::Uninit()
{
	if (pCharCustomization)
		logStatus("CharCustomization disable", MH_DisableHook(pCharCustomization));
}