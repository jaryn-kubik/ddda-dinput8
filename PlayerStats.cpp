#include "stdafx.h"
#include "PlayerStats.h"
#include "Cheats.h"

struct levelInfo
{
	LPVOID Zero;
	UINT16 mLevel;
	UINT16 mHPMax;
	UINT16 mStaminaMax;
	UINT16 mAttack;
	UINT16 mDefend;
	UINT16 mMgcAttack;
	UINT16 mMgcDefend;
	UINT16 mNextExp;

	levelInfo& operator+=(const levelInfo *lvlInfo)
	{
		if (lvlInfo)
		{
			this->mHPMax += lvlInfo->mHPMax;
			this->mStaminaMax += lvlInfo->mStaminaMax;
			this->mAttack += lvlInfo->mAttack;
			this->mDefend += lvlInfo->mDefend;
			this->mMgcAttack += lvlInfo->mMgcAttack;
			this->mMgcDefend += lvlInfo->mMgcDefend;
		}
		return *this;
	}
};

levelInfo* getLevelInfo(int vocation, int level)
{
	if (level == 1)
		level = 1;
	else if (level >= 2 && level < 11)
		level = 2;
	else if (level >= 11 && level < 101)
		level = 3;
	else if (level >= 101 && level < 201)
		level = 4;
	else if (level >= 201)
		level = 5;
	else
		level = 0;

	levelInfo *****pLevels = (levelInfo*****)Hooks::pGameMain;
	if (pLevels && *pLevels)
	{
		levelInfo ***pLevelVocation = (*pLevels)[0x86C / 4 + vocation];
		if (pLevelVocation)
		{
			levelInfo **pLevelInfo = pLevelVocation[0x70 / 4];
			if (pLevelInfo)
				return pLevelInfo[level];
		}
	}
	return nullptr;
}

void renderStatsRespec(const char *label, int offset, bool *respecShow)
{
	if (!*respecShow || !getLevelInfo(0, 0))
	{
		*respecShow = false;
		return;
	}

	static levelInfo lvlResult = {};
	static int respecStart = 1;
	static int respecAbove10[9] = {}, respecAbove100[9] = {};
	static bool changed = true;
	if (changed)
	{
		changed = false;
		lvlResult = {};

		while (lvlResult.mLevel < 10)
			lvlResult += getLevelInfo(respecStart, ++lvlResult.mLevel);

		for (int vocation = 0; vocation < 9; vocation++)
			for (int i = 0; i < respecAbove10[vocation]; i++)
				lvlResult += getLevelInfo(vocation + 1, ++lvlResult.mLevel);

		if (lvlResult.mLevel >= 100)
			for (int vocation = 0; vocation < 9; vocation++)
				for (int i = 0; i < respecAbove100[vocation]; i++)
					lvlResult += getLevelInfo(vocation + 1, ++lvlResult.mLevel);
	}

	int baseOffset = 0xA7000 + offset;
	int statsOffset = baseOffset + 0x96C;
	if (ImGui::Begin(string("Respec - ").append(label).c_str(), respecShow, ImVec2(400, 625)))
	{
		ImGui::Columns(3);
		ImGui::PushItemWidth(50.0f);
		ImGui::LabelText("HP", "%hu", lvlResult.mHPMax);
		ImGui::LabelText("Stamina", "%hu", lvlResult.mStaminaMax + static_cast<UINT16>(*GetBasePtr<float>(statsOffset + 4 * 4)));
		ImGui::LabelText("Level", "%hu", lvlResult.mLevel);
		ImGui::PopItemWidth();
		ImGui::NextColumn();
		ImGui::PushItemWidth(50.0f);
		ImGui::LabelText("Strength", "%hu", lvlResult.mAttack);
		ImGui::LabelText("Defenses", "%hu", lvlResult.mDefend);
		if (ImGui::Button("Reset"))
		{
			changed = true;
			respecStart = 1;
			std::fill_n(respecAbove10, 9, 0);
			std::fill_n(respecAbove100, 9, 0);
		}
		ImGui::PopItemWidth();
		ImGui::NextColumn();
		ImGui::PushItemWidth(50.0f);
		ImGui::LabelText("Magick", "%hu", lvlResult.mMgcAttack);
		ImGui::LabelText("MDefenses", "%hu", lvlResult.mMgcDefend);
		if (ImGui::Button("Confirm"))
		{
			*GetBasePtr<UINT16>(baseOffset + 0xDD0) = lvlResult.mLevel;
			*GetBasePtr<UINT16>(baseOffset + 0x994) = 0; //XP
			//*GetBasePtr<UINT16>(baseOffset + 0x998) = 0; //next XP

			*GetBasePtr<float>(statsOffset + 4 * 0) = static_cast<float>(lvlResult.mHPMax);
			*GetBasePtr<float>(statsOffset + 4 * 1) = static_cast<float>(lvlResult.mHPMax);
			*GetBasePtr<float>(statsOffset + 4 * 2) = static_cast<float>(lvlResult.mHPMax);

			*GetBasePtr<float>(statsOffset + 4 * 3) = static_cast<float>(lvlResult.mStaminaMax);
			*GetBasePtr<float>(statsOffset + 4 * 5) = static_cast<float>(lvlResult.mStaminaMax);

			*GetBasePtr<float>(statsOffset + 4 * 6) = static_cast<float>(lvlResult.mAttack);
			*GetBasePtr<float>(statsOffset + 4 * 7) = static_cast<float>(lvlResult.mDefend);
			*GetBasePtr<float>(statsOffset + 4 * 8) = static_cast<float>(lvlResult.mMgcAttack);
			*GetBasePtr<float>(statsOffset + 4 * 9) = static_cast<float>(lvlResult.mMgcDefend);
		}
		ImGui::PopItemWidth();
		ImGui::Columns();
		ImGui::Separator();

		ImGui::TextUnformatted("Starting vocation:");
		ImGui::Columns(3, nullptr, false);
		changed |= ImGui::RadioButton("Fighter", &respecStart, 1);
		ImGui::NextColumn();
		changed |= ImGui::RadioButton("Strider", &respecStart, 2);
		ImGui::NextColumn();
		changed |= ImGui::RadioButton("Mage", &respecStart, 3);
		ImGui::Columns();
		ImGui::Separator();

		ImGui::TextUnformatted("Levels 11 - 100:");
		ImGui::PushID("Levels11-100");
		int respecAbove10Sum = std::accumulate(respecAbove10, respecAbove10 + 9, 0);
		for (auto &vocation : Hooks::ListVocations)
		{
			if (offset && (vocation.first == 4 || vocation.first == 5 || vocation.first == 6))
				continue;
			int *val = respecAbove10 + vocation.first - 1;
			changed |= ImGui::SliderInt(vocation.second, val, 0, 90 - respecAbove10Sum + *val);
		}
		ImGui::PopID();

		ImGui::Separator();
		ImGui::TextUnformatted("Levels 101 - 200:");
		ImGui::PushID("Levels101-200");
		int respecAbove100Sum = std::accumulate(respecAbove100, respecAbove100 + 9, 0);
		for (auto &vocation : Hooks::ListVocations)
		{
			if (offset && (vocation.first == 4 || vocation.first == 5 || vocation.first == 6))
				continue;
			int *val = respecAbove100 + vocation.first - 1;
			changed |= ImGui::SliderInt(vocation.second, val, 0, 100 - respecAbove100Sum + *val);
		}
		ImGui::PopID();
	}
	ImGui::End();
}

