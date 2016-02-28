#include "stdafx.h"
#include "WeaponSets.h"
#include "PlayerStats.h"

LPBYTE getEquippedSkillsBaseCall;
LPBYTE emptyEquippedSkillsCall;
LPBYTE setEquippedSkillsCall;
LPBYTE getSetEquippedSkillsFilesReadyFlagCall;
LPBYTE getEquippedSkillsFilesBaseCall;
LPBYTE setEquippedSkillsFilesCall;
LPVOID pSomeBase2, pSomeBase3;
int iSomeOffsetPreEquippedSkillChange;
bool bRefreshSkillFileLinksStatus = false, bWeaponModKeyPressed = false;
void __declspec(naked) HEquipSkills()
{
	__asm
	{
		mov		bRefreshSkillFileLinksStatus, 1;
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

void __declspec(naked) HRefreshSkillFileLinks()
{
	__asm
	{
		mov		edi, pBase;
		mov		edi, [edi];
		call	getSetEquippedSkillsFilesReadyFlagCall;
		test	al, al;
		je		getBack;

		mov		bRefreshSkillFileLinksStatus, 0;
		mov		ecx, pSomeBase2;
		mov		ecx, [ecx];
		xor		eax, eax;
		call	getEquippedSkillsFilesBaseCall;
		push	eax;
		call	setEquippedSkillsFilesCall;

	getBack:
		ret;
	}
}

std::vector<std::array<int, 12 * 6>> weaponSets;
size_t weaponSetsCurrent = 0;
char Hooks::weaponSetsText[16] = {};
UINT32 timerTimeout;
LARGE_INTEGER timerFrequency, timerLast = {}, timerCurrent = {};
bool __stdcall WeaponSetsCycle()
{
	QueryPerformanceCounter(&timerCurrent);
	timerCurrent.QuadPart *= 1000;
	timerCurrent.QuadPart /= timerFrequency.QuadPart;

	if (weaponSets.size() == 0 || timerCurrent.QuadPart - timerLast.QuadPart < timerTimeout)
		return false;

	if (GetKeyState('R') & 0x8000)
	{
		if (bWeaponModKeyPressed)
			return false;

		timerLast.QuadPart = timerCurrent.QuadPart;
		if (++weaponSetsCurrent >= weaponSets.size())
			weaponSetsCurrent = 0;
		snprintf(Hooks::weaponSetsText, 16, "(Set %u)", weaponSetsCurrent);
		copy(weaponSets[weaponSetsCurrent].begin(), weaponSets[weaponSetsCurrent].end(), GetBasePtr<int>(0xA7808));
		bWeaponModKeyPressed = true;
	}
	else
		bWeaponModKeyPressed = false;
	return bWeaponModKeyPressed;
}

void __declspec(naked) HWeaponSets()
{
	__asm
	{
		pushad;
		sub		esp, 16;
		movdqu	xmmword ptr[esp], xmm1;
		sub		esp, 16;
		movdqu	xmmword ptr[esp], xmm2;
		sub		esp, 16;
		movdqu	xmmword ptr[esp], xmm3;
		sub		esp, 16;
		movdqu	xmmword ptr[esp], xmm4;
		sub		esp, 16;
		movdqu	xmmword ptr[esp], xmm5;
		sub		esp, 16;
		movdqu	xmmword ptr[esp], xmm6;
		sub		esp, 16;
		movdqu	xmmword ptr[esp], xmm7;

		mov		al, bRefreshSkillFileLinksStatus;
		test	al, al;
		jnz		refreshskillfilelinks;

		call	WeaponSetsCycle;
		test	al, al;
		jz		originalcode;
		call	HEquipSkills;

	refreshskillfilelinks:
		call	HRefreshSkillFileLinks;

	originalcode:
		movdqu	xmm7, xmmword ptr[esp];
		add		esp, 16;
		movdqu	xmm6, xmmword ptr[esp];
		add		esp, 16;
		movdqu	xmm5, xmmword ptr[esp];
		add		esp, 16;
		movdqu	xmm4, xmmword ptr[esp];
		add		esp, 16;
		movdqu	xmm3, xmmword ptr[esp];
		add		esp, 16;
		movdqu	xmm2, xmmword ptr[esp];
		add		esp, 16;
		movdqu	xmm1, xmmword ptr[esp];
		add		esp, 16;
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

void weaponSetsSave()
{
	for (auto setId : config.getSectionInts("weaponSets"))
		config.removeKey("weaponSets", std::to_string(setId).c_str());
	for (size_t setId = 0; setId < weaponSets.size(); setId++)
	{
		std::vector<int> list;
		for (size_t i = 0; i < weaponSets[setId].size(); i += 6)
		{
			list.push_back(weaponSets[setId][i + 0]);
			list.push_back(weaponSets[setId][i + 1]);
			list.push_back(weaponSets[setId][i + 2]);
			if (i / 6 == 4 || i / 6 == 5)
			{
				list.push_back(weaponSets[setId][i + 3]);
				list.push_back(weaponSets[setId][i + 4]);
				list.push_back(weaponSets[setId][i + 5]);
			}
		}
		config.setInts("weaponSets", std::to_string(setId).c_str(), list);
	}
}

bool renderWeaponSkill(int index, int weaponId, int skillCount, const char *label, const std::vector<std::pair<int, LPCSTR>> &items)
{
	bool changed = false;
	if (ImGui::TreeNode(label))
	{
		for (int i = 0; i < skillCount; i++)
			changed |= ImGui::ComboEnum<UINT32>(("##" + std::to_string(i)).c_str(), &weaponSets[index][weaponId * 6 + i], items);
		ImGui::TreePop();
	}
	return changed;
}

bool renderWeaponSet(int index)
{
	ImGui::PushID(index);
	bool treeOpened = ImGui::TreeNode(string("Set ").append(std::to_string(index)).c_str());
	ImGui::SameLine(100.0f);
	if (ImGui::SmallButton("Remove"))
	{
		weaponSets.erase(weaponSets.begin() + index);
		weaponSetsSave();
		if (weaponSets.size() == 0)
			Hooks::weaponSetsText[0] = '\0';
	}

	bool changed = false;
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
		if (ImGui::Checkbox("Enabled", &weaponSetsEnabled))
		{
			config.setBool("weaponSets", "enabled", weaponSetsEnabled);
			Hooks::SwitchHook("WeaponSets", pWeaponSetsP, weaponSetsEnabled);
			Hooks::SwitchHook("WeaponSets", pWeaponSetsS, weaponSetsEnabled);
		}

		if (ImGui::InputScalar<UINT32>("Timeout", &timerTimeout, 500, INT_MAX, 100))
			config.setUInt("weaponSets", "timeout", timerTimeout);

		if (ImGui::Button("Add set"))
		{
			weaponSets.emplace_back();
			std::copy(GetBasePtr<int>(0xA7808), GetBasePtr<int>(0xA7808 + 12 * 6 * sizeof(int)), weaponSets.back().data());
			weaponSetsSave();
		}

		for (size_t i = 0; i < weaponSets.size(); i++)
			if (renderWeaponSet(i))
				weaponSetsSave();
	}
}

void Hooks::WeaponSets()
{
	weaponSetsEnabled = config.getBool("weaponSets", "enabled", false);
	timerTimeout = config.getUInt("weaponSets", "timeout", 2000);
	for (auto setId : config.getSectionInts("weaponSets"))
	{
		auto list = config.getInts("weaponSets", std::to_string(setId).c_str());
		if (list.size() != 12 * 3 + 6)
			continue;
		weaponSets.emplace_back();

		for (size_t i = 0, j = 0; i < weaponSets.back().size(); i += 6)
		{
			weaponSets.back()[i + 0] = list[j++];
			weaponSets.back()[i + 1] = list[j++];
			weaponSets.back()[i + 2] = list[j++];
			if (i / 6 == 4 || i / 6 == 5)
			{
				weaponSets.back()[i + 3] = list[j++];
				weaponSets.back()[i + 4] = list[j++];
				weaponSets.back()[i + 5] = list[j++];
			}
		}
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
	BYTE sigS[] = { 0x03, 0x4C, 0x24, 0x28, 0x80, 0x7C, 0x24, 0x44, 0x01 };
	if (!FindSignature("WeaponSets", sigP, &pWeaponSetsP) ||
		!FindSignature("WeaponSets", sigS, &pWeaponSetsS))
		return;
	CreateHook("WeaponSets", pWeaponSetsP, &HWeaponSetsP, &oWeaponSetsP, weaponSetsEnabled);
	CreateHook("WeaponSets", pWeaponSetsS, &HWeaponSetsS, &oWeaponSetsS, weaponSetsEnabled);

	QueryPerformanceFrequency(&timerFrequency);
	InGameUIAdd(renderWeaponSetsUI);
}