// taken from CheatEngine table by Cielos
// http://forum.cheatengine.org/viewtopic.php?p=5641841#5641841

#include "stdafx.h"
#include "Cheats.h"
#include "ItemEditor.h"
#include "PlayerStats.h"

bool thirdSkillLevels[0x200][4] = {};
void thirdSkillLevelsInit(int partyId, std::vector<int> list)
{
	if (list.empty())
		return;
	if (find(list.begin(), list.end(), -1) != list.end())
		for (size_t i = 0; i < Hooks::ListItemEnchant.size() - 1; i++)
			thirdSkillLevels[Hooks::ListItemEnchant[i].first & 0x1FF][partyId] = true;
	else
		for (size_t i = 0; i < list.size(); i++)
			thirdSkillLevels[list[i] & 0x1FF][partyId] = true;
}

int __stdcall GetSkillTier(UINT16 skill, DWORD address)
{
	address -= (DWORD)GetBasePtr(0);
	if (address == 0xA7DFC)//player
		return thirdSkillLevels[skill & 0x1FF][0];
	if (address == 0xA85EC)//main pawn
		return thirdSkillLevels[skill & 0x1FF][1];
	if (address == 0xA9C4C)//pawn 1
		return thirdSkillLevels[skill & 0x1FF][2];
	if (address == 0xAB2AC)//pawn 2
		return thirdSkillLevels[skill & 0x1FF][3];
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

BYTE *pAffinityLast;
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
bool augmentModsParty[0x80][4] = {};
float augmentModsOn[0x80] = {}, augmentModsOff[0x80] = {};
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
		jne		isMainPawn;
		push	1;
		jmp		hasAugment;

	isMainPawn:
		add		eax, 0x7F0;		//main Pawn
		cmp		eax, ecx;
		jne		isPawn1;
		push	2;
		jmp		hasAugment;

	isPawn1:
		add		eax, 0x1660;	//pawn 2
		cmp		eax, ecx;
		jne		isPawn2;
		push	3;
		jmp		hasAugment;

	isPawn2:
		add		eax, 0x1660;	//pawn 3
		cmp		eax, ecx;
		jne		notParty;
		push	4;
		jmp		hasAugment;

	notParty:
		push	0;

	hasAugment:
		xor     eax, eax;
		add     ecx, 258h;

	hasAugmentLoop:
		cmp		dword ptr[ecx], edx;
		je		foundAugment;
		inc		eax;
		add		ecx, 4;
		cmp		eax, 6;
		jb		hasAugmentLoop;
		xor		al, al;
		jmp		getValue;

	foundAugment:
		mov		al, 1;

	getValue:
		pop		ecx;
		test	cl, cl;
		jz		doNotMod;

		test	al, al;
		jnz		loadValue;
		dec		ecx;
		lea		eax, [augmentModsParty + edx * 4 + ecx];
		mov		eax, [eax];

	loadValue:
		test	al, al;
		jz		loadOff;
		lea		eax, [augmentModsOn + edx * 4];
		jmp		returnValue;

	loadOff:
		lea		eax, [augmentModsOff + edx * 4];

	returnValue:
		movss	xmm0, [eax];
		comiss	xmm0, floatZeroConstant;
		jb		doNotMod;
		movss	dword ptr[esi], xmm0;
		retn	4;

	doNotMod:
		jmp		oAugmentMods;
	}
}

void augmentModsLoad()
{
	std::fill_n(augmentModsOn, 0x80, -1.0f);
	std::fill_n(augmentModsOff, 0x80, -1.0f);
	for (auto &key : config.getSection("augments"))
	{
		try
		{
			int augmentId = stoi(key);
			for (size_t i = 1; i < Hooks::ListSkillsAugments.size(); i++)
				if (Hooks::ListSkillsAugments[i].first == augmentId)
				{
					auto values = config.getFloats("augments", std::to_string(augmentId).c_str());
					augmentModsOn[augmentId] = values.at(0);
					augmentModsOff[augmentId] = values.at(1);
					break;
				}
		}
		catch (...) {}
	}
}

void renderCheatsSkillLevel(const char *label, float position, bool *check, int partyId, bool isHeader = false)
{
	ImGui::SameLine(position);
	if (isHeader)
		ImGui::Text(label);
	ImGui::SameLine(position + 50.0f);
	if (ImGui::Checkbox((string("##") + label).c_str(), check))
	{
		std::vector<int> list;
		for (size_t i = 0; i < Hooks::ListItemEnchant.size() - 1; i++)
		{
			int skillId = Hooks::ListItemEnchant[i].first;
			if (isHeader)
				thirdSkillLevels[skillId][partyId] = *check;
			if (thirdSkillLevels[skillId][partyId])
				list.push_back(skillId);
		}
		config.setInts("cheats", (string("thirdSkillLevel") + label).c_str(), list);
	}
}

