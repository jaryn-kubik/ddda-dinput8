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

void Hooks::CharCustomization()
{
	if (config.GetBool("", "charCustomization", false))
	{
		BYTE sig[] = { 0x83, 0xBB, 0x84, 0x02, 0x00, 0x00, 0x0B };

		if (FindSignature("CharCustomization", sig, &pCharCustomization))
			CreateHook("CharCustomization", pCharCustomization, &HCharCustomization, &oCharCustomization);
	}
	else
		logFile << "CharCustomization: disabled" << std::endl;
}