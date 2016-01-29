#include "Cheats.h"
#include "dinput8.h"

BYTE *pDrainStam = nullptr;
LPVOID oDrainStam = nullptr;
void __declspec(naked) HDrainStam()
{
	__asm
	{
		xorps   xmm0, xmm0;
		jmp		oDrainStam;
	}
}

float mWeight = 1.0;
BYTE *pWeight = nullptr;
LPVOID oWeight = nullptr;
void __declspec(naked) HWeight()
{
	__asm
	{
		movss	xmm0, mWeight;
		mulss	xmm5, xmm0;
		jmp		oWeight;
	}
}

void Cheats::Init()
{
	if (config.GetBool("cheats", "removeStaminaDrain", false))
	{
		BYTE stamSig[] = { 0xF3, 0x0F, 0x10, 0x43, 0x04,	//movss		xmm0, dword ptr[ebx + 4]
							0x8B, 0x0D };					//mov		ecx, dword_xxx

		if (utils::FindSignature(stamSig, &pDrainStam, "Cheat (stamina) signature"))
		{
			pDrainStam += 5;
			logStatus("Cheat (stamina) hook", MH_CreateHook(pDrainStam, &HDrainStam, &oDrainStam));
			logStatus("Cheat (stamina) enable", MH_EnableHook(pDrainStam));
		}
		else
			pDrainStam = nullptr;
	}
	else
		logFile << "Cheat (stamina): skipped" << std::endl;

	mWeight = config.GetFloat("cheats", "weightMultiplicator", -1);
	if (mWeight >= 0)
	{
		BYTE weightSig[] = { 0xF3, 0x0F, 0x58, 0xAB, 0x4C, 0x02, 0x00, 0x00,	//addss		xmm5, dword ptr [ebx+24Ch]
							0x45 };												//inc		ebp

		if (utils::FindSignature(weightSig, &pWeight, "Cheat (weight) signature"))
		{
			logStatus("Cheat (weight) hook", MH_CreateHook(pWeight, &HWeight, &oWeight));
			logStatus("Cheat (weight) enable", MH_EnableHook(pWeight));
		}
		else
			pWeight = nullptr;
	}
	else
		logFile << "Cheat (weight): skipped" << std::endl;
}

void Cheats::Uninit()
{
	if (pDrainStam)
		logStatus("Cheat (stamina) disable", MH_DisableHook(pDrainStam));
	if (pWeight)
		logStatus("Cheat (weight) disable", MH_DisableHook(pWeight));
}
