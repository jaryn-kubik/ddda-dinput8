#include "stdafx.h"
#include "Portcrystals.h"

LPVOID oMapClosest, oMapCursor;
float pMapClosest[3] = { 0 }, pMapCursor[3] = { 0 };
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

void renderPortcrystalsPos(const char *label, float *pPosition)
{
	float v[3] = { pPosition[0], pPosition[2], pPosition[1] };
	ImGui::InputFloat3(label, v, 0, ImGuiInputTextFlags_ReadOnly);
}

void renderPortcrystalsUI()
{
	if (ImGui::CollapsingHeader("Portcrystals"))
	{
		renderPortcrystalsPos("Cursor", pMapCursor);
		renderPortcrystalsPos("Closest", pMapClosest);
		ImGui::Separator();

		for (int i = 0; i < 10; i++)
		{
			DWORD address = 0xBE3A0 + 16 * i;
			renderPortcrystalsPos(std::to_string(i).c_str(), (float*)(*pBase + address / 4));
			ImGui::SameLine();
			if (ImGui::Button("Cursor"))
			{
				((float*)(*pBase + address / 4))[0] = pMapCursor[0];
				((float*)(*pBase + address / 4))[1] = pMapCursor[1];
				((float*)(*pBase + address / 4))[2] = pMapCursor[2];
			}
			ImGui::SameLine();
			if (ImGui::Button("Closest"))
			{
				((float*)(*pBase + address / 4))[0] = pMapClosest[0];
				((float*)(*pBase + address / 4))[1] = pMapClosest[1];
				((float*)(*pBase + address / 4))[2] = pMapClosest[2];
			}
		}
	}
}

void Hooks::Portcrystals()
{
	BYTE *pOffset;
	BYTE sig1[] = { 0xE8, 0xCC, 0xCC, 0xCC, 0xCC, 0x8B, 0x86, 0x94, 0x02, 0x00, 0x00, 0x83, 0xF8, 0x41, 0x7C };
	if (FindSignature("Portcrystals", sig1, &pOffset))
		CreateHook("Portcrystals", pOffset + 5, &HMapClosest, &oMapClosest);

	BYTE sig2[] = { 0xD9, 0x86, 0xD0, 0x08, 0x00, 0x00, 0x57 };
	if (FindSignature("Portcrystals", sig2, &pOffset))
		CreateHook("Portcrystals", pOffset, &HMapCursor, &oMapCursor);

	InGameUIAdd(renderPortcrystalsUI);
}