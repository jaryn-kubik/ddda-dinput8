#include "dinput8.h"
#include "Affinity.h"
#include "TweakBar.h"

enum AffinityMod { Disabled = -1, NoNegative, AllPositive, NoChange, InstantFriend = 850, InstantMax = 900 } iAffinityMod;
TwEnumVal affinityModEV[] =
{
	{ Disabled, "Disabled" }, { NoNegative, "No negative changes" }, { AllPositive, "All changes are positive" },
	{ NoChange, "No changes at all" }, { InstantFriend, "Instant friend (850)" }, { InstantMax, "Instant max (900)" }
};

UINT16 *pAffinityLast;
LPVOID oAffinity;
void __declspec(naked) HAffinity()
{
	__asm
	{
		mov		dword ptr[pAffinityLast], esi;
		cmp		iAffinityMod, Disabled;
		jle		getBack;
		cmp		iAffinityMod, AllPositive;
		jle		notInstant;
		cmp		iAffinityMod, NoChange;
		je		noNegative;
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
		je		noNegative;
		neg		ebp;
		jmp		getBack;

	noNegative:
		xor		ebp, ebp;

	getBack:
		jmp		oAffinity;
	}
}

void setAffinity(const void *value, void *clientData)
{
	if (clientData == &iAffinityMod)
		config.setInt(L"cheats", L"affinityMod", iAffinityMod = *(AffinityMod*)value);
	else if (pAffinityLast)
		pAffinityLast[(DWORD)clientData / 2] = *(UINT16*)value;
}

void getAffinity(void *value, void *clientData)
{
	if (clientData == &iAffinityMod)
		*(AffinityMod*)value = iAffinityMod;
	else if (pAffinityLast)
		*(UINT16*)value = pAffinityLast[(DWORD)clientData / 2];
}

void addAffinity(TwBar *bar)
{
	iAffinityMod = (AffinityMod)config.getInt(L"cheats", L"affinityMod", Disabled);
	TwAddVarCB(bar, "affinityMod", TwDefineEnum("AffinityMod", affinityModEV, 6), setAffinity, getAffinity, &iAffinityMod, "group=Affinity label=Mode");
	TwAddVarCB(bar, "affinityLast", TW_TYPE_UINT16, setAffinity, getAffinity, (LPVOID)0x8B8, "group=Affinity label='Last changed' max=900");
	TwAddVarCB(bar, "affinityAtt", TW_TYPE_UINT16, setAffinity, getAffinity, (LPVOID)0x8BA, "group=Affinity label=Attitude hexa=true");
	TwDefine("DDDAFix/Affinity group=Misc opened=false");
}

void Hooks::Affinity()
{
	BYTE *pOffset;
	BYTE sig[] = { 0x0F, 0xB7, 0x86, 0xB8, 0x08, 0x00, 0x00, 0x8B, 0xD8, 0x03, 0xC5 };
	if (!FindSignature("Affinity", sig, &pOffset))
		return;
	CreateHook("Affinity", pOffset, &HAffinity, &oAffinity);
	TweakBarAdd(addAffinity);
}