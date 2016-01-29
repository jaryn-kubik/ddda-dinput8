/*
	Idea taken from CheatEngine table by user Cielos
	http://forum.cheatengine.org/viewtopic.php?p=5641841#5641841
*/

#include "CharCustomization.h"
#include "dinput8.h"

BYTE *pCharCustomization = nullptr;
LPVOID oCharCustomization = nullptr;
void __declspec(naked) HCharCustomization()
{
	__asm
	{
		cmp     dword ptr[ebx + 284h], 10;
		jne		getBack;
		mov		dword ptr[ebx + 284h], 2;

	getBack:
		jmp		oCharCustomization;
	}
}

void CharCustomization::Init()
{
	if (config.GetBool("", "charCustomization", true))
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
	else
		logFile << "CharCustomization: skipped" << std::endl;
}

void CharCustomization::Uninit()
{
	if (pCharCustomization)
		logStatus("CharCustomization disable", MH_DisableHook(pCharCustomization));
}