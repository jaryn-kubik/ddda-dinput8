#include "stdafx.h"
#include "WeaponSets.h"
#include "ItemEditor.h"

LPBYTE getEquippedSkillsBaseCall, emptyEquippedSkillsCall, setEquippedSkillsCall;
LPBYTE getSetEquippedSkillsFilesReadyFlagCall, getEquippedSkillsFilesBaseCall, setEquippedSkillsFilesCall;
LPVOID pSomeBase2, pSomeBase3;
int iSomeOffsetPreEquippedSkillChange;
void __declspec(naked) HEquipSkills()
{
	__asm
	{
		mov		ecx, pSomeBase3;
		mov		ecx, [ecx];
		add		ecx, iSomeOffsetPreEquippedSkillChange;
		mov		byte ptr[ecx], 1;

		mov		ecx, pBase;
		mov		ecx, [ecx];
		xor		eax, eax;
		call	getEquippedSkillsBaseCall;
		call	emptyEquippedSkillsCall;
		call	setEquippedSkillsCall;
		ret;
	}
}

bool __declspec(naked) HRefreshSkillFileLinks()
{
	__asm
	{
		mov		edi, pBase;
		mov		edi, [edi];
		call	getSetEquippedSkillsFilesReadyFlagCall;
		test	al, al;
		jz		getBack;

		mov		ecx, pSomeBase2;
		mov		ecx, [ecx];
		xor		eax, eax;
		call	getEquippedSkillsFilesBaseCall;
		push	eax;
		call	setEquippedSkillsFilesCall;
		mov		eax, 1;

	getBack:
		ret;
	}
}

std::vector<std::array<int, 12 * 6>> weaponSetsSkills;
std::vector<std::array<int, 2>> weaponSetsWeapons;
size_t weaponSetsSkillsC = 0, weaponSetsWeaponsC = 0, weaponSetsTimeout;
CRITICAL_SECTION weaponSetsSync;
LARGE_INTEGER timerFrequency, timerLast = {}, timerCurrent = {};
bool weaponSetsRefresh = false;
char Hooks::weaponSetsText[16] = {};
void Hooks::WeaponSetsSkills()
{
	if (weaponSetsSkills.size() == 0)
		return;

	EnterCriticalSection(&weaponSetsSync);
	if (++weaponSetsSkillsC >= weaponSetsSkills.size())
		weaponSetsSkillsC = 0;
	snprintf(weaponSetsText, 16, "W%u | S%u", weaponSetsWeaponsC, weaponSetsSkillsC);
	copy(weaponSetsSkills[weaponSetsSkillsC].begin(), weaponSetsSkills[weaponSetsSkillsC].end(), GetBasePtr<int>(0xA7808));
	weaponSetsRefresh = true;
	LeaveCriticalSection(&weaponSetsSync);
}

void Hooks::WeaponSetsWeapons()
{
	if (weaponSetsWeapons.size() == 0)
		return;

	EnterCriticalSection(&weaponSetsSync);
	if (++weaponSetsWeaponsC >= weaponSetsWeapons.size())
		weaponSetsWeaponsC = 0;
	snprintf(weaponSetsText, 16, "W%u | S%u", weaponSetsWeaponsC, weaponSetsSkillsC);
	copy(weaponSetsWeapons[weaponSetsWeaponsC].begin(), weaponSetsWeapons[weaponSetsWeaponsC].end(), GetBasePtr<int>(0xA76E4));
	weaponSetsRefresh = true;
	LeaveCriticalSection(&weaponSetsSync);
}

void __stdcall HWeaponSetsRefresh()
{
	QueryPerformanceCounter(&timerCurrent);
	timerCurrent.QuadPart *= 1000;
	timerCurrent.QuadPart /= timerFrequency.QuadPart;
	if (timerCurrent.QuadPart - timerLast.QuadPart < weaponSetsTimeout)
		return;
	timerLast.QuadPart = timerCurrent.QuadPart;

	EnterCriticalSection(&weaponSetsSync);
	if (weaponSetsRefresh)
	{
		HEquipSkills();
		if (HRefreshSkillFileLinks())
			weaponSetsRefresh = false;
	}
	LeaveCriticalSection(&weaponSetsSync);
}

