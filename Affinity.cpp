#include "dinput8.h"
#include "Affinity.h"
#include "TweakBar.h"

enum AffinityMod { Disable, NoNegative, AllPositive, InstantFriend = 850, InstantMax = 900 } iAffinityMod;
TwEnumVal affinityModEV[] =
{
	{ Disable, "Disable" }, { NoNegative, "No negative changes" },{ AllPositive, "All changes are positive" },
	{ InstantFriend, "Instant friend (850)" }, { InstantMax, "Instant max (900)" }
};

UINT16 *pAffinityLast;
LPVOID oAffinity;
void __declspec(naked) HAffinity()
{
	__asm
	{
		mov		dword ptr[pAffinityLast], esi;
		cmp		iAffinityMod, Disable;
		je		getBack;
		cmp		iAffinityMod, AllPositive;
		jle		notInstant;
		movzx	eax, word ptr[esi + 0x8B8];
		cmp		eax, [iAffinityMod];
		jae		notInstant;
		sub		eax, [iAffinityMod];
		neg		eax;
		mov		ebp, eax;
		jmp		getBack;

	notInstant:
		test	ebp, ebp;
		jns		getBack;
		cmp		dword ptr[iAffinityMod], NoNegative;
		jne		allPositive;
		xor		ebp, ebp;
		jmp		getBack;

	allPositive:
		neg		ebp;

	getBack:
		jmp		oAffinity;
	}
}

void setAffinity(const void *value, void *clientData)
{
	if (pAffinityLast)
		pAffinityLast[(DWORD)clientData / 2] = *(UINT16*)value;
}

void getAffinity(void *value, void *clientData)
{
	if (pAffinityLast)
		*(UINT16*)value = pAffinityLast[(DWORD)clientData / 2];
}

void addAffinity(TwBar *bar)
{
	BYTE *pOffset;
	BYTE sig[] = { 0x0F, 0xB7, 0x86, 0xB8, 0x08, 0x00, 0x00, 0x8B, 0xD8, 0x03, 0xC5 };
	if (!Hooks::FindSignature("Affinity", sig, &pOffset))
		return;
	Hooks::CreateHook("Affinity", pOffset, &HAffinity, &oAffinity);

	TwAddVarRW(bar, "affinityMod", TwDefineEnum("AffinityMod", affinityModEV, 5), &iAffinityMod, "group=Affinity label=Mode");
	TwAddVarCB(bar, "affinityLast", TW_TYPE_UINT16, setAffinity, getAffinity, (LPVOID)0x8B8, "group=Affinity label='Last changed' max=900");
	TwAddVarCB(bar, "affinityAtt", TW_TYPE_UINT16, setAffinity, getAffinity, (LPVOID)0x8BA, "group=Affinity label=Attitude hexa=true");
	TwDefine("DDDAFix/Affinity opened=false");
}

void Hooks::Affinity() { TweakBarAdd(addAffinity); }