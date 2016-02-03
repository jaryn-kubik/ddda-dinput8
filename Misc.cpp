// taken from CheatEngine table by Cielos
// http://forum.cheatengine.org/viewtopic.php?p=5641841#5641841

#include "Misc.h"
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

const float plus = 89.9f, minus = -89.9f;
LPVOID oExtendVerticalCam1, oExtendVerticalCam2, oExtendVerticalBow1, oExtendVerticalBow2;
void __declspec(naked) HExtendVerticalCam1()
{
	__asm	movss	xmm0, plus
	__asm	jmp		oExtendVerticalCam1;
}

void __declspec(naked) HExtendVerticalCam2()
{
	__asm	movss	xmm0, minus
	__asm	jmp		oExtendVerticalCam2;
}

void __declspec(naked) HExtendVerticalBow1()
{
	__asm	movss	xmm0, plus
	__asm	jmp		oExtendVerticalBow1;
}

void __declspec(naked) HExtendVerticalBow2()
{
	__asm	movss	xmm0, minus
	__asm	jmp		oExtendVerticalBow2;
}

void Hooks::Misc()
{
	BYTE *pOffset;
	if (config.getBool(L"main", L"charCustomization", false))
	{
		BYTE sig[] = { 0x83, 0xBB, 0x84, 0x02, 0x00, 0x00, 0x0B };	//cmp	dword ptr [ebx+284h], 0Bh
		if (FindSignature("CharCustomization", sig, &pOffset))
			CreateHook("CharCustomization", pOffset, &HCharCustomization, &oCharCustomization);
	}
	else
		logFile << "CharCustomization: disabled" << std::endl;

	if (config.getBool(L"main", L"disableAutoCam", false))
	{
		BYTE sigV[] = { 0x80, 0xBE, 0xF0, 0x02, 0x00, 0x00, 0x00,	//cmp	byte ptr [esi+2F0h], 0
						0x0F, 0x85, 0xCC, 0xCC, 0x00, 0x00 };		//jnz
		if (FindSignature("DisableAutoCamV", sigV, &pOffset))
			Set<BYTE>(pOffset + 7, { 0x90, 0xE9 });

		BYTE sigH[] = { 0x80, 0xBA, 0xF1, 0x02, 0x00, 0x00, 0x00,	//cmp	byte ptr [edx+2F1h], 0
						0x0F, 0x85, 0xCC, 0xCC, 0x00, 0x00 };		//jnz
		if (FindSignature("DisableAutoCamH", sigH, &pOffset))
			Set<BYTE>(pOffset + 7, { 0x90, 0xE9 });
	}
	else
		logFile << "DisableAutoCam: disabled" << std::endl;


	if (config.getBool(L"main", L"extendVerticalCam", false))
	{
		BYTE sig[] = { 0xF3, 0x0F, 0x10, 0x80, 0x8C, 0x00, 0x00, 0x00,	//movss	xmm0, dword ptr [eax+8Ch]
					0xF3, 0x0F, 0x10, 0x0D, 0xCC, 0xCC, 0xCC, 0xCC,		//movss	xmm1
					0xF3, 0x0F, 0x10, 0x11 };							//movss	xmm2, dword ptr [ecx]
		if (FindSignature("ExtendVerticalCam", sig, &pOffset))
		{
			CreateHook("ExtendVerticalCam1", pOffset += 8, &HExtendVerticalCam1, &oExtendVerticalCam1);
			CreateHook("ExtendVerticalCam2", pOffset + 0x21, &HExtendVerticalCam2, &oExtendVerticalCam2);
		}

		BYTE sigBow[] = { 0xF3, 0x0F, 0x10, 0x81, 0x8C, 0x00, 0x00, 0x00,	//movss	xmm0, dword ptr [ecx+8Ch]
						0xF3, 0x0F, 0x10, 0x0D, 0xCC, 0xCC, 0xCC, 0xCC,		//movss	xmm1
						0xF3, 0x0F, 0x10, 0x17 };							//movss	xmm2, dword ptr [edi]
		if (FindSignature("ExtendVerticalCamBow", sigBow, &pOffset))
		{
			CreateHook("ExtendVerticalCamBow1", pOffset += 8, &HExtendVerticalBow1, &oExtendVerticalBow1);
			CreateHook("ExtendVerticalCamBow2", pOffset + 0x27, &HExtendVerticalBow2, &oExtendVerticalBow2);
		}
	}
	else
		logFile << "ExtendVerticalCam: disabled" << std::endl;
}