void __declspec(naked) HWeaponSets()
{
	__asm
	{
		pushad;
		sub		esp, 16 * 8;
		movdqu	xmmword ptr[esp + 16 * 0], xmm0;
		movdqu	xmmword ptr[esp + 16 * 1], xmm1;
		movdqu	xmmword ptr[esp + 16 * 2], xmm2;
		movdqu	xmmword ptr[esp + 16 * 3], xmm3;
		movdqu	xmmword ptr[esp + 16 * 4], xmm4;
		movdqu	xmmword ptr[esp + 16 * 5], xmm5;
		movdqu	xmmword ptr[esp + 16 * 6], xmm6;
		movdqu	xmmword ptr[esp + 16 * 7], xmm7;
		call	HWeaponSetsRefresh;
		movdqu	xmm7, xmmword ptr[esp + 16 * 7];
		movdqu	xmm6, xmmword ptr[esp + 16 * 6];
		movdqu	xmm5, xmmword ptr[esp + 16 * 5];
		movdqu	xmm4, xmmword ptr[esp + 16 * 4];
		movdqu	xmm3, xmmword ptr[esp + 16 * 3];
		movdqu	xmm2, xmmword ptr[esp + 16 * 2];
		movdqu	xmm1, xmmword ptr[esp + 16 * 1];
		movdqu	xmm0, xmmword ptr[esp + 16 * 0];
		add		esp, 16 * 8;
		popad;
		ret;
	}
}

LPBYTE pWeaponSetsP, oWeaponSetsP;
void __declspec(naked) HWeaponSetsP()
{
	__asm	call	HWeaponSets;
	__asm	jmp		oWeaponSetsP;
}

LPBYTE pWeaponSetsS, oWeaponSetsS;
void __declspec(naked) HWeaponSetsS()
{
	__asm	call	HWeaponSets;
	__asm	jmp		oWeaponSetsS;
}

bool renderWeaponSkill(int index, int weaponId, int skillCount, const char *label, const std::vector<std::pair<int, LPCSTR>> &items)
{
	bool changed = false;
	if (ImGui::TreeNode(label))
	{
		for (int i = 0; i < skillCount; i++)
			changed |= ImGui::ComboEnum<UINT32>(("##" + std::to_string(i)).c_str(), &weaponSetsSkills[index][weaponId * 6 + i], items);
		ImGui::TreePop();
	}
	return changed;
}

bool renderWeaponSet(int index)
{
	ImGui::PushID(index);
	bool treeOpened = ImGui::TreeNode(string("Skills ").append(std::to_string(index)).c_str());
	ImGui::SameLine(100.0f);

	bool changed = false;
	if (changed |= ImGui::SmallButton("Remove"))
		weaponSetsSkills.erase(weaponSetsSkills.begin() + index);

	if (treeOpened)
	{
		changed |= renderWeaponSkill(index, 0, 3, "Sword", Hooks::ListSkillsSword);
		changed |= renderWeaponSkill(index, 1, 3, "Mace", Hooks::ListSkillsSword);
		changed |= renderWeaponSkill(index, 2, 3, "Longsword", Hooks::ListSkillsLongsword);
		changed |= renderWeaponSkill(index, 6, 3, "Warhammer", Hooks::ListSkillsLongsword);
		changed |= renderWeaponSkill(index, 3, 3, "Dagger", Hooks::ListSkillsDagger);
		changed |= renderWeaponSkill(index, 4, 6, "Staff", Hooks::ListSkillsStaves);
		changed |= renderWeaponSkill(index, 5, 6, "Archistaff", Hooks::ListSkillsStaves);
		changed |= renderWeaponSkill(index, 7, 3, "Shield", Hooks::ListSkillsShield);
		changed |= renderWeaponSkill(index, 8, 3, "Magick Shield", Hooks::ListSkillsMagickShield);
		changed |= renderWeaponSkill(index, 9, 3, "Bow", Hooks::ListSkillsBow);
		changed |= renderWeaponSkill(index, 10, 3, "Longbow", Hooks::ListSkillsLongbow);
		changed |= renderWeaponSkill(index, 11, 3, "Magick Bow", Hooks::ListSkillsMagickBow);
		ImGui::TreePop();
	}
	ImGui::PopID();
	return changed;
}

