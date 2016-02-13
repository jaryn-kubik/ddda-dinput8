// taken from CheatEngine table by Cielos
// http://forum.cheatengine.org/viewtopic.php?p=5641841#5641841

#include "stdafx.h"
#include "Misc.h"

LPBYTE pCharCustomization;
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
LPBYTE pExtendVerticalCam1, pExtendVerticalCam2, pExtendVerticalBow1, pExtendVerticalBow2;
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

LPBYTE pAutoCamV, pAutoCamH;
LPVOID oAutoCamV, oAutoCamH;
void __declspec(naked) HAutoCamV()
{
	__asm	cmp		esi, 0;
	__asm	jmp		oAutoCamV;
}

void __declspec(naked) HAutoCamH()
{
	__asm	cmp		edx, 0;
	__asm	jmp		oAutoCamH;
}

bool charCustomization, extendVerticalCam, disableAutoCam;
void getMisc(void *value, void *clientData) { *(bool*)value = *(bool*)clientData; }
void setMisc(const void *value, void *clientData)
{
	*(bool*)clientData = *(bool*)value;
	if (clientData == &charCustomization)
	{
		config.setBool("main", "charCustomization", charCustomization);
		Hooks::SwitchHook("CharCustomization", pCharCustomization, charCustomization);
	}
	else if (clientData == &extendVerticalCam)
	{
		config.setBool("main", "extendVerticalCam", extendVerticalCam);
		Hooks::SwitchHook("ExtendVerticalCam1", pExtendVerticalCam1, extendVerticalCam);
		Hooks::SwitchHook("ExtendVerticalCam2", pExtendVerticalCam2, extendVerticalCam);
		Hooks::SwitchHook("ExtendVerticalCamBow1", pExtendVerticalBow1, extendVerticalCam);
		Hooks::SwitchHook("ExtendVerticalCamBow2", pExtendVerticalBow2, extendVerticalCam);
	}
	else if (clientData == &disableAutoCam)
	{
		config.setBool("main", "disableAutoCam", disableAutoCam);
		Hooks::SwitchHook("DisableAutoCamV", pAutoCamV, disableAutoCam);
		Hooks::SwitchHook("DisableAutoCamH", pAutoCamH, disableAutoCam);
	}
}

LPVOID oWeather;
void __declspec(naked) HWeather()
{
	__asm
	{
		cmp		byte ptr[ecx + 0xB33A8], 0;
		je		flagZero;
		cmp		word ptr[ecx + 0x34], 0xDC;
		jmp		oWeather;

	flagZero:
		cmp		word ptr[ecx + 0x34], 0xE6;
		je		unsetZero;
		cmp		ecx, ecx;
		jmp		oWeather;

	unsetZero:
		cmp		ecx, 0;
		jmp		oWeather;
	}
}

void getWeather(void *value, void *clientData)
{
	if (pBase && *pBase)
		*(DWORD*)value = (*pBase)[(DWORD)clientData / 4];
}

void setWeather(const void *value, void *clientData)
{
	if (pBase && *pBase)
		(*pBase)[(DWORD)clientData / 4] = *(DWORD*)value;
}