void renderStatsVocation(const char *label, int offset)
{
	ImGui::PushID(label);
	ImGui::InputScalar<int>("##rank", GetBasePtr(offset + 0x28 * 0), 0, 9, -1, 25.0f);
	ImGui::SameLine();
	ImGui::InputScalar<int>("##xp", GetBasePtr(offset + 0x28 * 1), 0, INT_MAX, -1, 60.0f);
	ImGui::SameLine();
	ImGui::InputScalar<int>("##next", GetBasePtr(offset + 0x28 * 2), 0, INT_MAX, -1, 60.0f);
	ImGui::SameLine();
	ImGui::Text("(rank/xp/next) %s", label);
	ImGui::PopID();
}

void renderStatsParty(const char *label, int offset, bool *respecShow)
{
	ImGui::PushID(label);
	if (!ImGui::TreeNode(label))
		return;

	int baseOffset = 0xA7000 + offset;
	int statsOffset = baseOffset + 0x96C;
	if (ImGui::TreeNode("Attributes"))
	{
		ImGui::Columns(3, nullptr, false);
		ImGui::InputScalar<int>("Level", GetBasePtr(baseOffset + 0xDD0), 0, 200, -1);
		ImGui::InputScalar<int>("DP", GetBasePtr(baseOffset + 0xA14), 0, INT_MAX, -1);
		ImGui::NextColumn();
		ImGui::InputScalar<int>("XP", GetBasePtr(baseOffset + 0x994), 0, INT_MAX, -1);
		ImGui::InputScalar<int>("Next", GetBasePtr(baseOffset + 0x998), 0, INT_MAX, -1);
		ImGui::NextColumn();
		if (ImGui::Button("Level Up"))
			*GetBasePtr<UINT32>(baseOffset + 0x994) = *GetBasePtr<UINT32>(baseOffset + 0x998);
		if (ImGui::Button("Respec"))
			*respecShow = true;
		ImGui::Columns();

		ImGui::Separator();
		ImGui::InputFloatN("HP (Current/Max/Max+)", GetBasePtr<float>(statsOffset + 4 * 0), 3, 70.0f);
		ImGui::InputFloatN("Stamina (Current/Max/Max+)", GetBasePtr<float>(statsOffset + 4 * 3), 3, 70.0f);

		ImGui::Separator();
		ImGui::Columns(2, nullptr, false);
		ImGui::InputFloatN("Strength", GetBasePtr<float>(statsOffset + 4 * 6), 1, 70.0f);
		ImGui::InputFloatN("Defenses", GetBasePtr<float>(statsOffset + 4 * 7), 1, 70.0f);
		ImGui::NextColumn();
		ImGui::InputFloatN("Magick", GetBasePtr<float>(statsOffset + 4 * 8), 1, 70.0f);
		ImGui::InputFloatN("Magick Defenses", GetBasePtr<float>(statsOffset + 4 * 9), 1, 70.0f);
		ImGui::Columns();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Vocations"))
	{
		ImGui::ComboEnum<UINT32>("Current", GetBasePtr(baseOffset + 0x6E0), Hooks::ListVocations);

		int vocationOffset = statsOffset + 13 * 4;
		renderStatsVocation("Fighter", vocationOffset + 4 * 0);
		renderStatsVocation("Strider", vocationOffset + 4 * 1);
		renderStatsVocation("Mage", vocationOffset + 4 * 2);
		if (!offset)// player
		{
			renderStatsVocation("Mystic Knight", vocationOffset + 4 * 3);
			renderStatsVocation("Assassin", vocationOffset + 4 * 4);
			renderStatsVocation("Magick Archer", vocationOffset + 4 * 5);
		}
		renderStatsVocation("Warrior", vocationOffset + 4 * 6);
		renderStatsVocation("Ranger", vocationOffset + 4 * 7);
		renderStatsVocation("Sorcerer", vocationOffset + 4 * 8);
		ImGui::TreePop();
	}

	if (offset && ImGui::TreeNode("Inclinations"))//main pawn
	{
		int inclinationsOffset = statsOffset + 0x1224;
		ImGui::InputFloat("Scather", GetBasePtr<float>(inclinationsOffset += 0));
		ImGui::InputFloat("Medicant", GetBasePtr<float>(inclinationsOffset += 4 * 3));
		ImGui::InputFloat("Mitigator", GetBasePtr<float>(inclinationsOffset += 4 * 3));
		ImGui::InputFloat("Challenger", GetBasePtr<float>(inclinationsOffset += 4 * 3));
		ImGui::InputFloat("Utilitarian", GetBasePtr<float>(inclinationsOffset += 4 * 3));
		ImGui::InputFloat("Guardian", GetBasePtr<float>(inclinationsOffset += 4 * 3));
		ImGui::InputFloat("Nexus", GetBasePtr<float>(inclinationsOffset += 4 * 3));
		ImGui::InputFloat("Pioneer", GetBasePtr<float>(inclinationsOffset += 4 * 3));
		ImGui::InputFloat("Acquisitor", GetBasePtr<float>(inclinationsOffset += 4 * 3));
		ImGui::InputFloat("Skill use", GetBasePtr<float>(inclinationsOffset += 4 * 5));
		ImGui::TreePop();
	}
	ImGui::TreePop();
	ImGui::PopID();
}

bool renderStatsSkill(int offset, int skillCount, const char *label, const std::vector<std::pair<int, LPCSTR>> &items)
{
	bool changed = false;
	if (ImGui::TreeNode(label))
	{
		for (int i = 0; i < skillCount; i++)
			changed |= ImGui::ComboEnum<UINT32>(("##" + std::to_string(i)).c_str(), GetBasePtr(offset + 4 * i), items);
		ImGui::TreePop();
	}
	return changed;
}

bool renderStatsSkills(const char *label, int offset, std::pair<bool, int> *state)
{
	bool changed = false;
	bool treeOpened = ImGui::TreeNode(label);
	ImGui::SameLine(150.0f);
	if (ImGui::SmallButton(string("Learned Skills##").append(label).c_str()))
		state->first = true;
	if (treeOpened)
	{
		int skillsOffset = 0xA7808 + offset;
		changed |= renderStatsSkill(skillsOffset + 24 * 12, 6, "Augments", Hooks::ListSkillsAugments);
		changed |= renderStatsSkill(skillsOffset + 24 * 0, 3, "Sword", Hooks::ListSkillsSword);
		changed |= renderStatsSkill(skillsOffset + 24 * 1, 3, "Mace", Hooks::ListSkillsSword);
		changed |= renderStatsSkill(skillsOffset + 24 * 2, 3, "Longsword", Hooks::ListSkillsLongsword);
		changed |= renderStatsSkill(skillsOffset + 24 * 6, 3, "Warhammer", Hooks::ListSkillsLongsword);
		changed |= renderStatsSkill(skillsOffset + 24 * 3, 3, "Dagger", Hooks::ListSkillsDagger);
		changed |= renderStatsSkill(skillsOffset + 24 * 4, 6, "Staff", Hooks::ListSkillsStaves);
		changed |= renderStatsSkill(skillsOffset + 24 * 5, 6, "Archistaff", Hooks::ListSkillsStaves);
		changed |= renderStatsSkill(skillsOffset + 24 * 7, 3, "Shield", Hooks::ListSkillsShield);
		changed |= renderStatsSkill(skillsOffset + 24 * 8, 3, "Magick Shield", Hooks::ListSkillsMagickShield);
		changed |= renderStatsSkill(skillsOffset + 24 * 9, 3, "Bow", Hooks::ListSkillsBow);
		changed |= renderStatsSkill(skillsOffset + 24 * 10, 3, "Longbow", Hooks::ListSkillsLongbow);
		changed |= renderStatsSkill(skillsOffset + 24 * 11, 3, "Magick Bow", Hooks::ListSkillsMagickBow);
		ImGui::TreePop();
	}
	return changed;
}

std::vector<std::pair<LPCSTR, const std::vector<std::pair<int, LPCSTR>>*>> SkillTypeList =
{
	{ "Sword", &Hooks::ListSkillsSword },
	{ "Longsword", &Hooks::ListSkillsLongsword },
	{ "Dagger", &Hooks::ListSkillsDagger },
	{ "Staff", &Hooks::ListSkillsStaves },
	{ "Shield", &Hooks::ListSkillsShield },
	{ "MShield", &Hooks::ListSkillsMagickShield },
	{ "Bow", &Hooks::ListSkillsBow },
	{ "MBow", &Hooks::ListSkillsMagickBow },
	{ "Longbow", &Hooks::ListSkillsLongbow },
	{ "Core", &Hooks::ListSkillsCore },
	{ "Augments", &Hooks::ListSkillsAugments }
};
void renderStatsLearnedSkills(const char *label, int offset, std::pair<bool, int> *state)
{
	if (state->first)
	{
		ImGui::Begin(string("Learned skills - ").append(label).c_str(), &(state->first), ImVec2(500, 400));
		ImGui::Columns(5, nullptr, false);
		for (size_t i = 0; i < SkillTypeList.size(); i++)
		{
			ImGui::RadioButton(SkillTypeList[i].first, &(state->second), i);
			if (i % 2 == 1)
				ImGui::NextColumn();
		}
		ImGui::Columns();
		ImGui::Separator();

		int learnedOffset = 0xA7E00 + offset;
		if (state->second == 10)//augments
			learnedOffset += 0x38 * 3 + 4;
		auto skillList = SkillTypeList[state->second].second;
		bool twoLevels = state->second < 9;
		for (size_t i = 1; i < skillList->size(); i++)
		{
			int id = skillList->at(i).first;
			if (id % 10 == 0 && i > 1)
				ImGui::Separator();

			ImGui::PushID(i);
			ImGui::Text(skillList->at(i).second);
			ImGui::SameLine(400.0f);
			UINT32 *pLvl1 = GetBasePtr<UINT32>(learnedOffset + (id / 32) * 4);
			UINT32 *pLvl2 = GetBasePtr<UINT32>(learnedOffset + 0x38 + (id / 32) * 4);
			UINT32 flagBit = 1U << id % 32;

			if (ImGui::CheckboxFlags("##lvl1", pLvl1, flagBit) && *pLvl1 & flagBit && twoLevels)
				*pLvl2 = *pLvl2 & ~flagBit;
			if (twoLevels)
			{
				ImGui::SameLine();
				if (ImGui::CheckboxFlags("##lvl2", pLvl2, flagBit) && *pLvl2 & flagBit)
					*pLvl1 = *pLvl1 & ~flagBit;
			}
			ImGui::PopID();
		}
		ImGui::End();
	}
}

LPBYTE pEquipChanged;
void renderStatsUI()
{
	static bool respecPlayer = false, respectPawn1 = false, respectPawn2 = false, respectPawn3 = false;
	renderStatsRespec("Player", 0, &respecPlayer);
	renderStatsRespec("Main Pawn", 0x7F0, &respectPawn1);
	renderStatsRespec("Pawn 1", 0x7F0 + 0x1660, &respectPawn2);
	renderStatsRespec("Pawn 2", 0x7F0 + 0x1660 + 0x1660, &respectPawn3);

	if (ImGui::CollapsingHeader("Stats"))
	{
		ImGui::PushID("Stats");
		ImGui::Columns(2, nullptr, false);
		ImGui::InputScalar<int>("Gold", GetBasePtr(0xA7A18), 0, INT_MAX, 1000);
		ImGui::NextColumn();
		ImGui::InputScalar<int>("RC", GetBasePtr(0xA7A1C), 0, INT_MAX, 100);
		ImGui::Columns();

		renderStatsParty("Player", 0, &respecPlayer);
		renderStatsParty("Main Pawn", 0x7F0, &respectPawn1);
		renderStatsParty("Pawn 1", 0x7F0 + 0x1660, &respectPawn2);
		renderStatsParty("Pawn 2", 0x7F0 + 0x1660 + 0x1660, &respectPawn3);
		ImGui::PopID();
	}

	static std::pair<bool, int> learnedSkills[4] = {};
	if (ImGui::CollapsingHeader("Skills"))
	{
		ImGui::PushID("Skills");
		if (renderStatsSkills("Player", 0, learnedSkills) && pEquipChanged)
			pEquipChanged[0] = 1;
		if (renderStatsSkills("Main Pawn", 0x7F0, learnedSkills + 1) && pEquipChanged)
			pEquipChanged[1] = 1;
		if (renderStatsSkills("Pawn 1", 0x7F0 + 0x1660, learnedSkills + 2) && pEquipChanged)
			pEquipChanged[2] = 1;
		if (renderStatsSkills("Pawn 2", 0x7F0 + 0x1660 + 0x1660, learnedSkills + 3) && pEquipChanged)
			pEquipChanged[3] = 1;
		ImGui::PopID();
	}
	ImGui::PushID("Learned skills");
	renderStatsLearnedSkills("Player", 0, learnedSkills);
	renderStatsLearnedSkills("Main Pawn", 0x7F0, learnedSkills + 1);
	renderStatsLearnedSkills("Pawn 1", 0x7F0 + 0x1660, learnedSkills + 2);
	renderStatsLearnedSkills("Pawn 2", 0x7F0 + 0x1660 + 0x1660, learnedSkills + 3);
	ImGui::PopID();
}

void Hooks::PlayerStats()
{
	BYTE sigRun[] = { 0x6A, 0x00, 0x6A, 0x00, 0x6A, 0x00,	//push 0, 0, 0
					0x68, 0xCC, 0xCC, 0xCC, 0xCC,			//push pEquipChanged
					0x8B, 0xF8, 0xE8 };						//mov	edi, eax
	if (FindSignature("EquipChanged", sigRun, &pEquipChanged))
		pEquipChanged = *(BYTE**)(pEquipChanged + 7);
	else
		pEquipChanged = nullptr;
	InGameUIAdd(renderStatsUI);
}

const std::vector<std::pair<int, LPCSTR>> Hooks::ListVocations =
{
	{ 1, "Fighter"},
	{ 2, "Strider"},
	{ 3, "Mage"},
	{ 4, "Mystic Knight"},
	{ 5, "Assassin"},
	{ 6, "Magick Archer"},
	{ 7, "Warrior"},
	{ 8, "Ranger"},
	{ 9, "Sorcerer"}
};

const std::vector<std::pair<int, LPCSTR>> Hooks::ListSkillsCore =
{
	{ -1, "Empty" },
	{ 30, "30: Sword - Dire Onslaught" },
	{ 31, "31: Sword - Takedown" },
	{ 32, "32: Sword - Controlled Fall" },
	{ 90, "90: Longsword - Devastate" },
	{ 91, "91: Longsword - Eviscerate" },
	{ 92, "92: Longsword - Controlled Fall" },
	{ 140, "140: Dagger - Engrave" },
	{ 141, "141: Dagger - Roundelay" },
	{ 142, "142: Dagger - Double Vault" },
	{ 143, "143: Dagger - Forward Roll" },
	{ 200, "200: Staff - Focused Bolt" },
	{ 201, "201: Staff - Magick Agent" },
	{ 202, "202: Staff - Levitate" },
	{ 260, "260: Shield - Deflect" },
	{ 300, "300: MShield - Reflect" },
	{ 340, "340: Bow - Quick Loose" },
	{ 341, "341: MBow - True Seeker" },
	{ 390, "390: Longbow - Quick Loose" }
};

const std::vector<std::pair<int, LPCSTR>> Hooks::ListSkillsSword =
{
	{ -1, "-1: Empty" },
	{ 40, "40: Blink Strike | Burst Strike" },
	{ 41, "41: Broad Cut | Broad Slash" },
	{ 42, "42: Downthrust | Downcrack" },
	{ 43, "43: Tusk Toss | Antler Toss" },
	{ 44, "44: Compass Slash | Full Moon Slash" },
	{ 45, "45: Skyward Lash | Heavenward Lash" },
	{ 46, "46: Flesh Skewer | Soul Skewer" },
	{ 47, "47: Hindsight Slash | Hindsight Sweep" },
	{ 48, "48: Stone Will | Steel Will" },
	{ 49, "49: Legion's Bite | Dragon's Maw" },
	{ 50, "50: Perilous Sigil | Ruinous Sigil" },
	{ 51, "51: Magick Cannon | Great Cannon" },
	{ 52, "52: Funnel Sigil | Vortex Sigil" },
	{ 53, "53: Sky Dance | Sky Rapture" },
	{ 54, "54: Stone Grove | Stone Forest" },
	{ 55, "55: Intimate Strike | Initimate Gambit" },
	{ 56, "56: Windmill Slash | Great Windmill" },
	{ 57, "57: Powder Charge | Powder Blast" },
	{ 58, "58: Gouge | Dire Gouge" },
	{ 59, "59: Clarity | Clairvoyance" }
};

const std::vector<std::pair<int, LPCSTR>> Hooks::ListSkillsLongsword =
{
	{ -1, "-1: Empty" },
	{ 100, "100: Upward Strike | Whirlwind Slash" },
	{ 101, "101: Pommel Strike | Pommel Bash" },
	{ 102, "102: Savage Lunge | Indomitable Lunge" },
	{ 103, "103: Escape Slash | Exodus Slash" },
	{ 104, "104: Savage Lash | Indomitable Lash" },
	{ 105, "105: Ladder Blade | Catapult Blade" },
	{ 106, "106: Spark Slash | Corona Slash" },
	{ 107, "107: Act of Atonement | Act of Vengeance" },
	{ 108, "108: Battle Cry | War Cry" },
	{ 109, "109: Arc of Might | Arc of Deliverance" }
};

const std::vector<std::pair<int, LPCSTR>> Hooks::ListSkillsDagger =
{
	{ -1, "-1: Empty" },
	{ 150, "150: Biting Wind | Cutting Wind" },
	{ 151, "151: Toss and Trigger | Advanced Trigger" },
	{ 152, "152: Scarlet Kisses | Hundred Kisses" },
	{ 153, "153: Dazzle Hold | Dazzle Blast" },
	{ 154, "154: Sprint | Mad Dash" },
	{ 155, "155: Helm Splitter | Skull Splitter" },
	{ 156, "156: Ensnare | Implicate" },
	{ 157, "157: Pilfer | Master Thief" },
	{ 158, "158: Reset | Instant Reset" },
	{ 159, "159: Stepping Stone | Leaping Stone" },
	{ 160, "160: Sunburst | Sunflare" },
	{ 161, "161: Shadowpin | Shadowshackle" },
	{ 162, "162: Scension | Grand Scension" },
	{ 163, "163: Magick Rebuffer | Magick Rebalancer" },
	{ 164, "164: Wind Harness | Gale Harness" },
	{ 165, "165: Back Kick | Escape Onslaught" },
	{ 166, "166: Spiderbite | Snakebite" },
	{ 167, "167: Backfire | Immolation" },
	{ 168, "168: Stealth | Invisibility" },
	{ 169, "169: Easy Kill | Masterful Kill" }
};

const std::vector<std::pair<int, LPCSTR>> Hooks::ListSkillsStaves =
{
	{ -1, "-1: Empty" },
	{ 210, "210: Ingle | High Ingle" },
	{ 211, "211: Frazil | High Frazil" },
	{ 212, "212: Levin | High Levin" },
	{ 213, "213: Comestion | High Comestion" },
	{ 214, "214: Frigor | High Frigor" },
	{ 215, "215: Brontide | High Brontide" },
	{ 216, "216: Grapnel | High Grapnel" },
	{ 217, "217: Silentium | High Silentium" },
	{ 218, "218: Blearing | High Blearing" },
	{ 219, "219: Lassitude | High Lassitude" },
	{ 220, "220: Anodyne | High Anodyne" },
	{ 221, "221: Halidom | High Halidom" },
	{ 222, "222: Fire Boon | Fire Affinity" },
	{ 223, "223: Ice Boon | Ice Affinity" },
	{ 224, "224: Thunder Boon | Thunder Affinity" },
	{ 225, "225: Holy Boon | Holy Affinity" },
	{ 226, "226: Dark Boon | Dark Affinity" },
	{ 227, "227: Bolide | High Bolide" },
	{ 228, "228: Gicel | High Gicel" },
	{ 229, "229: Fulmination | High Fulmination" },
	{ 230, "230: Seism | High Seism" },
	{ 231, "231: Maelstrom | High Maelstrom" },
	{ 232, "232: Exequy | High Exequy" },
	{ 233, "233: Petrifaction | High Petrifaction" },
	{ 234, "234: Miasma | High Miasma" },
	{ 235, "235: Perdition | High Perdition" },
	{ 236, "236: Sopor | High Sopor" },
	{ 237, "237: Voidspell | High Voidspell" },
	{ 238, "238: Spellscreen | High Spellscreen" },
	{ 239, "239: Necromancy | High Necromancy" }
};

/*const std::vector<std::pair<int, LPCSTR>> Hooks::skillsStaff[] =
{
	{ -1, "-1: Empty" },
	{ 210, "210: Ingle | High Ingle" },
	{ 211, "211: Frazil | High Frazil" },
	{ 212, "212: Levin | High Levin" },
	{ 213, "213: Comestion | High Comestion" },
	{ 214, "214: Frigor | High Frigor" },
	{ 215, "215: Brontide | High Brontide" },
	{ 216, "216: Grapnel | High Grapnel" },
	{ 217, "217: Silentium | High Silentium" },
	{ 218, "218: Blearing | High Blearing" },
	{ 220, "220: Anodyne | High Anodyne" },
	{ 221, "221: Halidom | High Halidom" },
	{ 222, "222: Fire Boon | Fire Affinity" },
	{ 223, "223: Ice Boon | Ice Affinity" },
	{ 224, "224: Thunder Boon | Thunder Affinity" },
	{ 225, "225: Holy Boon | Holy Affinity" },
	{ 226, "226: Dark Boon | Dark Affinity" },
	{ 227, "227: Bolide | High Bolide" },
	{ 235, "235: Perdition | High Perdition" },
	{ 236, "236: Sopor | High Sopor" },
	{ 238, "238: Spellscreen | High Spellscreen" }
};

const std::vector<std::pair<int, LPCSTR>> Hooks::skillsArchistaff[] =
{
	{ -1, "-1: Empty" },
	{ 210, "210: Ingle | High Ingle" },
	{ 211, "211: Frazil | High Frazil" },
	{ 212, "212: Levin | High Levin" },
	{ 213, "213: Comestion | High Comestion" },
	{ 214, "214: Frigor | High Frigor" },
	{ 215, "215: Brontide | High Brontide" },
	{ 217, "217: Silentium | High Silentium" },
	{ 218, "218: Blearing | High Blearing" },
	{ 219, "219: Lassitude | High Lassitude"},
	{ 222, "222: Fire Boon | Fire Affinity" },
	{ 223, "223: Ice Boon | Ice Affinity" },
	{ 224, "224: Thunder Boon | Thunder Affinity" },
	{ 225, "225: Holy Boon | Holy Affinity" },
	{ 226, "226: Dark Boon | Dark Affinity" },
	{ 227, "227: Bolide | High Bolide" },
	{ 228, "228: Gicel | High Gicel" },
	{ 229, "229: Fulmination | High Fulmination" },
	{ 230, "230: Seism | High Seism" },
	{ 231, "231: Maelstrom | High Maelstrom" },
	{ 232, "232: Exequy | High Exequy" },
	{ 233, "233: Petrifaction | High Petrifaction" },
	{ 234, "234: Miasma | High Miasma" },
	{ 237, "237: Voidspell | High Voidspell" },
	{ 239, "239: Necromancy | High Necromancy" }
};*/

const std::vector<std::pair<int, LPCSTR>> Hooks::ListSkillsShield =
{
	{ -1, "-1: Empty" },
	{ 270, "270: Shield Strike | Shield Storm" },
	{ 271, "271: Springboard | Launchboard" },
	{ 272, "272: Shield Summons | Shield Drum" },
	{ 273, "273: Cymbal Attack | Cymbal Onslaught" },
	{ 274, "274: Sheltered Spike | Sheltered Assault" },
	{ 275, "275: Perfect Defense | Divine Defense" },
	{ 276, "276: Moving Castle | Swift Castle" },
	{ 277, "277: Flight Response | Enhanced Response" },
	{ 278, "278: Staredown | Showdown" }
};

const std::vector<std::pair<int, LPCSTR>> Hooks::ListSkillsMagickShield =
{
	{ -1, "-1: Empty" },
	{ 310, "310: Firecounter | Flame Riposte" },
	{ 311, "311: Icecounter | Frost Riposte" },
	{ 312, "312: Thundercounter | Thunder Riposte" },
	{ 313, "313: Holycounter | Blessed Riposte" },
	{ 314, "314: Darkcounter | Abyssal Riposte" },
	{ 315, "315: Fire Enchanter | Flame Trance" },
	{ 316, "316: Ice Enchanter | Ice Enchanter" },
	{ 317, "317: Thunder Enchanter | Thunder Enchanter" },
	{ 318, "318: Holy Enchanter | Blessed Trance" },
	{ 319, "319: Dark Enchanter | Abyssal Trance" },
	{ 320, "320: Holy Glare | Holy Furor" },
	{ 321, "321: Dark Anguish | Abyssal Anguish" },
	{ 322, "322: Holy Wall | Holy Fortress" },
	{ 323, "323: Demonspite | Demonswrath" },
	{ 324, "324: Holy Aid | Holy Grace" }
};

const std::vector<std::pair<int, LPCSTR>> Hooks::ListSkillsBow =
{
	{ -1, "-1: Empty" },
	{ 350, "350: Threefold Arrow | Fivefold Flurry" },
	{ 351, "351: Triad Shot | Pentad Shot" },
	{ 352, "352: Full Bend | Mighty Bend" },
	{ 353, "353: Cloudburst Volley | Downpour Volley" },
	{ 354, "354: Splinter Dart | Fracture Dart" },
	{ 355, "355: Whistle Dart | Shriek Dart" },
	{ 356, "356: Keen Sight | Lyncean Sight" },
	{ 357, "357: Puncture Dart | Skewer Dart" },
	{ 358, "358: Blunting Arrow | Plegic Arrow" }
};

const std::vector<std::pair<int, LPCSTR>> Hooks::ListSkillsLongbow =
{
	{ -1, "-1: Empty" },
	{ 400, "400: Sixfold Arrow | Tenfold Flurry" },
	{ 401, "401: Heptad Shot | Endecad Shot" },
	{ 402, "402: Dire Arrow | Deathly Arrow" },
	{ 403, "403: Foot Binder | Body Binder" },
	{ 404, "404: Invasive Arrow | Crippling Arrow" },
	{ 405, "405: Flying Din | Fearful Din" },
	{ 406, "406: Meteor Shot | Comet Shot" },
	{ 407, "407: Whirling Arrow | Spiral Arrow" },
	{ 408, "408: Gamble Draw | Great Gamble" }
};

const std::vector<std::pair<int, LPCSTR>> Hooks::ListSkillsMagickBow =
{
	{ -1, "-1: Empty" },
	{ 359, "359: Threefold Bolt | Sixfold Bolt" },
	{ 360, "360: Seeker Bolt | Hunter Bolt" },
	{ 361, "361: Explosive Bolt | Explosive Rivet" },
	{ 362, "362: Ricochet Seeker | Ricochet Hunter" },
	{ 363, "363: Magickal Flare | Magickal Gleam" },
	{ 364, "364: Funnel Trail | Vortex Trail" },
	{ 365, "365: Ward Arrow | Great Ward Arrow" },
	{ 366, "366: Bracer Arrow | Great Bracer Arrow" },
	{ 367, "367: Sacrificial Bolt | Great Sacrifice" }
};

const std::vector<std::pair<int, LPCSTR>> Hooks::ListSkillsAugments =
{
	{ -1, "-1: Empty" },
	{ 0, "0: Fitness" },
	{ 1, "1: Sinew" },
	{ 2, "2: Egression" },
	{ 3, "3: Prescience" },
	{ 4, "4: Exhilaration" },
	{ 5, "5: Vehemence" },
	{ 6, "6: Vigilance" },
	{ 10, "10: Leg-Strength" },
	{ 11, "11: Arm-Strength" },
	{ 12, "12: Grit" },
	{ 13, "13: Damping" },
	{ 14, "14: Dexterity" },
	{ 15, "15: Eminence" },
	{ 16, "16: Endurance" },
	{ 20, "20: Infection" },
	{ 21, "21: Equanimity" },
	{ 22, "22: Beatitude" },
	{ 23, "23: Perpetuation" },
	{ 24, "24: Intervention" },
	{ 25, "25: Attunement" },
	{ 26, "26: Apotropaism" },
	{ 30, "30: Adamance" },
	{ 31, "31: Periphery" },
	{ 32, "32: Sanctuary" },
	{ 33, "33: Restoration" },
	{ 34, "34: Retribution" },
	{ 35, "35: Reinforcement" },
	{ 36, "36: Fortitude" },
	{ 40, "40: Watchfulness" },
	{ 41, "41: Preemption" },
	{ 42, "42: Autonomy" },
	{ 43, "43: Bloodlust" },
	{ 44, "44: Entrancement" },
	{ 45, "45: Sanguinity" },
	{ 46, "46: Toxicity" },
	{ 50, "50: Resilience" },
	{ 51, "51: Resistance" },
	{ 52, "52: Detection" },
	{ 53, "53: Regeneration" },
	{ 54, "54: Allure" },
	{ 55, "55: Potential" },
	{ 56, "56: Magnitude" },
	{ 60, "60: Temerity" },
	{ 61, "61: Audacity" },
	{ 62, "62: Proficiency" },
	{ 63, "63: Ferocity" },
	{ 64, "64: Impact" },
	{ 65, "65: Bastion" },
	{ 66, "66: Clout" },
	{ 70, "70: Trajectory" },
	{ 71, "71: Morbidity" },
	{ 72, "72: Precision" },
	{ 73, "73: Stability" },
	{ 74, "74: Efficacy" },
	{ 75, "75: Radiance" },
	{ 76, "76: Longevity" },
	{ 80, "80: Gravitas" },
	{ 81, "81: Articulacy" },
	{ 82, "82: Conservation" },
	{ 83, "83: Emphasis" },
	{ 84, "84: Suasion" },
	{ 85, "85: Acuity" },
	{ 86, "86: Awareness" },
	{ 90, "90: Unknown" },
	{ 91, "91: Suasion" },
	{ 92, "92: Thrift" },
	{ 93, "93: Weal" },
	{ 94, "94: Renown" },
	{ 100, "100: Predation" },
	{ 101, "101: Fortune" },
	{ 102, "102: Tenacity" },
	{ 103, "103: Conveyance" },
	{ 104, "104: Acquisition" },
	{ 105, "105: Prolongation" },
	{ 107, "107: Mettle" },
	{ 108, "108: Athleticism" },
	{ 109, "109: Recuperation" },
	{ 110, "110: Adhesion" },
	{ 111, "111: Opportunism" },
	{ 112, "112: Flow" },
	{ 113, "113: Grace" },
	{ 114, "114: Facility" }
};