bool weaponSetsEnabled;
void renderWeaponSetsUI()
{
	if (ImGui::CollapsingHeader("Weapon sets"))
	{
		if (ImGui::InputScalar<size_t>("Timeout", &weaponSetsTimeout, 0, INT_MAX, 100, 200.0f))
			config.setUInt("weaponSets", "timeout", weaponSetsTimeout);

		if (ImGui::Checkbox("Enabled", &weaponSetsEnabled))
		{
			config.setBool("weaponSets", "enabled", weaponSetsEnabled);
			Hooks::SwitchHook("WeaponSets", pWeaponSetsP, weaponSetsEnabled);
			Hooks::SwitchHook("WeaponSets", pWeaponSetsS, weaponSetsEnabled);
		}

		bool changed = false;
		ImGui::SameLine();
		if (ImGui::Button("Add skill set") && ((changed = true)))
		{
			weaponSetsSkills.emplace_back();
			std::copy(GetBasePtr<int>(0xA7808), GetBasePtr<int>(0xA7808 + 12 * 6 * sizeof(int)), weaponSetsSkills.back().data());
		}

		for (size_t i = 0; i < weaponSetsSkills.size(); i++)
			changed |= renderWeaponSet(i);

		ImGui::Separator();
		if (ImGui::TreeNode("Weapons"))
		{
			ImGui::TextUnformatted("Primary");
			ImGui::TextUnformatted("Secondary", 150.0f);
			ImGui::SameLine(270.0f);
			if (ImGui::Button("Add") && ((changed = true)))
			{
				weaponSetsWeapons.emplace_back();
				weaponSetsWeapons.back().fill(-1);
			}

			for (size_t i = 0; i < weaponSetsWeapons.size(); i++)
			{
				ImGui::PushID(i);
				changed |= ImGui::InputScalar<UINT32>("##wP", &weaponSetsWeapons[i][0], 0, UINT32_MAX, 0, 75.0f, ImGuiInputTextFlags_CharsHexadecimal);
				ImGui::SameLine();
				if (ImGui::SmallButton("Set##sP") && Hooks::pItem && ((changed = true)))
					weaponSetsWeapons[i][0] = (*(UINT32**)(Hooks::pItem + 0x04))[0x40 / 4];
				ImGui::SameLine();

				changed |= ImGui::InputScalar<UINT32>("##wS", &weaponSetsWeapons[i][1], 0, UINT32_MAX, 0, 75.0f, ImGuiInputTextFlags_CharsHexadecimal);
				ImGui::SameLine();
				if (ImGui::SmallButton("Set##sS") && Hooks::pItem && ((changed = true)))
					weaponSetsWeapons[i][1] = (*(UINT32**)(Hooks::pItem + 0x04))[0x40 / 4];
				ImGui::SameLine();

				if (ImGui::Button("Remove") && ((changed = true)))
					weaponSetsWeapons.erase(weaponSetsWeapons.begin() + i);
				ImGui::SameLine();
				ImGui::Text("Weapon %u", i);
				ImGui::PopID();
			}
			ImGui::TreePop();
		}

		if (changed)
		{
			if (weaponSetsSkills.size() == 0 && weaponSetsWeapons.size() == 0)
				Hooks::weaponSetsText[0] = '\0';
			for (auto setId : config.getSectionInts("weaponSets"))
				config.removeKey("weaponSets", std::to_string(setId).c_str());

			for (size_t setId = 0; setId < weaponSetsSkills.size(); setId++)
			{
				std::vector<int> list;
				for (size_t i = 0; i < weaponSetsSkills[setId].size(); i += 6)
				{
					list.push_back(weaponSetsSkills[setId][i + 0]);
					list.push_back(weaponSetsSkills[setId][i + 1]);
					list.push_back(weaponSetsSkills[setId][i + 2]);
					if (i / 6 == 4 || i / 6 == 5)
					{
						list.push_back(weaponSetsSkills[setId][i + 3]);
						list.push_back(weaponSetsSkills[setId][i + 4]);
						list.push_back(weaponSetsSkills[setId][i + 5]);
					}
				}
				config.setInts("weaponSets", std::to_string(setId).c_str(), list);
			}

			std::vector<int> list;
			for (size_t i = 0; i < weaponSetsWeapons.size(); i++)
			{
				list.push_back(weaponSetsWeapons[i][0]);
				list.push_back(weaponSetsWeapons[i][1]);
			}
			config.setInts("weaponSets", "weapons", list);
		}
	}
}

