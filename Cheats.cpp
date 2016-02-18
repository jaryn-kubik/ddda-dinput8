// taken from CheatEngine table by Cielos
// http://forum.cheatengine.org/viewtopic.php?p=5641841#5641841

#include "stdafx.h"
#include "Cheats.h"

bool thirdSkillLevels1[512] = { false };
bool thirdSkillLevels2[512] = { false };
bool thirdSkillLevels3[512] = { false };
bool thirdSkillLevels4[512] = { false };
void thirdSkillLevelsInit(bool *skillArray, std::vector<int> list)
{
	std::fill_n(skillArray, 512, false);
	if (list.empty())
		return;

	if (find(list.begin(), list.end(), -1) != list.end())
	{
		list =
		{
			40, 42, 46, 47, 52, 54, 57, 58,
			102, 104, 106, 109,
			150, 152, 155, 159, 161, 164, 165, 167,
			210, 212, 214, 215, 220, 222, 223, 224, 225, 226, 227, 228, 229, 230, 236,
			270, 274, 278,
			310, 311, 312, 313, 314, 315, 316, 317, 318, 319, 320,
			352, 353, 356, 359, 361, 363,
			402, 403, 407
		};
	}

	for (size_t i = 0; i < list.size(); i++)
		skillArray[list[i] & 0x1FF] = true;
}

void reloadSkills()
{
	thirdSkillLevelsInit(thirdSkillLevels1, config.getList("cheats", "thirdSkillLevelPlayer"));
	thirdSkillLevelsInit(thirdSkillLevels2, config.getList("cheats", "thirdSkillLevelPawnMain"));
	thirdSkillLevelsInit(thirdSkillLevels3, config.getList("cheats", "thirdSkillLevelPawn1"));
	thirdSkillLevelsInit(thirdSkillLevels4, config.getList("cheats", "thirdSkillLevelPawn2"));
}

int __stdcall GetSkillTier(UINT16 skill, DWORD address)
{
	address -= (DWORD)GetBasePtr(0);
	if (address == 0xA7DFC)//player
		return thirdSkillLevels1[skill & 0x1FF];
	if (address == 0xA85EC)//main pawn
		return thirdSkillLevels2[skill & 0x1FF];
	if (address == 0xA9C4C)//pawn 1
		return thirdSkillLevels3[skill & 0x1FF];
	if (address == 0xAB2AC)//pawn 2
		return thirdSkillLevels4[skill & 0x1FF];
	return 0;
}

bool skillLevel;
LPBYTE pSkillLevel, oSkillLevel;
void __declspec(naked) HSkillLevel()
{
	__asm
	{
		pushad;
		push	edi;
		push	esi;
		call	GetSkillTier;
		cmp		eax, 0;
		popad;

		jne		getBack;
		test	dword ptr[edi + ecx * 4 + 0x74], eax;
	getBack:
		setnz	dl;
		jmp		oSkillLevel;
	}
}

int runType;
LPBYTE pRunType, oRunType;
void __declspec(naked) HRunType()
{
	__asm
	{
		cmp		runType, 0;
		je		animOnly;
		cmp		runType, 2;
		je		stamOnly;
		mov		eax, 0x20;
		jmp		oRunType;

	animOnly:
		and		eax, 0x20;
		jmp		oRunType;

	stamOnly:
		or eax, 0x20;
		jmp		oRunType;
	}
}

float mWeight;
LPBYTE pWeight, oWeight;
void __declspec(naked) HWeight()
{
	__asm	movss	xmm0, mWeight;
	__asm	mulss	xmm5, xmm0;
	__asm	jmp		oWeight;
}

bool realTime;
float mTimeInterval;
UINT rTimeInterval;
LPBYTE pTimeInterval, oTimeInterval;
void __declspec(naked) HTimeInterval()
{
	__asm
	{
		cmp		dword ptr[esp + 8], 0;
		je		getBack;
		cmp		realTime, 0;
		je		notRealTime;
		mov		eax, 0x11;
		mov		dword ptr[esp + 8], eax;
		jmp		getBack;

	notRealTime:
		fild	dword ptr[esp + 8];
		fmul	mTimeInterval;
		fistp	rTimeInterval;
		mov		eax, rTimeInterval;
		mov		dword ptr[esp + 8], eax;

	getBack:
		jmp		oTimeInterval;
	}
}

