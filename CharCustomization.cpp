/*
	Idea taken from CheatEngine table by user Cielos
	http://forum.cheatengine.org/viewtopic.php?p=5641841#5641841
*/

#include "CharCustomization.h"
#include "dinput8.h"

BYTE *pCharCustomization;
LPVOID oCharCustomization;
int __stdcall checkKeys()
{
	return GetAsyncKeyState(config.GetUInt("", "charCustomizationKey", VK_HOME)) & 0x8000;
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

	if (utils::FindSignature(sig, &pCharCustomization, "CharCustomization signature"))
	{
		logStatus("CharCustomization hook", MH_CreateHook(pCharCustomization, &HCharCustomization, &oCharCustomization));
		logStatus("CharCustomization enable", MH_EnableHook(pCharCustomization));
	}
	else
		pCharCustomization = nullptr;
}

void CharCustomization::Uninit()
{
	if (pCharCustomization)
		logStatus("CharCustomization disable", MH_DisableHook(pCharCustomization));
}