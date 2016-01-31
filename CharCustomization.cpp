/*
	Idea taken from CheatEngine table by user Cielos
	http://forum.cheatengine.org/viewtopic.php?p=5641841#5641841
*/

#include "CharCustomization.h"
#include "dinput8.h"

LPVOID oCharCustomization;
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
	if (config.getBool(L"main", L"charCustomization", false))
	{
		BYTE sig[] = { 0x83, 0xBB, 0x84, 0x02, 0x00, 0x00, 0x0B }; //cmp	dword ptr [ebx+284h], 0Bh
		BYTE *pOffset;
		if (FindSignature("CharCustomization", sig, &pOffset))
			CreateHook("CharCustomization", pOffset, &HCharCustomization, &oCharCustomization);
	}
	else
		logFile << "CharCustomization: disabled" << std::endl;
}