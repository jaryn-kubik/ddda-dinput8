#include "stdafx.h"
#include "PlayerStats.h"

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

	return GetWorldPtr<levelInfo>({ 0x86C + vocation * 4, 0x70, level * 4 });
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
	if (!ImGui::TreeNode(label))
		return;
	ImGui::PushID(label);

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