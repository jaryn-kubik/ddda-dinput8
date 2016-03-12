#include "stdafx.h"
#include "DamageLog.h"
#include "ImGui/imgui_internal.h"

struct damageLogInfo
{
	LPVOID targetBase;
	BYTE targetId;
	float damage;
	damageLogInfo(LPVOID base, BYTE id, float dmg) : targetBase(base), targetId(id), damage(dmg) {}
};

CRITICAL_SECTION damageLogSync;
std::vector<damageLogInfo> damageLogBuffer;
void __stdcall HDamageLogStore(BYTE *targetBase, float damage)
{
	EnterCriticalSection(&damageLogSync);
	damageLogBuffer.emplace_back(targetBase, targetBase[0x2D], damage);
	LeaveCriticalSection(&damageLogSync);
}

LPBYTE pDamageLog1, oDamageLog1;
void __declspec(naked) HDamageLog1()
{
	__asm	mov		eax, [esp];
	__asm	pushad;
	__asm	push	eax;
	__asm	push	ebx;
	__asm	call	HDamageLogStore;
	__asm	popad;
	__asm	jmp		oDamageLog1;
}

LPBYTE pDamageLog2, oDamageLog2;
void __declspec(naked) HDamageLog2()
{
	__asm	mov		eax, [esp];
	__asm	pushad;
	__asm	push	eax;
	__asm	push	esi;
	__asm	call	HDamageLogStore;
	__asm	popad;
	__asm	jmp		oDamageLog2;
}

LPBYTE pDamageLog3, oDamageLog3;
void __declspec(naked) HDamageLog3()
{
	__asm	mov		eax, [esp];
	__asm	pushad;
	__asm	push	eax;
	__asm	push	esi;
	__asm	call	HDamageLogStore;
	__asm	popad;
	__asm	jmp		oDamageLog3;
}

bool damageLog = false;
UINT32 damageLogTargetType;
ImFont *damageLogFont = nullptr;
ImVec2 damageLogPosition, damageLogSize;
ImVec4 damageLogForeground, damageLogBackground;
ImGuiWindowFlags damageLogFlags = ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar;
void renderDamageLog(bool getsInput)
{
	if (!damageLog)
		return;

	ImGui::PushStyleColor(ImGuiCol_WindowBg, damageLogBackground);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.0f, 5.0f));
	ImGui::SetNextWindowPos(damageLogPosition, ImGuiSetCond_Once);
	ImGui::SetNextWindowSize(damageLogSize, ImGuiSetCond_Once);
	if (ImGui::Begin("DamageLog", nullptr, getsInput ? damageLogFlags : damageLogFlags | ImGuiWindowFlags_NoResize))
	{
		ImGui::PushStyleColor(ImGuiCol_Text, damageLogForeground);
		ImGuiListClipper clipper(damageLogBuffer.size(), ImGui::GetTextLineHeightWithSpacing());

		LPVOID lastId = nullptr;
		for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
		{
			if (lastId != damageLogBuffer[i].targetBase)
			{
				if (lastId)
					ImGui::Separator();
				lastId = damageLogBuffer[i].targetBase;
			}

			if (damageLogTargetType == 2)
				ImGui::Text("%08X -> %.2f", damageLogBuffer[i].targetBase, damageLogBuffer[i].damage);
			else if (damageLogTargetType == 1)
				ImGui::Text("%02hhX -> %.2f", damageLogBuffer[i].targetId, damageLogBuffer[i].damage);
			else
				ImGui::Text("%.2f", damageLogBuffer[i].damage);
		}

		clipper.End();
		ImGui::SetScrollHere();
		ImGui::PopStyleColor();
	}
	ImGui::End();
	ImGui::PopStyleVar();
	ImGui::PopStyleColor();
}

void DamageLogSwitch()
{
	config.setBool("inGameUI", "damageLog", damageLog = !damageLog);
	Hooks::SwitchHook("DamageLog", pDamageLog1, damageLog);
	Hooks::SwitchHook("DamageLog", pDamageLog2, damageLog);
	Hooks::SwitchHook("DamageLog", pDamageLog3, damageLog);
}

