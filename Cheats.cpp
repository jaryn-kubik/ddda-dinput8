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
}

void Cheats::Uninit()
{
	if (pDrainStam)
		logStatus("Cheat (stamina) disable", MH_DisableHook(pDrainStam));
}
