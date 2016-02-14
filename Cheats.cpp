// taken from CheatEngine table by Cielos
// http://forum.cheatengine.org/viewtopic.php?p=5641841#5641841

#include "stdafx.h"
#include "Cheats.h"

bool thirdSkillLevels[0x200] = { false };
void thirdSkillLevelsInit()
{
	int temp[] =
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

	for (int i = 0; i < sizeof(temp) / sizeof(int); i++)
		thirdSkillLevels[temp[i]] = true;
}

int __stdcall GetSkillTier(UINT16 skill, DWORD address)
{
	if (thirdSkillLevels[skill & 0x1FF] && pBase && *pBase)
	{
		address -= (DWORD)*pBase;
		return address == 0xA7DFC ||	//player
			address == 0xA85EC ||		//main pawn
			address == 0xA9C4C ||		//pawn 1
			address == 0xAB2AC;			//pawn 2
	}
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

const float floatZeroConstant = 0.0f;
float augmentModsValues[0x80];
bool augmentMods;
LPBYTE pAugmentMods, oAugmentMods;
void __declspec(naked) HAugmentMods()
{
	__asm
	{
		//cmp		edx, 0x51; //0x4B;// 0x51;
		lea		eax, [augmentModsValues + edx * 4];
		movss	xmm0, [eax];
		comiss	xmm0, floatZeroConstant;
		jae		modValue;
		jmp		oAugmentMods;

	modValue:
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
		movss	dword ptr[esi], xmm0;
		mov		eax, 1;
		retn	4;
	}
}

template<typename T>
T switchCheats(LPCVOID value, T &var, LPCSTR msg, LPVOID pTarget)
{
	bool prevState = var >= 0;
	var = *(T*)value;
	bool newState = var >= 0;
	if (prevState != newState)
		Hooks::SwitchHook(msg, pTarget, newState);
	return var;
}

void getCheats(void *value, void *clientData)
{
	if (clientData == &skillLevel || clientData == &augmentMods)
		*(bool*)value = *(bool*)clientData;
	else
		*(UINT32*)value = *(UINT32*)clientData;
}

void setCheats(const void *value, void *clientData)
{
	if (clientData == &runType)
		config.setInt("cheats", "runType", switchCheats(value, runType, "Cheat (runType)", pRunType));
	else if (clientData == &mWeight)
		config.setFloat("cheats", "weightMultiplicator", switchCheats(value, mWeight, "Cheat (weight)", pWeight));
	else if (clientData == &mTimeInterval)
	{
		config.setFloat("cheats", "timeInterval", switchCheats(value, mTimeInterval, "Cheat (timeInterval)", pTimeInterval));
		realTime = mTimeInterval == 0;
	}
	else if (clientData == &skillLevel)
	{
		config.setBool("cheats", "thirdSkillLevel", skillLevel = *(bool*)value);
		Hooks::SwitchHook("Cheat (thirdSkillLevel)", pSkillLevel, skillLevel);
	}
	else if (clientData == &augmentMods)
	{
		config.setBool("cheats", "augmentMods", augmentMods = *(bool*)value);
		Hooks::SwitchHook("Cheat (augmentMods)", pAugmentMods, augmentMods);
	}
	else if (clientData == augmentModsValues + 0x51)
		config.setFloat("cheats", "augmentArticulacy", augmentModsValues[0x51] = *(float*)value);
	else if (clientData == augmentModsValues + 0x4B)
		config.setFloat("cheats", "augmentRadiance", augmentModsValues[0x4B] = *(float*)value);
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
		TweakBarAdd([](TwBar *b)
		{
			TwEnumVal runTypeMapEV[] = { { -1, "disabled" }, { 0, "town animation" }, { 1, "town animation + stamina" }, { 2, "stamina" } };
			TwType runTypeEnum = TwDefineEnum("RunTypeEnum", runTypeMapEV, 4);
			TwAddVarCB(b, "miscRun", runTypeEnum, setCheats, getCheats, &runType, "group=Main label='Outside run type'");
		});
	}

	BYTE sigWeight[] = { 0xF3, 0x0F, 0x58, 0xAB, 0x4C, 0x02, 0x00, 0x00,	//addss		xmm5, dword ptr [ebx+24Ch]
							0x45 };											//inc		ebp
	if (FindSignature("Cheat (weight)", sigWeight, &pWeight))
	{
		mWeight = config.getFloat("cheats", "weightMultiplicator", -1);
		CreateHook("Cheat (weight)", pWeight, &HWeight, &oWeight, mWeight >= 0);
		TweakBarAddCB("miscWeight", TW_TYPE_FLOAT, setCheats, getCheats, &mWeight, "group=Main label='Weight multiplicator' step=0.01 min=-1.0 max=1.0 precision=4");
	}

	BYTE sigTime[] = { 0x8B, 0x44, 0x24, 0x08, 0x01, 0x86, 0x68, 0x87, 0x0B, 0x00 };
	if (FindSignature("Cheat (timeInterval)", sigTime, &pTimeInterval))
	{
		mTimeInterval = config.getFloat("cheats", "timeInterval", -1);
		realTime = mTimeInterval == 0;
		CreateHook("Cheat (timeInterval)", pTimeInterval, &HTimeInterval, &oTimeInterval, mTimeInterval >= 0);
		TweakBarAddCB("miscTime", TW_TYPE_FLOAT, setCheats, getCheats, &mTimeInterval, "group=Main label='Time speed' step=0.1 min=-1.0");
	}

	BYTE sigSkill[] = { 0x85, 0x44, 0x8F, 0x74, 0x0F, 0x95, 0xC2 };
	if (FindSignature("Cheat (thirdSkillLevel)", sigSkill, &pSkillLevel))
	{
		skillLevel = config.getBool("cheats", "thirdSkillLevel", false);
		CreateHook("Cheat (thirdSkillLevel)", pSkillLevel, &HSkillLevel, &oSkillLevel, skillLevel);
		TweakBarAddCB("miscSkills", TW_TYPE_BOOLCPP, setCheats, getCheats, &skillLevel, "group=Main label='3rd level skills'");

		oSkillLevel += 7;
		thirdSkillLevelsInit();
	}

	BYTE sigSpell[] = { 0x33, 0xC0, 0x81, 0xC1, 0x58, 0x02, 0x00, 0x00, 0x39, 0x11, 0x74, 0x34 };
	if (FindSignature("Cheat (augmentMods)", sigSpell, &pAugmentMods))
	{
		std::fill_n(augmentModsValues, 0x80, -1.0f);
		augmentMods = config.getBool("cheats", "augmentMods", false);
		augmentModsValues[0x51] = config.getFloat("cheats", "augmentArticulacy", -1);
		augmentModsValues[0x4B] = config.getFloat("cheats", "augmentRadiance", -1);

		CreateHook("Cheat (augmentMods)", pAugmentMods, &HAugmentMods, &oAugmentMods, augmentMods);
		TweakBarAddCB("augmentMods", TW_TYPE_BOOLCPP, setCheats, getCheats, &augmentMods, "group='Augment mods' label=Enabled");
		TweakBarAddCB("augmentArticulacy", TW_TYPE_FLOAT, setCheats, getCheats, augmentModsValues + 0x51, "group='Augment mods' label='Spell casting time' step=1.0 min=-1.0 precision=1 max=100.0");
		TweakBarAddCB("augmentRadiance", TW_TYPE_FLOAT, setCheats, getCheats, augmentModsValues + 0x4B, "group='Augment mods' label='Lantern power' step=1.0 min=-1.0 precision=1");
		TweakBarDefine("DDDAFix/'Augment mods' group=Main opened=false");
	}

	BYTE *pOffset;
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
}