UINT16 *pAffinityLast;
LPVOID oAffinity;
enum AffinityMod { Disabled = -1, NoNegative, AllPositive, NoChange, InstantFriend = 850, InstantMax = 900 } iAffinityMod;
void __declspec(naked) HAffinity()
{
	__asm
	{
		mov		dword ptr[pAffinityLast], esi;
		cmp		iAffinityMod, Disabled;
		jle		getBack;
		cmp		iAffinityMod, AllPositive;
		jle		notInstant;
		cmp		iAffinityMod, NoChange;
		je		noNegative;
		movzx	eax, word ptr[esi + 0x8B8];
		cmp		eax, [iAffinityMod];
		jae		notInstant;
		sub		eax, [iAffinityMod];
		neg		eax;
		mov		ebp, eax;
		jmp		getBack;

	notInstant:
		test	ebp, ebp;
		jns		getBack;
		cmp		dword ptr[iAffinityMod], NoNegative;
		je		noNegative;
		neg		ebp;
		jmp		getBack;

	noNegative:
		xor		ebp, ebp;

	getBack:
		jmp		oAffinity;
	}
}

const float floatZeroConstant = 0.0f;
float augmentModsValues[0x80];
bool augmentMods;
LPBYTE pAugmentMods, oAugmentMods;
void __declspec(naked) HAugmentMods()
{
	__asm
	{
		mov		eax, pBase;
		mov		eax, [eax];

		add		eax, 0xA76D0;	//player
		cmp		eax, ecx;
		je		isParty;

		add		eax, 0x7F0;		//main Pawn
		cmp		eax, ecx;
		je		isParty;

		add		eax, 0x1660;	//pawn 2
		cmp		eax, ecx;
		je		isParty;

		add		eax, 0x1660;	//pawn 3
		cmp		eax, ecx;
		je		isParty;

		jmp		oAugmentMods;

	isParty:
		lea		eax, [augmentModsValues + edx * 4];
		movss	xmm0, [eax];
		comiss	xmm0, floatZeroConstant;
		jae		modValue;
		jmp		oAugmentMods;

	modValue:
		movss	dword ptr[esi], xmm0;
		mov		eax, 1;
		retn	4;
	}
}

