#include "stdafx.h"
#include "InclinationProfiles.h"
#include "ImGui/imgui_internal.h"

namespace Vocation {
	enum Enum
	{
		Fighter = 0,
		Strider,
		Mage,
		MysticKnight,
		Assassin,
		MagickArcher,
		Warrior,
		Ranger,
		Sorcerer,
		Last = Sorcerer,
		Length = Last + 1
	};
}

namespace Inclination
{
	enum Enum
	{
		Scather = 0,
		Medicant,
		Mitigator,
		Challenger,
		Utilitarian,
		Guardian,
		Nexus,
		Pioneer,
		Acquisitor,
		Last = Acquisitor,
		Length = Last + 1
	};
}

static char const * const inclinationNames[Inclination::Enum::Length] =
{
	"Scather",
	"Medicant",
	"Mitigator",
	"Challenger",
	"Utilitarian",
	"Guardian",
	"Nexus",
	"Pioneer",
	"Acquisitor"
};

static char const * const vocationNames[Vocation::Enum::Length] =
{
	"Fighter",
	"Strider",
	"Mage",
	"MysticKnight",
	"Assassin",
	"MagickArcher",
	"Warrior",
	"Ranger",
	"Sorcerer"
};

const bool VALID_PAWN_VOCATIONS[Vocation::Enum::Length] = {
	true,  // Fighter
	true,  // Strider
	true,  // Mage
	false, // Mystic Knight
	false, // Assassin
	false, // Magick Archer
	true,  // Warrior
	true,  // Ranger
	true   // Sorcerer
};

static bool mainPawnEnabled = false;
static bool pawn1Enabled = false;
static bool pawn2Enabled = false;

const float DEFAULT = 500.f;

static float profiles[Vocation::Enum::Length][Inclination::Enum::Length] =
{
	{ DEFAULT, DEFAULT, DEFAULT, DEFAULT, DEFAULT, DEFAULT, DEFAULT, DEFAULT, DEFAULT },
	{ DEFAULT, DEFAULT, DEFAULT, DEFAULT, DEFAULT, DEFAULT, DEFAULT, DEFAULT, DEFAULT },
	{ DEFAULT, DEFAULT, DEFAULT, DEFAULT, DEFAULT, DEFAULT, DEFAULT, DEFAULT, DEFAULT },
	{ DEFAULT, DEFAULT, DEFAULT, DEFAULT, DEFAULT, DEFAULT, DEFAULT, DEFAULT, DEFAULT },
	{ DEFAULT, DEFAULT, DEFAULT, DEFAULT, DEFAULT, DEFAULT, DEFAULT, DEFAULT, DEFAULT },
	{ DEFAULT, DEFAULT, DEFAULT, DEFAULT, DEFAULT, DEFAULT, DEFAULT, DEFAULT, DEFAULT },
	{ DEFAULT, DEFAULT, DEFAULT, DEFAULT, DEFAULT, DEFAULT, DEFAULT, DEFAULT, DEFAULT },
	{ DEFAULT, DEFAULT, DEFAULT, DEFAULT, DEFAULT, DEFAULT, DEFAULT, DEFAULT, DEFAULT }
};

static void applyInclinations(int pawnIndex)
{
	const int offset = 0x7F0 + pawnIndex * 0x1660;
	const int baseOffset = 0xA7000 + offset;
	const int statsOffset = baseOffset + 0x96C;
	const int inclinationsOffset = statsOffset + 0x1224;
	const int vocationOffset = baseOffset + 0x6E0;
	const UINT32 vocation = *GetBasePtr<UINT32>(vocationOffset) - 1;

	for (int inclination = 0; inclination < Inclination::Enum::Length; ++inclination) 
	{
		*(GetBasePtr<float>(inclinationsOffset + inclination * 12)) = profiles[vocation][inclination];
	}
}

static void renderInclinationUI()
{
	if (ImGui::CollapsingHeader("Inclination Profiles")) 
	{
		ImGui::Columns(3, nullptr, false);
		ImGui::Checkbox("Main Pawn Enabled", &mainPawnEnabled);
		ImGui::NextColumn();
		ImGui::Checkbox("Pawn 1 Enabled", &pawn1Enabled);
		ImGui::NextColumn();
		ImGui::Checkbox("Pawn 2 Enabled", &pawn2Enabled);
		ImGui::Columns();

		for (int vocation = 0; vocation < Vocation::Enum::Length; ++vocation) 
		{
			if (!VALID_PAWN_VOCATIONS[vocation])
				continue;

			const char * const vocationName = vocationNames[vocation];

			if (ImGui::TreeNode(vocationName))
			{
				for (int inclination = 0; inclination < Inclination::Enum::Length; ++inclination)
					ImGui::InputFloat(inclinationNames[inclination], &profiles[vocation][inclination], 0.0f, 0.0f, 0);

				ImGui::TreePop();
			}
		}

		if (ImGui::Button("Save Settings")) 
		{
			for (int vocation = 0; vocation < Vocation::Enum::Length; ++vocation)
			{
				if (!VALID_PAWN_VOCATIONS[vocation])
					continue;

				auto values = std::vector<float>(profiles[vocation], std::end(profiles[vocation]));
				config.setFloats("inclinationProfiles", vocationNames[vocation], std::move(values), 0);
				config.setBool("inclinationProfiles", "pawnEnabled", mainPawnEnabled);
				config.setBool("inclinationProfiles", "pawn1Enabled", pawn1Enabled);
				config.setBool("inclinationProfiles", "pawn2Enabled", pawn2Enabled);
			}
		}
	}
}

static void applyInclinations() {
	if (mainPawnEnabled)
		applyInclinations(0);

	if (pawn1Enabled)
		applyInclinations(1);

	if (pawn2Enabled)
		applyInclinations(2);
}

void Hooks::InclinationProfiles()
{
	for (int vocation = 0; vocation < Vocation::Enum::Length; ++vocation)
	{
		if (!VALID_PAWN_VOCATIONS[vocation])
			continue;

		auto values = config.getFloats("inclinationProfiles", vocationNames[vocation]);
		std::copy(values.begin(), values.end(), profiles[vocation]);
	}

	mainPawnEnabled = config.getBool("inclinationProfiles", "pawnEnabled", false);
	pawn1Enabled = config.getBool("inclinationProfiles", "pawn1Enabled", false);
	pawn2Enabled = config.getBool("inclinationProfiles", "pawn2Enabled", false);
	InGameUIAdd(renderInclinationUI);
	InGameUIAddTickHandler(applyInclinations);
}
