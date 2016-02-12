#include "stdafx.h"
#include "Portcrystals.h"

LPVOID oMapClosest, oMapCursor;
DWORD pMapClosest[3] = { 0 }, pMapCursor[3] = { 0 };
void __declspec(naked) HMapClosest()
{
	__asm	mov		edx, [eax];
	__asm	mov		dword ptr[pMapClosest], edx;
	__asm	mov		edx, [eax + 4];
	__asm	mov		dword ptr[pMapClosest + 4], edx;
	__asm	mov		edx, [eax + 8];
	__asm	mov		dword ptr[pMapClosest + 8], edx;
	__asm	jmp		oMapClosest;
}

void __declspec(naked) HMapCursor()
{
	__asm	mov		edx, [esi + 0x8D0];
	__asm	mov		dword ptr[pMapCursor], edx;
	__asm	mov		edx, [esi + 0x8D4];
	__asm	mov		dword ptr[pMapCursor + 4], edx;
	__asm	mov		edx, [esi + 0x8D8];
	__asm	mov		dword ptr[pMapCursor + 8], edx;
	__asm	jmp		oMapCursor;
}

void setPorts(const void *value, void *clientData)
{
	if (pBase && *pBase)
		memcpy(*pBase + (DWORD)clientData / 4, value, 4 * 3);
}

void getPorts(void *value, void *clientData)
{
	if (pBase && *pBase)
		memcpy(value, *pBase + (DWORD)clientData / 4, 4 * 3);
}

void setPort(void *clientData, DWORD *target)
{
	if (pBase && *pBase)
	{
		(*pBase)[((DWORD)clientData + 0) / 4] = target[0];
		(*pBase)[((DWORD)clientData + 4) / 4] = target[1];
		(*pBase)[((DWORD)clientData + 8) / 4] = target[2];
	}
}
void setPortCursor(void *clientData) { setPort(clientData, pMapCursor); }
void setPortClosest(void *clientData) { setPort(clientData, pMapClosest); }

void addPortcrystals(TwBar *bar)
{
	BYTE *pOffset;
	BYTE sig1[] = { 0xE8, 0xCC, 0xCC, 0xCC, 0xCC, 0x8B, 0x86, 0x94, 0x02, 0x00, 0x00, 0x83, 0xF8, 0x41, 0x7C };
	if (Hooks::FindSignature("Portcrystals", sig1, &pOffset))
		Hooks::CreateHook("Portcrystals", pOffset + 5, &HMapClosest, &oMapClosest);

	BYTE sig2[] = { 0xD9, 0x86, 0xD0, 0x08, 0x00, 0x00, 0x57 };
	if (Hooks::FindSignature("Portcrystals", sig2, &pOffset))
		Hooks::CreateHook("Portcrystals", pOffset, &HMapCursor, &oMapCursor);

	TwStructMember positionStruct[] =
	{
		{ "X", TW_TYPE_FLOAT, 0, "" },
		{ "Y", TW_TYPE_FLOAT, 8, "" },
		{ "Z", TW_TYPE_FLOAT, 4, "" }
	};
	TwType positionType = TwDefineStruct("positionStruct", positionStruct, 3, 4 * 3, nullptr, nullptr);
	TwAddVarRO(bar, "portcrystalCursor", positionType, &pMapCursor, "group=Portcrystals label=Cursor");
	TwAddVarRO(bar, "portcrystalClosest", positionType, &pMapClosest, "group=Portcrystals label=Closest");
	TwAddSeparator(bar, "portcrystalSeparator", "group=Portcrystals");

	for (int i = 0; i < 10; i++)
	{
		LPVOID address = (LPVOID)(0xBE3A0 + 16 * i);

		string name = "portcrystal" + std::to_string(i);
		string def = "group=Portcrystals label=" + std::to_string(i + 1);
		TwAddVarCB(bar, name.c_str(), positionType, setPorts, getPorts, address, def.c_str());

		def = "group=" + name;
		TwAddButton(bar, ("Set Cursor " + std::to_string(i + 1)).c_str(), setPortCursor, address, def.c_str());
		TwAddButton(bar, ("Set Closest " + std::to_string(i + 1)).c_str(), setPortClosest, address, def.c_str());
	}
	TwDefine("DDDAFix/Portcrystals opened=false");
}

void Hooks::Portcrystals() { TweakBarAdd(addPortcrystals); }