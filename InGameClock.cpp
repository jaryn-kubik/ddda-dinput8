#include "stdafx.h"
#include "InGameClock.h"
#include "ImGui/imgui_internal.h"
#include "WeaponSets.h"

bool clockEnabled;
ImFont *clockFont;
ImVec2 clockPosition;
ImVec4 clockForeground, clockBackground;
UINT32 clockTimebase, clockSize;
ImGuiWindowFlags clockFlags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar;
void renderClock()
{
	if (!clockEnabled)
		return;

	DWORD h = *GetBasePtr<UINT32>(0xB876C);
	DWORD m = *GetBasePtr<UINT32>(0xB8770);
	if (clockTimebase != 1)
	{
		DWORD t = h * 60 + m;
		t -= t % clockTimebase;
		h = t / 60;
		m = t % 60;
	}

	static CHAR clockBuf[] = "00:00";
	clockBuf[0] = (CHAR)(0x30 + h / 10);
	clockBuf[1] = (CHAR)(0x30 + h % 10);
	clockBuf[3] = (CHAR)(0x30 + m / 10);
	clockBuf[4] = (CHAR)(0x30 + m % 10);

	ImGui::PushStyleColor(ImGuiCol_WindowBg, clockBackground);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.0f, 5.0f));
	ImGui::SetNextWindowPos(clockPosition, ImGuiSetCond_Once);
	if (ImGui::Begin("InGameClock", nullptr, clockFlags))
	{
		ImGui::PushStyleColor(ImGuiCol_Text, clockForeground);
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts.back());
		ImGui::TextUnformatted(clockBuf);
		ImGui::PopFont();
		if (strlen(Hooks::weaponSetsText))
		{
			ImVec2 size = ImGui::CalcTextSize(Hooks::weaponSetsText);
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 - size.x / 2);
			ImGui::TextUnformatted(Hooks::weaponSetsText);
		}
		ImGui::PopStyleColor();
	}
	ImGui::End();
	ImGui::PopStyleVar();
	ImGui::PopStyleColor();
}

void Hooks::InGameClockSwitch() { config.setBool("inGameUI", "clock", clockEnabled = !clockEnabled); }
void Hooks::InGameClockInc(BYTE minutes) { *GetBasePtr<UINT32>(0xB8768) += minutes * 60000; }
void Hooks::InGameClockDec(BYTE minutes)
{
	DWORD time = *GetBasePtr<UINT32>(0xB8768);
	if (time < minutes * 60000U)
	{
		if (*GetBasePtr<UINT32>(0xB8760) > 0)
			(*GetBasePtr<UINT32>(0xB8760))--;
		time += 3600000 * 24;
	}
	*GetBasePtr<UINT32>(0xB8768) = time - minutes * 60000;
}

void renderClockUI()
{
	if (ImGui::CollapsingHeader("Clock"))
	{
		if (ImGui::Checkbox("Enabled", &clockEnabled))
			config.setBool("inGameUI", "clock", clockEnabled);

		if (ImGui::InputScalar<DWORD>("Size", &clockSize, 1, 1024))
			config.setUInt("inGameUI", "clockSize", clockSize);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("requires game restart");

		if (ImGui::InputScalar<DWORD>("Timebase", &clockTimebase, 1, 24 * 60))
			config.setUInt("inGameUI", "clockTimebase", clockTimebase);

		if (ImGui::ColorEdit4("Foreground", (float*)&clockForeground))
			config.setUInt("inGameUI", "clockForeground", ImGui::ColorConvertFloat4ToU32(clockForeground), true);

		if (ImGui::ColorEdit4("Background", (float*)&clockBackground))
			config.setUInt("inGameUI", "clockBackground", ImGui::ColorConvertFloat4ToU32(clockBackground), true);

		if (ImGui::Button("Save Position"))
		{
			ImGuiWindow *window = nullptr;
			ImGuiState *state = (ImGuiState*)ImGui::GetInternalState();
			for (ImGuiWindow *w : state->Windows)
				if (strcmp(w->Name, "InGameClock") == 0)
					window = w;
			if (window)
				config.setInts("inGameUI", "clockPosition", { (int)window->Pos.x, (int)window->Pos.y });
		}
	}
}

void initClockUI()
{
	CHAR syspath[MAX_PATH];
	GetWindowsDirectory(syspath, MAX_PATH);
	strcat_s(syspath, "\\Fonts\\arial.ttf");
	ImGui::GetIO().Fonts->AddFontDefault();
	ImGui::GetIO().Fonts->AddFontFromFileTTF(syspath, (float)clockSize);
}

void Hooks::InGameClock()
{
	clockSize = config.getUInt("inGameUI", "clockSize", 30);
	clockTimebase = config.getUInt("inGameUI", "clockTimebase", 1);
	if (clockTimebase < 1)
		clockTimebase = 1;

	ImU32 foreground = config.getUInt("inGameUI", "clockForeground", ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 1.0f)));
	ImU32 background = config.getUInt("inGameUI", "clockBackground", ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 1.0f, 0.1f)));
	auto position = config.getInts("inGameUI", "clockPosition");
	clockForeground = ImGui::ColorConvertU32ToFloat4(foreground);
	clockBackground = ImGui::ColorConvertU32ToFloat4(background);
	if (position.size() == 2)
		clockPosition = ImVec2((float)position[0], (float)position[1]);

	clockEnabled = config.getBool("inGameUI", "clock", false);
	if (!clockEnabled)
		logFile << "InGameClock: disabled" << std::endl;

	InGameUIAdd(renderClockUI);
	InGameUIAddWindow(renderClock);
	InGameUIAddInit(initClockUI);
}