void Hooks::WeaponSets()
{
	InitializeCriticalSection(&weaponSetsSync);
	QueryPerformanceFrequency(&timerFrequency);
	InGameUIAdd(renderWeaponSetsUI);

	weaponSetsEnabled = config.getBool("weaponSets", "enabled", false);
	weaponSetsTimeout = config.getUInt("weaponSets", "timeout", 500);
	for (auto setId : config.getSectionInts("weaponSets"))
	{
		auto list = config.getInts("weaponSets", std::to_string(setId).c_str());
		if (list.size() != 12 * 3 + 6)
			continue;
		weaponSetsSkills.emplace_back();

		for (size_t i = 0, j = 0; i < weaponSetsSkills.back().size(); i += 6)
		{
			weaponSetsSkills.back()[i + 0] = list[j++];
			weaponSetsSkills.back()[i + 1] = list[j++];
			weaponSetsSkills.back()[i + 2] = list[j++];
			if (i / 6 == 4 || i / 6 == 5)
			{
				weaponSetsSkills.back()[i + 3] = list[j++];
				weaponSetsSkills.back()[i + 4] = list[j++];
				weaponSetsSkills.back()[i + 5] = list[j++];
			}
		}
	}

	auto list = config.getInts("weaponSets", "weapons");
	if (list.size() % 2 == 0)
		for (size_t i = 0; i < list.size();)
		{
			weaponSetsWeapons.emplace_back();
			weaponSetsWeapons.back()[0] = list[i++];
			weaponSetsWeapons.back()[1] = list[i++];
		}

	BYTE *pOffset;
	BYTE sig1[] = { 0x8B, 0x0D, 0xCC, 0xCC, 0xCC, 0xCC, 0xC6, 0x81, 0xCC, 0xCC, 0xCC, 0xCC, 0x01, 0x33, 0xF6 };
	if (!FindSignature("WeaponSets", sig1, &pOffset))
		return;
	pSomeBase3 = *(LPBYTE*)(pOffset + 2);
	iSomeOffsetPreEquippedSkillChange = *(int*)(pOffset + 8);

	BYTE sig2[] = { 0x83, 0xF8, 0x04, 0x75, 0x03, 0x33, 0xC0, 0xC3 };
	if (!FindSignature("WeaponSets", sig2, &getEquippedSkillsBaseCall))
		return;

	BYTE sig3[] = { 0x56, 0x83, 0xCE, 0xFF, 0x89, 0xB0 };
	if (!FindSignature("WeaponSets", sig3, &emptyEquippedSkillsCall))
		return;

	BYTE sig4[] = { 0x51, 0x8B, 0x90, 0xCC, 0xCC, 0xCC, 0xCC, 0x85, 0xD2, 0x74, 0x1B };
	if (!FindSignature("WeaponSets", sig4, &setEquippedSkillsCall))
		return;

	BYTE sig5[] = { 0x51, 0x53, 0x56, 0x33, 0xF6, 0xB3, 0x01, 0x83, 0xFE, 0x04 };
	if (!FindSignature("WeaponSets", sig5, &getSetEquippedSkillsFilesReadyFlagCall))
		return;

	BYTE sig6[] = { 0x8B, 0x0D, 0xCC, 0xCC, 0xCC, 0xCC, 0x8B, 0xC6, 0xE8, 0xCC, 0xCC, 0xCC, 0xCC, 0x50, 0xE8, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xC7 };
	if (!FindSignature("WeaponSets", sig6, &pOffset))
		return;
	pSomeBase2 = *(LPBYTE*)(pOffset + 2);

	BYTE sig7[] = { 0x85, 0xC0, 0x75, 0x07, 0x8B, 0x81, 0xCC, 0xCC, 0xCC, 0xCC, 0xC3, 0x48 };
	if (!FindSignature("WeaponSets", sig7, &getEquippedSkillsFilesBaseCall))
		return;

	BYTE sig8[] = { 0x53, 0x55, 0x8B, 0x6C, 0x24, 0x0C, 0xF6, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0x89 };
	if (!FindSignature("WeaponSets", sig8, &setEquippedSkillsFilesCall))
		return;

	BYTE sigP[] = { 0x03, 0xD3, 0x03, 0xD0, 0x80, 0x7C, 0x24, 0x44, 0x01 };
	BYTE sigS[] = { 0x03, 0xCB, 0x03, 0x4C, 0x24, 0x28, 0x80, 0x7C, 0x24, 0x44, 0x01 };
	if (!FindSignature("WeaponSets", sigP, &pWeaponSetsP) ||
		!FindSignature("WeaponSets", sigS, &pWeaponSetsS))
		return;
	CreateHook("WeaponSets", pWeaponSetsP, &HWeaponSetsP, &oWeaponSetsP, weaponSetsEnabled);
	CreateHook("WeaponSets", pWeaponSetsS, &HWeaponSetsS, &oWeaponSetsS, weaponSetsEnabled);
}