void renderCheatsUI()
{
	if (ImGui::CollapsingHeader("Cheats"))
	{
		std::pair<int, LPCSTR> runTypeMapEV[] = { { -1, "Disabled" }, { 0, "Town Animation" }, { 1, "Town Animation + Stamina" }, { 2, "Stamina" } };
		if (ImGui::ComboEnum<int>("Outside run type", &runType, runTypeMapEV, 4))
		{
			config.setInt("cheats", "runType", runType);
			Hooks::SwitchHook("Cheat (runType)", pRunType, runType >= 0);
		}

		if (ImGui::DragFloat("Weight multiplicator", &mWeight, 0.01f, -1.0f, 1.0f))
		{
			config.setFloat("cheats", "weightMultiplicator", mWeight);
			Hooks::SwitchHook("Cheat (weight)", pWeight, mWeight >= 0);
		}

		if (ImGui::DragFloat("Time speed", &mTimeInterval, 0.1f, -1.0f, 60.0f))
		{
			config.setFloat("cheats", "timeInterval", mTimeInterval);
			realTime = mTimeInterval == 0;
			Hooks::SwitchHook("Cheat (timeInterval)", pTimeInterval, mTimeInterval >= 0);
		}

		if (ImGui::Checkbox("3rd level skills", &skillLevel))
		{
			config.setBool("cheats", "thirdSkillLevel", skillLevel);
			Hooks::SwitchHook("Cheat (thirdSkillLevel)", pSkillLevel, skillLevel);
		}
		ImGui::SameLine();
		if (ImGui::Button("  (reload skills)"))
			reloadSkills();

		ImGui::Separator();
		if (ImGui::TreeNode("Augment mods"))
		{
			if (ImGui::Checkbox("Enabled", &skillLevel))
			{
				config.setBool("cheats", "augmentMods", augmentMods);
				Hooks::SwitchHook("Cheat (augmentMods)", pAugmentMods, augmentMods);
			}

			if (ImGui::DragFloat("Articulacy", augmentModsValues + 0x51, 1.0f, -1.0f, 100.0f))
				config.setFloat("cheats", "augmentArticulacy", augmentModsValues[0x51]);
			if (ImGui::DragFloat("Radiance", augmentModsValues + 0x4B, 0.5f, -1.0f, FLT_MAX))
				config.setFloat("cheats", "augmentRadiance", augmentModsValues[0x4B]);
			if (ImGui::DragFloat("Sinew", augmentModsValues + 0x01, 1.0f, -1.0f, FLT_MAX))
				config.setFloat("cheats", "augmentSinew", augmentModsValues[0x01]);
			if (ImGui::DragFloat("Perpetuation", augmentModsValues + 0x17, 1.0f, -1.0f, FLT_MAX))
				config.setFloat("cheats", "augmentPerpetuation", augmentModsValues[0x17]);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Affinity mods"))
		{
			std::pair<int, LPCSTR> affinityModEV[] =
			{
				{ Disabled, "Disabled" },{ NoNegative, "No negative changes" },{ AllPositive, "All changes are positive" },
				{ NoChange, "No changes at all" },{ InstantFriend, "Instant friend (850)" },{ InstantMax, "Instant max (900)" }
			};

			if (ImGui::ComboEnum<int>("Mode", &iAffinityMod, affinityModEV, 6))
				config.setInt("cheats", "affinityMod", iAffinityMod);

			ImGui::InputScalar<UINT16>("Last changed", pAffinityLast + 0x8B8 / 2, 0, 1000);
			ImGui::InputScalar<UINT16>("Attitude", pAffinityLast + 0x8BA / 2, 0, UINT16_MAX);
		}
	}
}

void Hooks::Cheats()
{
	BYTE sigRun[] = { 0x8B, 0x42, 0x40,			//mov	eax, [edx+40h]
					0x53,						//push	ebx
					0x8B, 0x5C, 0x24, 0x08 };	//mov	ebx, [esp+4+arg_0]
	if (FindSignature("Cheat (runType)", sigRun, &pRunType))
	{
		runType = config.getInt("cheats", "runType", false);
		if (runType > 2 || runType < -1)
			runType = -1;
		CreateHook("Cheat (runType)", pRunType += 3, &HRunType, &oRunType, runType >= 0);
	}

	BYTE sigWeight[] = { 0xF3, 0x0F, 0x58, 0xAB, 0x4C, 0x02, 0x00, 0x00,	//addss		xmm5, dword ptr [ebx+24Ch]
							0x45 };											//inc		ebp
	if (FindSignature("Cheat (weight)", sigWeight, &pWeight))
	{
		mWeight = config.getFloat("cheats", "weightMultiplicator", -1);
		CreateHook("Cheat (weight)", pWeight, &HWeight, &oWeight, mWeight >= 0);
	}

	BYTE sigTime[] = { 0x8B, 0x44, 0x24, 0x08, 0x01, 0x86, 0x68, 0x87, 0x0B, 0x00 };
	if (FindSignature("Cheat (timeInterval)", sigTime, &pTimeInterval))
	{
		mTimeInterval = config.getFloat("cheats", "timeInterval", -1);
		realTime = mTimeInterval == 0;
		CreateHook("Cheat (timeInterval)", pTimeInterval, &HTimeInterval, &oTimeInterval, mTimeInterval >= 0);
	}

	BYTE sigSkill[] = { 0x85, 0x44, 0x8F, 0x74, 0x0F, 0x95, 0xC2 };
	if (FindSignature("Cheat (thirdSkillLevel)", sigSkill, &pSkillLevel))
	{
		skillLevel = config.getBool("cheats", "thirdSkillLevel", false);
		CreateHook("Cheat (thirdSkillLevel)", pSkillLevel, &HSkillLevel, &oSkillLevel, skillLevel);
		oSkillLevel += 7;
		reloadSkills();
	}

	BYTE *pOffset;
	BYTE sigAffinity[] = { 0x0F, 0xB7, 0x86, 0xB8, 0x08, 0x00, 0x00, 0x8B, 0xD8, 0x03, 0xC5 };
	if (FindSignature("Affinity", sigAffinity, &pOffset))
	{
		iAffinityMod = (AffinityMod)config.getInt("cheats", "affinityMod", Disabled);
		CreateHook("Affinity", pOffset, &HAffinity, &oAffinity);
	}

	BYTE sigSpell[] = { 0x33, 0xC0, 0x81, 0xC1, 0x58, 0x02, 0x00, 0x00, 0x39, 0x11, 0x74, 0x34 };
	if (FindSignature("Cheat (augmentMods)", sigSpell, &pAugmentMods))
	{
		std::fill_n(augmentModsValues, 0x80, -1.0f);
		augmentMods = config.getBool("cheats", "augmentMods", false);
		augmentModsValues[0x51] = config.getFloat("cheats", "augmentArticulacy", -1);
		augmentModsValues[0x4B] = config.getFloat("cheats", "augmentRadiance", -1);
		augmentModsValues[0x01] = config.getFloat("cheats", "augmentSinew", -1);
		augmentModsValues[0x17] = config.getFloat("cheats", "augmentPerpetuation", -1);
		CreateHook("Cheat (augmentMods)", pAugmentMods, &HAugmentMods, &oAugmentMods, augmentMods);
	}

	if (config.getBool("cheats", "shareWeaponSkills", false))
	{
		BYTE sig1[] = { 0x0F,0x84,0x97,0x00,0x00,0x00,0x8B,0xCC,0xCC,0xCC,0x8B,0xCC,0x8B };
		BYTE sig2[] = { 0x75,0x06,0xC7,0x02,0xFF,0xFF,0xFF,0xFF,0x8B,0xCC,0xCC,0x83 };
		BYTE sig3[] = { 0x75,0x18,0x8B,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0x70,0x02 };
		BYTE sig4[] = { 0x75,0x18,0x8B,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0x33,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0x74,0x02 };
		BYTE sig5[] = { 0x75,0x18,0x8B,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0x33,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0x78,0x02 };
		BYTE sig6[] = { 0x75,0x18,0x8B,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0x33,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0x7C,0x02 };
		BYTE sig7[] = { 0x75,0x18,0x8B,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0x33,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0x80,0x02 };
		BYTE sig8[] = { 0x75,0x18,0x8B,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0x33,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0x84,0x02 };

		if (FindSignature("Cheat (shareWeaponSkills1)", sig1, &pOffset))
			Set<BYTE>(pOffset, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });

		if (FindSignature("Cheat (shareWeaponSkills2)", sig2, &pOffset)) Set<BYTE>(pOffset, { 0xEB });
		if (FindSignature("Cheat (shareWeaponSkills3)", sig3, &pOffset)) Set<BYTE>(pOffset, { 0xEB });
		if (FindSignature("Cheat (shareWeaponSkills4)", sig4, &pOffset)) Set<BYTE>(pOffset, { 0xEB });
		if (FindSignature("Cheat (shareWeaponSkills5)", sig5, &pOffset)) Set<BYTE>(pOffset, { 0xEB });
		if (FindSignature("Cheat (shareWeaponSkills6)", sig6, &pOffset)) Set<BYTE>(pOffset, { 0xEB });
		if (FindSignature("Cheat (shareWeaponSkills7)", sig7, &pOffset)) Set<BYTE>(pOffset, { 0xEB });
		if (FindSignature("Cheat (shareWeaponSkills8)", sig8, &pOffset)) Set<BYTE>(pOffset, { 0xEB });
	}
	else
		logFile << "Cheat (shareWeaponSkills): disabled" << std::endl;

	if (config.getBool("cheats", "ignoreEquipVocation", false))
	{
		BYTE sig1[] = { 0x0F, 0x94, 0xC0, 0xC2, 0x0C, 0x00 };
		BYTE sig2[] = { 0x74, 0x0F, 0x3B, 0xC2, 0x74, 0x0E, 0x8B, 0x41, 0x04, 0x83 };
		BYTE sig3[] = { 0x3B, 0xC2, 0x75, 0x15, 0x8B, 0x4D, 0x10 };
		BYTE sig4[] = { 0x0F, 0x94, 0xC0, 0x84, 0xC0, 0x75, 0x09, 0x5F };
		BYTE sig5[] = { 0x0F, 0x94, 0xC0, 0x84, 0xC0, 0x74, 0x60 };
		BYTE sig6[] = { 0x0F, 0x95, 0xC1, 0x84, 0xC9, 0x74, 0x09, 0x83 };
		BYTE sig7[] = { 0x75, 0x02, 0xB3, 0x01, 0x8B, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xE8 };
		BYTE sig8[] = { 0x75, 0x17, 0x8B, 0x44, 0x24, 0x14 };

		if (FindSignature("Cheat (ignoreEquipVocation1)", sig1, &pOffset)) Set<BYTE>(pOffset, { 0x90, 0xB0, 0x01 });
		if (FindSignature("Cheat (ignoreEquipVocation2)", sig2, &pOffset)) Set<BYTE>(pOffset + 4, { 0xEB });
		if (FindSignature("Cheat (ignoreEquipVocation3)", sig3, &pOffset)) Set<BYTE>(pOffset + 2, { 0x90, 0x90 });
		if (FindSignature("Cheat (ignoreEquipVocation4)", sig4, &pOffset)) Set<BYTE>(pOffset, { 0x90, 0xB0, 0x01 });
		if (FindSignature("Cheat (ignoreEquipVocation5)", sig5, &pOffset)) Set<BYTE>(pOffset, { 0x90, 0xB0, 0x01 });
		if (FindSignature("Cheat (ignoreEquipVocation6)", sig6, &pOffset)) Set<BYTE>(pOffset, { 0x90, 0x30, 0xC9 });
		if (FindSignature("Cheat (ignoreEquipVocation7)", sig7, &pOffset)) Set<BYTE>(pOffset, { 0x90, 0x90 });
		if (FindSignature("Cheat (ignoreEquipVocation8)", sig8, &pOffset)) Set<BYTE>(pOffset, { 0x90, 0x90 });
	}
	else
		logFile << "Cheat (ignoreEquipVocation): disabled" << std::endl;

	if (config.getBool("cheats", "ignoreSkillVocation", false))
	{
		BYTE sig1[] = { 0x74, 0x2F, 0x8B, 0x47, 0x10 };
		BYTE sig2[] = { 0x74, 0x74, 0x8B, 0x44, 0x24, 0x1C };
		BYTE sig3[] = { 0x74, 0x24, 0x83, 0xBD, 0xCC, 0xCC, 0xCC, 0xCC, 0x05 };
		BYTE sig4[] = { 0x8B, 0x4A, 0x10, 0x49, 0x3B, 0xC1 };

		if (FindSignature("Cheat (ignoreSkillVocation1)", sig1, &pOffset)) Set<BYTE>(pOffset, { 0x90, 0x90 });
		if (FindSignature("Cheat (ignoreSkillVocation2)", sig2, &pOffset)) Set<BYTE>(pOffset, { 0x90, 0x90 });
		if (FindSignature("Cheat (ignoreSkillVocation3)", sig3, &pOffset)) Set<BYTE>(pOffset, { 0x90, 0x90 });
		if (FindSignature("Cheat (ignoreSkillVocation4)", sig4, &pOffset)) Set<BYTE>(pOffset, { 0x90, 0x90, 0x8B, 0xC1 });
	}
	else
		logFile << "Cheat (ignoreSkillVocation): disabled" << std::endl;

	InGameUIAdd(renderCheatsUI);
}