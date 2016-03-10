#include "stdafx.h"
#include "DamageLog.h"
#include "ImGui/imgui_internal.h"

CRITICAL_SECTION damageLogSync;
std::vector<std::pair<UINT32, float>> damageLogBuffer;
void __stdcall HDamageLogStore(UINT32 target, float damage)
{
	EnterCriticalSection(&damageLogSync);
	damageLogBuffer.emplace_back(target, damage);
	LeaveCriticalSection(&damageLogSync);
}

LPBYTE pDamageLog, oDamageLog;
void __declspec(naked) HDamageLog()
{
	__asm
	{
		mov		ecx, pWorld;
		mov		ecx, [ecx];
		mov		ecx, [ecx + 0x99C];
		cmp		ecx, [ebp + 0x50];
		jne		getBack;

		mov		ecx, [esp + 0xC];
		pushad;
		push	ecx;
		sub		ebp, 0x1720;
		push	ebp;
		call	HDamageLogStore;
		popad;

	getBack:
		jmp		oDamageLog;
	}
}

bool damageLog = false, damageLogShowTarget = true;
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

		UINT32 lastId = 0;
		for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
		{
			if (lastId != damageLogBuffer[i].first)
			{
				if (lastId)
					ImGui::Separator();
				lastId = damageLogBuffer[i].first;
			}

			if (damageLogShowTarget)
				ImGui::Text("%08X -> %.2f", damageLogBuffer[i].first, damageLogBuffer[i].second);
			else
				ImGui::Text("%.2f", damageLogBuffer[i].second);
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
	Hooks::SwitchHook("DamageLog", pDamageLog, damageLog);
}

void renderDamageLogUI()
{
	if (ImGui::CollapsingHeader("Damage Log"))
	{
		if (ImGui::Checkbox("Enabled", &damageLog))
			DamageLogSwitch();

		if (ImGui::Checkbox("Show target id", &damageLogShowTarget))
			config.setBool("inGameUI", "damageLogShowTarget", damageLogShowTarget);

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
	damageLogShowTarget = config.getBool("inGameUI", "damageLogShowTarget", true);
	ImU32 foreground = config.getUInt("inGameUI", "damageLogForeground", ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)));
	ImU32 background = config.getUInt("inGameUI", "damageLogBackground", ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 1.0f, 0.1f)));
	auto position = config.getInts("inGameUI", "damageLogPosition");
	auto size = config.getInts("inGameUI", "damageLogSize");
	damageLogForeground = ImGui::ColorConvertU32ToFloat4(foreground);
	damageLogBackground = ImGui::ColorConvertU32ToFloat4(background);
	damageLogPosition = position.size() == 2 ? ImVec2((float)position[0], (float)position[1]) : ImVec2(500, 0);
	damageLogSize = size.size() == 2 ? ImVec2((float)size[0], (float)size[1]) : ImVec2(200, 400);

	BYTE sig[] = { 0xF3, 0x0F, 0x10, 0x47, 0x08, 0x8B, 0x8F, 0xB4, 0x01, 0x00, 0x00 };
	if (FindSignature("DamageLog", sig, &pDamageLog))
		CreateHook("DamageLog", pDamageLog, HDamageLog, &oDamageLog, damageLog);
	InGameUIAdd(renderDamageLogUI);
	InGameUIAddWindow(renderDamageLog);
}