void renderCheatsAugment(const char *label, float position, int partyId, int skillId)
{
	ImGui::SameLine(position);
	if (ImGui::Checkbox((string("##") + label).c_str(), augmentModsParty[skillId] + partyId))
	{
		std::vector<int> list;
		for (size_t i = 1; i < Hooks::ListSkillsAugments.size(); i++)
			if (augmentModsParty[Hooks::ListSkillsAugments[i].first][partyId])
				list.push_back(Hooks::ListSkillsAugments[i].first);
		config.setInts("augments", (string("augments") + label).c_str(), list);
	}
}

bool shareWeaponSkills, ignoreEquipVocation;
std::vector<std::pair<int, LPCSTR>> runTypeMapEV = { { -1, "Disabled" },{ 0, "Town Animation" },{ 1, "Town Animation + Stamina" },{ 2, "Stamina" } };
void renderCheatsUI()
{
	static bool setSkillsOpened = false, setAugmentsOpened = false;
	if (setSkillsOpened)
	{
		ImGui::Begin("Set 3rd level skills", &setSkillsOpened, ImVec2(525, 400));
		static bool selectAll1 = false, selectAll2 = false, selectAll3 = false, selectAll4 = false;
		renderCheatsSkillLevel("Player", 200.0f + 75.0f * 0, &selectAll1, 0, true);
		renderCheatsSkillLevel("Pawn", 200.0f + 75.0f * 1, &selectAll2, 1, true);
		renderCheatsSkillLevel("Pawn1", 200.0f + 75.0f * 2, &selectAll3, 2, true);
		renderCheatsSkillLevel("Pawn2", 200.0f + 75.0f * 3, &selectAll4, 3, true);

		for (size_t i = 0; i < Hooks::ListItemEnchant.size() - 1; i++)
		{
			ImGui::PushID(i);
			ImGui::Text(Hooks::ListItemEnchant[i].second);
			int skillId = Hooks::ListItemEnchant[i].first;
			renderCheatsSkillLevel("Player", 200.0f + 75.0f * 0, thirdSkillLevels[skillId] + 0, 0);
			renderCheatsSkillLevel("Pawn", 200.0f + 75.0f * 1, thirdSkillLevels[skillId] + 1, 1);
			renderCheatsSkillLevel("Pawn1", 200.0f + 75.0f * 2, thirdSkillLevels[skillId] + 2, 2);
			renderCheatsSkillLevel("Pawn2", 200.0f + 75.0f * 3, thirdSkillLevels[skillId] + 3, 3);
			ImGui::PopID();
		}
		ImGui::End();
	}

	if (setAugmentsOpened)
	{
		ImGui::Begin("Set augment mods", &setAugmentsOpened, ImVec2(625, 400));
		ImGui::TextUnformatted("On", 150.0f + 50.0f * 0);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("(when the augment is equipped)");
		ImGui::TextUnformatted("Off", 150.0f + 50.0f * 2.5f);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("(when the augment is not equipped)");
		ImGui::TextUnformatted("Player", 150.0f + 50.0f * 5);
		ImGui::TextUnformatted("Pawn", 150.0f + 50.0f * 6);
		ImGui::TextUnformatted("Pawn1", 150.0f + 50.0f * 7);
		ImGui::TextUnformatted("Pawn2", 150.0f + 50.0f * 8);

		for (size_t i = 1; i < Hooks::ListSkillsAugments.size(); i++)
		{
			int skillId = Hooks::ListSkillsAugments[i].first;
			if (skillId <= 100 && skillId % 10 == 0)
				ImGui::Separator();
			ImGui::PushID(i);
			ImGui::TextUnformatted(Hooks::ListSkillsAugments[i].second);
			ImGui::PushItemWidth(100.0f);
			ImGui::SameLine(150.0f + 50.0f * 0);
			bool changed = ImGui::InputFloatEx("##on", augmentModsOn + skillId, 1.0f, -1.0f);
			ImGui::SameLine(150.0f + 50.0f * 2.5f);
			changed |= ImGui::InputFloatEx("##off", augmentModsOff + skillId, 1.0f, -1.0f);
			ImGui::PopItemWidth();

			if (changed)
			{
				if (augmentModsOn[skillId] < 0.0f && augmentModsOff[skillId] < 0.0f)
					config.removeKey("augments", std::to_string(skillId).c_str());
				else
					config.setFloats("augments", std::to_string(skillId).c_str(), { augmentModsOn[skillId], augmentModsOff[skillId] });
			}

			renderCheatsAugment("Player", 150.0f + 50.0f * 5, 0, skillId);
			renderCheatsAugment("Pawn", 150.0f + 50.0f * 6, 1, skillId);
			renderCheatsAugment("Pawn1", 150.0f + 50.0f * 7, 2, skillId);
			renderCheatsAugment("Pawn2", 150.0f + 50.0f * 8, 3, skillId);
			ImGui::PopID();
		}
		ImGui::End();
	}

	if (ImGui::CollapsingHeader("Cheats"))
	{
		if (ImGui::Checkbox("Share weapon skills", &shareWeaponSkills))
			config.setBool("cheats", "shareWeaponSkills", shareWeaponSkills);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("requires game restart");

		if (ImGui::Checkbox("Ignore equip vocation", &ignoreEquipVocation))
			config.setBool("cheats", "ignoreEquipVocation", ignoreEquipVocation);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("requires game restart");

		bool prevState = runType >= 0;
		if (ImGui::ComboEnum<int>("Outside run type", &runType, runTypeMapEV))
		{
			config.setInt("cheats", "runType", runType);
			if (prevState != runType >= 0)
				Hooks::SwitchHook("Cheat (runType)", pRunType, runType >= 0);
		}

		prevState = mTimeInterval >= 0;
		if (ImGui::InputFloatEx("Time speed", &mTimeInterval, 0.1f, -1.0f, 60.0f))
		{
			config.setFloat("cheats", "timeInterval", mTimeInterval);
			realTime = mTimeInterval == 0;
			if (prevState != mTimeInterval >= 0)
				Hooks::SwitchHook("Cheat (timeInterval)", pTimeInterval, mTimeInterval >= 0);
		}

		if (ImGui::Checkbox("3rd level skills", &skillLevel))
		{
			config.setBool("cheats", "thirdSkillLevel", skillLevel);
			Hooks::SwitchHook("Cheat (thirdSkillLevel)", pSkillLevel, skillLevel);
		}
		ImGui::SameLine();
		if (ImGui::Button("Set##skills"))
			setSkillsOpened = true;

		if (ImGui::Checkbox("Augment mods", &augmentMods))
		{
			config.setBool("augments", "enabled", augmentMods);
			Hooks::SwitchHook("Cheat (augmentMods)", pAugmentMods, augmentMods);
		}
		ImGui::SameLine();
		if (ImGui::Button("Set##augments"))
			setAugmentsOpened = true;

		ImGui::Separator();
		if (ImGui::TreeNode("Affinity mod"))
		{
			std::vector<std::pair<int, LPCSTR>> affinityModEV =
			{
				{ Disabled, "Disabled" },{ NoNegative, "No negative changes" },{ AllPositive, "All changes are positive" },
				{ NoChange, "No changes at all" },{ InstantFriend, "Instant friend (850)" },{ InstantMax, "Instant max (900)" }
			};

			if (ImGui::ComboEnum<int>("Mode", &iAffinityMod, affinityModEV))
				config.setInt("cheats", "affinityMod", iAffinityMod);

			if (pAffinityLast)
			{
				ImGui::InputScalar<UINT16>("Last", pAffinityLast + 0x8B8, 0, 1000);
				ImGui::InputScalar<UINT16>("Attitude", pAffinityLast + 0x8BA, 0, UINT16_MAX);
			}
			ImGui::TreePop();
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
		thirdSkillLevelsInit(0, config.getInts("cheats", "thirdSkillLevelPlayer"));
		thirdSkillLevelsInit(1, config.getInts("cheats", "thirdSkillLevelPawn"));
		thirdSkillLevelsInit(2, config.getInts("cheats", "thirdSkillLevelPawn1"));
		thirdSkillLevelsInit(3, config.getInts("cheats", "thirdSkillLevelPawn2"));
	}

	BYTE *pOffset;
	BYTE sigAffinity[] = { 0x0F, 0xB7, 0x86, 0xB8, 0x08, 0x00, 0x00, 0x8B, 0xD8, 0x03, 0xC5 };
	if (FindSignature("Cheat (affinity)", sigAffinity, &pOffset))
	{
		iAffinityMod = (AffinityMod)config.getInt("cheats", "affinityMod", Disabled);
		CreateHook("Cheat (affinity)", pOffset, &HAffinity, &oAffinity);
	}

	BYTE sigAug[] = { 0x33, 0xC0, 0x81, 0xC1, 0x58, 0x02, 0x00, 0x00,  0x39, 0x11, 0x74, 0xCC, 0x40, 0x83, 0xC1, 0x04, 0x83, 0xF8, 0x06, 0x72, 0xCC, 0x32, 0xC0, 0x8B, 0x0D };
	if (FindSignature("Cheat (augmentMods)", sigAug, &pAugmentMods))
	{
		augmentMods = config.getBool("augments", "enabled", false);
		augmentModsLoad();
		CreateHook("Cheat (augmentMods)", pAugmentMods, &HAugmentMods, nullptr, augmentMods);
		oAugmentMods = pAugmentMods + sizeof sigAug - 2;

		for (auto i : config.getInts("augments", "augmentsPlayer"))
			augmentModsParty[i & 0x7F][0] = true;
		for (auto i : config.getInts("augments", "augmentsPawn"))
			augmentModsParty[i & 0x7F][1] = true;
		for (auto i : config.getInts("augments", "augmentsPawn1"))
			augmentModsParty[i & 0x7F][2] = true;
		for (auto i : config.getInts("augments", "augmentsPawn2"))
			augmentModsParty[i & 0x7F][3] = true;
	}

	if ((shareWeaponSkills = config.getBool("cheats", "shareWeaponSkills", false)))
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

	if ((ignoreEquipVocation = config.getBool("cheats", "ignoreEquipVocation", false)))
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

	InGameUIAdd(renderCheatsUI);
}