void Hooks::Misc()
{
	BYTE sigChar[] = { 0x83, 0xBB, 0x84, 0x02, 0x00, 0x00, 0x0B };	//cmp	dword ptr [ebx+284h], 0Bh
	if (FindSignature("CharCustomization", sigChar, &pCharCustomization))
	{
		charCustomization = config.getBool("main", "charCustomization", false);
		CreateHook("CharCustomization", pCharCustomization, &HCharCustomization, &oCharCustomization, charCustomization);
		TweakBarAddCB("miscChar", TW_TYPE_BOOLCPP, setMisc, getMisc, &charCustomization, "group=Misc label='Char customization'");
	}

	BYTE sigCam[] = { 0xF3, 0x0F, 0x10, 0x80, 0x8C, 0x00, 0x00, 0x00,	//movss	xmm0, dword ptr [eax+8Ch]
					0xF3, 0x0F, 0x10, 0x0D, 0xCC, 0xCC, 0xCC, 0xCC,		//movss	xmm1
					0xF3, 0x0F, 0x10, 0x11 };							//movss	xmm2, dword ptr [ecx]

	BYTE sigBow[] = { 0xF3, 0x0F, 0x10, 0x81, 0x8C, 0x00, 0x00, 0x00,	//movss	xmm0, dword ptr [ecx+8Ch]
					0xF3, 0x0F, 0x10, 0x0D, 0xCC, 0xCC, 0xCC, 0xCC,		//movss	xmm1
					0xF3, 0x0F, 0x10, 0x17 };							//movss	xmm2, dword ptr [edi]

	if (FindSignature("ExtendVerticalCam", sigCam, &pExtendVerticalCam1) &&
		FindSignature("ExtendVerticalCamBow", sigBow, &pExtendVerticalBow1))
	{
		pExtendVerticalCam1 += 8;
		pExtendVerticalCam2 = pExtendVerticalCam1 + 0x21;
		pExtendVerticalBow1 += 8;
		pExtendVerticalBow2 = pExtendVerticalBow1 + 0x27;

		extendVerticalCam = config.getBool("main", "extendVerticalCam", false);
		CreateHook("ExtendVerticalCam1", pExtendVerticalCam1, &HExtendVerticalCam1, &oExtendVerticalCam1, extendVerticalCam);
		CreateHook("ExtendVerticalCam2", pExtendVerticalCam2, &HExtendVerticalCam2, &oExtendVerticalCam2, extendVerticalCam);
		CreateHook("ExtendVerticalCamBow1", pExtendVerticalBow1, &HExtendVerticalBow1, &oExtendVerticalBow1, extendVerticalCam);
		CreateHook("ExtendVerticalCamBow2", pExtendVerticalBow2, &HExtendVerticalBow2, &oExtendVerticalBow2, extendVerticalCam);
		TweakBarAddCB("miscExtendCam", TW_TYPE_BOOLCPP, setMisc, getMisc, &extendVerticalCam, "group=Misc label='Camera extend'");
	}

	BYTE sigV[] = { 0x80, 0xBE, 0xF0, 0x02, 0x00, 0x00, 0x00,	//cmp	byte ptr [esi+2F0h], 0
					0x0F, 0x85, 0xCC, 0xCC, 0x00, 0x00 };		//jnz
	BYTE sigH[] = { 0x80, 0xBA, 0xF1, 0x02, 0x00, 0x00, 0x00,	//cmp	byte ptr [edx+2F1h], 0
					0x0F, 0x85, 0xCC, 0xCC, 0x00, 0x00 };		//jnz
	if (FindSignature("DisableAutoCamV", sigV, &pAutoCamV) &&
		FindSignature("DisableAutoCamH", sigH, &pAutoCamH))
	{
		disableAutoCam = config.getBool("main", "disableAutoCam", false);
		CreateHook("DisableAutoCamV", pAutoCamV, &HAutoCamV, nullptr, disableAutoCam);
		CreateHook("DisableAutoCamH", pAutoCamH, &HAutoCamH, nullptr, disableAutoCam);
		oAutoCamV = pAutoCamV + 7;
		oAutoCamH = pAutoCamH + 7;
		TweakBarAddCB("miscAutoCam", TW_TYPE_BOOLCPP, setMisc, getMisc, &disableAutoCam, "group=Misc label='Disable autocam'");
	}

	BYTE *pOffset;
	BYTE sigWeather[] = { 0x80, 0xB9, 0xA8, 0x33, 0x0B, 0x00, 0x00, 0x74, 0x15 };
	if (FindSignature("Weather", sigWeather, &pOffset))
	{
		CreateHook("Weather", pOffset, &HWeather, nullptr);
		oWeather = pOffset + 7;
	}

	TweakBarAdd([](TwBar *b)
	{
		TwEnumVal weatherEV[] = { { 0, "Clear sky" },{ 1, "Cloudy" },{ 2, "Foggy" },{ 3, "Vulcanic (post-game)" } };
		TwType weatherEnum = TwDefineEnum("weatherEnum", weatherEV, 4);
		TwAddVarCB(b, "miscWeather", weatherEnum, setWeather, getWeather, (LPVOID)0xB8780, "group=Misc label=Weather");
		TwAddVarCB(b, "miscWeatherExtra", TW_TYPE_BOOL32, setWeather, getWeather, (LPVOID)0xB33A8, "group=Misc label='Post-game weather'");
	});

	TweakBarDefine("DDDAFix/Misc opened=false");
}