#include "Cheats.h"
#include "dinput8.h"

//taken from http://forum.cheatengine.org/viewtopic.php?p=5641841#5641841
LPVOID oRunAnimation;
void __declspec(naked) HRunAnimation()
{
	__asm
	{
		mov		eax, 0x20;
		jmp		oRunAnimation;
	}
}

float mWeight = 1.0;
LPVOID oWeight;
void __declspec(naked) HWeight()
{
	__asm
	{
		movss	xmm0, mWeight;
		mulss	xmm5, xmm0;
		jmp		oWeight;
	}
}

void Hooks::Cheats()
{
	if (config.getBool(L"cheats", L"useTownRun", false))
	{
		BYTE townWalkSig[] = { 0x8B, 0x42, 0x40,			//mov	eax, [edx+40h]
								0x53,						//push	ebx
								0x8B, 0x5C, 0x24, 0x08 };	//mov	ebx, [esp+4+arg_0]
		BYTE *pOffset;
		if (FindSignature("Cheat (town run)", townWalkSig, &pOffset))
			CreateHook("Cheat (town run)", pOffset + 3, &HRunAnimation, &oRunAnimation);
	}
	else
		logFile << "Cheat (town run): disabled" << std::endl;

	mWeight = config.getFloat(L"cheats", L"weightMultiplicator", -1);
	if (mWeight >= 0)
	{
		BYTE weightSig[] = { 0xF3, 0x0F, 0x58, 0xAB, 0x4C, 0x02, 0x00, 0x00,	//addss		xmm5, dword ptr [ebx+24Ch]
							0x45 };												//inc		ebp
		BYTE *pOffset;
		if (FindSignature("Cheat (weight)", weightSig, &pOffset))
			CreateHook("Cheat (weight)", pOffset, &HWeight, &oWeight);
	}
	else
		logFile << "Cheat (weight): disabled" << std::endl;
}