void renderDamageLogUI()
{
	if (ImGui::CollapsingHeader("Damage Log"))
	{
		if (ImGui::Checkbox("Enabled", &damageLog))
			DamageLogSwitch();

		std::pair<UINT32, const char*> targetType[]{ { 0, "Dmg only" },{ 1, "Group id" },{ 2, "Unique id" } };
		if (ImGui::RadioButtons<UINT32>(&damageLogTargetType, targetType))
			config.setUInt("inGameUI", "damageLogTargetType", damageLogTargetType);

		if (ImGui::ColorEdit4("Foreground", (float*)&damageLogForeground))
			config.setUInt("inGameUI", "damageLogForeground", ImGui::ColorConvertFloat4ToU32(damageLogForeground), true);

		if (ImGui::ColorEdit4("Background", (float*)&damageLogBackground))
			config.setUInt("inGameUI", "damageLogBackground", ImGui::ColorConvertFloat4ToU32(damageLogBackground), true);

		if (ImGui::Button("Save Position/Size"))
		{
			ImGuiWindow *window = nullptr;
			ImGuiState *state = (ImGuiState*)ImGui::GetInternalState();
			for (ImGuiWindow *w : state->Windows)
				if (strcmp(w->Name, "DamageLog") == 0)
					window = w;
			if (window)
			{
				config.setInts("inGameUI", "damageLogPosition", { (int)window->Pos.x, (int)window->Pos.y });
				config.setInts("inGameUI", "damageLogSize", { (int)window->Size.x, (int)window->Size.y });
			}
		}
	}
}

void Hooks::DamageLog()
{
	InitializeCriticalSection(&damageLogSync);
	HotkeysAdd("keyDamageLog", 'P', DamageLogSwitch);

	damageLog = config.getBool("inGameUI", "damageLog", false);
	damageLogTargetType = min(config.getUInt("inGameUI", "damageLogTargetType", 2), 2);
	ImU32 foreground = config.getUInt("inGameUI", "damageLogForeground", ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)));
	ImU32 background = config.getUInt("inGameUI", "damageLogBackground", ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 1.0f, 0.1f)));
	auto position = config.getInts("inGameUI", "damageLogPosition");
	auto size = config.getInts("inGameUI", "damageLogSize");
	damageLogForeground = ImGui::ColorConvertU32ToFloat4(foreground);
	damageLogBackground = ImGui::ColorConvertU32ToFloat4(background);
	damageLogPosition = position.size() == 2 ? ImVec2((float)position[0], (float)position[1]) : ImVec2(500, 0);
	damageLogSize = size.size() == 2 ? ImVec2((float)size[0], (float)size[1]) : ImVec2(200, 400);

	BYTE sig1[] = { 0x51, 0xF3, 0x0F, 0x11, 0x0C, 0x24, 0xE8, 0xCC, 0xCC, 0xCC, 0xCC, 0x8B, 0x13, 0x8B, 0x82, 0xD4, 0x01, 0x00, 0x00 };
	BYTE sig2[] = { 0x51, 0xF3, 0x0F, 0x11, 0x0C, 0x24, 0xE8, 0xCC, 0xCC, 0xCC, 0xCC, 0x8B, 0x16, 0x8B, 0x82, 0xD4, 0x01, 0x00, 0x00 };
	BYTE sig3[] = { 0x51, 0xF3, 0x0F, 0x11, 0x0C, 0x24, 0xE8, 0xCC, 0xCC, 0xCC, 0xCC, 0x8B, 0x06, 0x8B, 0x90, 0xD4, 0x01, 0x00, 0x00 };
	if (FindSignature("DamageLog", sig1, &pDamageLog1) &&
		FindSignature("DamageLog", sig2, &pDamageLog2) &&
		FindSignature("DamageLog", sig3, &pDamageLog3))
	{
		CreateHook("DamageLog", pDamageLog1 += 6, HDamageLog1, &oDamageLog1, damageLog);
		CreateHook("DamageLog", pDamageLog2 += 6, HDamageLog2, &oDamageLog2, damageLog);
		CreateHook("DamageLog", pDamageLog3 += 6, HDamageLog3, &oDamageLog3, damageLog);
	}

	InGameUIAdd(renderDamageLogUI);
	InGameUIAddWindow(renderDamageLog);
}