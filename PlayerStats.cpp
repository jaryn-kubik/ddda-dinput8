#include "dinput8.h"
#include "PlayerStats.h"
#include "TweakBar.h"

LPVOID pSelectedSkill, oSelectedSkill;
void __declspec(naked) HSelectedSkill()
{
	__asm	shr		eax, 5;
	__asm	mov		pSelectedSkill, eax;
	__asm	jmp		oSelectedSkill;
}

DWORD **pMainPointer;
void setStats(const void *value, void *clientData)
{
	if (pMainPointer || *pMainPointer)
		(*pMainPointer)[(DWORD)clientData / 4] = *(UINT32*)value;
}

void getStats(void *value, void *clientData)
{
	if (pMainPointer || *pMainPointer)
		*(UINT32*)value = (*pMainPointer)[(DWORD)clientData / 4];
}

void addSkill(TwBar *bar, DWORD offset, string name)
{
	string var = "skills" + name;
	string def = "group=" + name + " label='";
	TwAddVarCB(bar, (var + "P1").c_str(), TW_TYPE_INT32, setStats, getStats, (LPVOID)(offset + 4 * 0), (def + "Primary 1'").c_str());
	TwAddVarCB(bar, (var + "P2").c_str(), TW_TYPE_INT32, setStats, getStats, (LPVOID)(offset + 4 * 1), (def + "Primary 2'").c_str());
	TwAddVarCB(bar, (var + "P3").c_str(), TW_TYPE_INT32, setStats, getStats, (LPVOID)(offset + 4 * 2), (def + "Primary 3'").c_str());
	TwAddVarCB(bar, (var + "S1").c_str(), TW_TYPE_INT32, setStats, getStats, (LPVOID)(offset + 4 * 3), (def + "Secondary 1'").c_str());
	TwAddVarCB(bar, (var + "S2").c_str(), TW_TYPE_INT32, setStats, getStats, (LPVOID)(offset + 4 * 4), (def + "Secondary 2'").c_str());
	TwAddVarCB(bar, (var + "S3").c_str(), TW_TYPE_INT32, setStats, getStats, (LPVOID)(offset + 4 * 5), (def + "Secondary 3'").c_str());
	TwDefine(("DDDAFix/" + name + " group='Equipped skills' opened=false").c_str());
}

void addPlayerStats(TwBar *bar)
{
	BYTE *pOffset;
	BYTE sig1[] = { 0x8B, 0x15, 0xCC, 0xCC, 0xCC, 0xCC, 0x33, 0xDB, 0x8B, 0xF8 };
	if (!Hooks::FindSignature("PlayerStats", sig1, &pOffset))
		return;
	pMainPointer = (DWORD**)*(LPDWORD)(pOffset + 2);

	//player
	TwAddVarCB(bar, "playerLevel", TW_TYPE_UINT32, setStats, getStats, (LPVOID)0xA7DD0, "group=Player label=Level");
	TwAddVarCB(bar, "playerRC", TW_TYPE_UINT32, setStats, getStats, (LPVOID)0xA7A1C, "group=Player label=Rift Crystals");
	TwAddVarCB(bar, "playerGold", TW_TYPE_UINT32, setStats, getStats, (LPVOID)0xA7A18, "group=Player label=Gold");
	TwAddVarCB(bar, "playerDCP", TW_TYPE_UINT32, setStats, getStats, (LPVOID)0xA7A14, "group=Player label=Discipline Points");
	TwAddVarCB(bar, "playerXP", TW_TYPE_UINT32, setStats, getStats, (LPVOID)0xA7994, "group=Player label=XP");

	TwAddVarCB(bar, "playerHPC", TW_TYPE_FLOAT, setStats, getStats, (LPVOID)0xA796C, "group=PStats label='HP Current'");
	TwAddVarCB(bar, "playerHP", TW_TYPE_FLOAT, setStats, getStats, (LPVOID)0xA7970, "group=PStats label='HP Max'");
	TwAddVarCB(bar, "playerHPlus", TW_TYPE_FLOAT, setStats, getStats, (LPVOID)0xA7974, "group=PStats label='HP Max+'");
	TwAddVarCB(bar, "playerStamC", TW_TYPE_FLOAT, setStats, getStats, (LPVOID)0xA7978, "group=PStats label='Stamina Current'");
	TwAddVarCB(bar, "playerStam", TW_TYPE_FLOAT, setStats, getStats, (LPVOID)0xA797C, "group=PStats label='Stamina Max'");
	TwAddVarCB(bar, "playerStamPlus", TW_TYPE_FLOAT, setStats, getStats, (LPVOID)0xA7980, "group=PStats label='Stamina Max+'");
	TwAddVarCB(bar, "playerStr", TW_TYPE_FLOAT, setStats, getStats, (LPVOID)0xA7984, "group=PStats label=Strength");
	TwAddVarCB(bar, "playerDef", TW_TYPE_FLOAT, setStats, getStats, (LPVOID)0xA7988, "group=PStats label=Defenses");
	TwAddVarCB(bar, "playerMStr", TW_TYPE_FLOAT, setStats, getStats, (LPVOID)0xA798C, "group=PStats label=Magick");
	TwAddVarCB(bar, "playerMDef", TW_TYPE_FLOAT, setStats, getStats, (LPVOID)0xA7990, "group=PStats label='Magick Defenses'");

	TwAddVarCB(bar, "playerVocFighter", TW_TYPE_UINT32, setStats, getStats, (LPVOID)0xA79A0, "group=PVocations label=Fighter max=9");
	TwAddVarCB(bar, "playerVocStrider", TW_TYPE_UINT32, setStats, getStats, (LPVOID)0xA79A4, "group=PVocations label=Strider max=9");
	TwAddVarCB(bar, "playerVocMage", TW_TYPE_UINT32, setStats, getStats, (LPVOID)0xA79A8, "group=PVocations label=Mage max=9");
	TwAddVarCB(bar, "playerVocKnight", TW_TYPE_UINT32, setStats, getStats, (LPVOID)0xA79AC, "group=PVocations label='Mystic Knight' max=9");
	TwAddVarCB(bar, "playerVocAss", TW_TYPE_UINT32, setStats, getStats, (LPVOID)0xA79B0, "group=PVocations label=Assassin max=9");
	TwAddVarCB(bar, "playerVocArc", TW_TYPE_UINT32, setStats, getStats, (LPVOID)0xA79B4, "group=PVocations label='Magic Archer' max=9");
	TwAddVarCB(bar, "playerVocWar", TW_TYPE_UINT32, setStats, getStats, (LPVOID)0xA79B8, "group=PVocations label=Warrior max=9");
	TwAddVarCB(bar, "playerVocRang", TW_TYPE_UINT32, setStats, getStats, (LPVOID)0xA79BC, "group=PVocations label=Ranger max=9");
	TwAddVarCB(bar, "playerVocSorc", TW_TYPE_UINT32, setStats, getStats, (LPVOID)0xA79C0, "group=PVocations label=Sorcerer max=9");
	TwDefine("DDDAFix/PStats group=Player label=Stats opened=false");
	TwDefine("DDDAFix/PVocations group=Player label=Vocations opened=false");
	TwDefine("DDDAFix/Player opened=false");

	//main pawn
	TwAddVarCB(bar, "pawnLevel", TW_TYPE_UINT32, setStats, getStats, (LPVOID)0xA85C0, "group=Pawn label=Level");
	TwAddVarCB(bar, "pawnDCP", TW_TYPE_UINT32, setStats, getStats, (LPVOID)0xA8204, "group=Pawn label=Discipline Points");
	TwAddVarCB(bar, "pawnXP", TW_TYPE_UINT32, setStats, getStats, (LPVOID)0xA8184, "group=Pawn label=XP");

	TwAddVarCB(bar, "pawnHPC", TW_TYPE_FLOAT, setStats, getStats, (LPVOID)0xA815C, "group=MStats label='HP Current'");
	TwAddVarCB(bar, "pawnHP", TW_TYPE_FLOAT, setStats, getStats, (LPVOID)0xA8160, "group=MStats label='HP Max'");
	TwAddVarCB(bar, "pawnHPlus", TW_TYPE_FLOAT, setStats, getStats, (LPVOID)0xA8164, "group=MStats label='HP Max+'");
	TwAddVarCB(bar, "pawnStamC", TW_TYPE_FLOAT, setStats, getStats, (LPVOID)0xA8168, "group=MStats label='Stamina Current'");
	TwAddVarCB(bar, "pawnStam", TW_TYPE_FLOAT, setStats, getStats, (LPVOID)0xA816C, "group=MStats label='Stamina Max'");
	TwAddVarCB(bar, "pawnStamPlus", TW_TYPE_FLOAT, setStats, getStats, (LPVOID)0xA8170, "group=MStats label='Stamina Max+'");
	TwAddVarCB(bar, "pawnStr", TW_TYPE_FLOAT, setStats, getStats, (LPVOID)0xA8174, "group=MStats label=Strength");
	TwAddVarCB(bar, "pawnDef", TW_TYPE_FLOAT, setStats, getStats, (LPVOID)0xA8178, "group=MStats label=Defenses");
	TwAddVarCB(bar, "pawnMStr", TW_TYPE_FLOAT, setStats, getStats, (LPVOID)0xA817C, "group=MStats label=Magick");
	TwAddVarCB(bar, "pawnMDef", TW_TYPE_FLOAT, setStats, getStats, (LPVOID)0xA8180, "group=MStats label='Magick Defenses'");

	TwAddVarCB(bar, "pawnVocFighter", TW_TYPE_UINT32, setStats, getStats, (LPVOID)0xA8190, "group=MVocations label=Fighter max=9");
	TwAddVarCB(bar, "pawnVocStrider", TW_TYPE_UINT32, setStats, getStats, (LPVOID)0xA8194, "group=MVocations label=Strider max=9");
	TwAddVarCB(bar, "pawnVocMage", TW_TYPE_UINT32, setStats, getStats, (LPVOID)0xA8198, "group=MVocations label=Mage max=9");
	TwAddVarCB(bar, "pawnVocWar", TW_TYPE_UINT32, setStats, getStats, (LPVOID)0xA81A8, "group=MVocations label=Warrior max=9");
	TwAddVarCB(bar, "pawnVocRang", TW_TYPE_UINT32, setStats, getStats, (LPVOID)0xA81AC, "group=MVocations label=Ranger max=9");
	TwAddVarCB(bar, "pawnVocSorc", TW_TYPE_UINT32, setStats, getStats, (LPVOID)0xA81B0, "group=MVocations label=Sorcerer max=9");

	TwAddVarCB(bar, "pawnIScather", TW_TYPE_FLOAT, setStats, getStats, (LPVOID)0xA9380, "group=MInclination label=Scather");
	TwAddVarCB(bar, "pawnIMedicant", TW_TYPE_FLOAT, setStats, getStats, (LPVOID)0xA938C, "group=MInclination label=Medicant");
	TwAddVarCB(bar, "pawnIMitigator", TW_TYPE_FLOAT, setStats, getStats, (LPVOID)0xA9398, "group=MInclination label=Mitigator");
	TwAddVarCB(bar, "pawnIChallenger", TW_TYPE_FLOAT, setStats, getStats, (LPVOID)0xA93A4, "group=MInclination label=Challenger");
	TwAddVarCB(bar, "pawnIUtilitarian", TW_TYPE_FLOAT, setStats, getStats, (LPVOID)0xA93B0, "group=MInclination label=Utilitarian");
	TwAddVarCB(bar, "pawnIGuardian", TW_TYPE_FLOAT, setStats, getStats, (LPVOID)0xA93BC, "group=MInclination label=Guardian");
	TwAddVarCB(bar, "pawnINexus", TW_TYPE_FLOAT, setStats, getStats, (LPVOID)0xA93C8, "group=MInclination label=Nexus");
	TwAddVarCB(bar, "pawnIPioneer", TW_TYPE_FLOAT, setStats, getStats, (LPVOID)0xA93D4, "group=MInclination label=Pioneer");
	TwAddVarCB(bar, "pawnIAcquisitor", TW_TYPE_FLOAT, setStats, getStats, (LPVOID)0xA93E0, "group=MInclination label=Acquisitor");
	TwDefine("DDDAFix/MStats group=Pawn label=Stats opened=false");
	TwDefine("DDDAFix/MVocations group=Pawn label=Vocations opened=false");
	TwDefine("DDDAFix/MInclination group=Pawn label=Inclinations opened=false");
	TwDefine("DDDAFix/Pawn opened=false");

	//skills
	BYTE sig2[] = { 0x8B, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0x33, 0xC9, 0x33, 0xC0, 0x8B, 0xFF };
	if (!Hooks::FindSignature("EquippedSkill", sig2, &pOffset))
		return;
	Hooks::CreateHook("EquippedSkill", pOffset + 6, &HSelectedSkill, &oSelectedSkill);

	TwAddVarRO(bar, "skillsSelected", TW_TYPE_INT32, &pSelectedSkill, "group='Equipped skills' label=Selected");
	addSkill(bar, 0xA7808 + 24 * 0, "Sword");
	addSkill(bar, 0xA7808 + 24 * 1, "Mace");
	addSkill(bar, 0xA7808 + 24 * 2, "Longsword");
	addSkill(bar, 0xA7808 + 24 * 3, "Dagger");
	addSkill(bar, 0xA7808 + 24 * 4, "Staff");
	addSkill(bar, 0xA7808 + 24 * 5, "Archistaff");
	addSkill(bar, 0xA7808 + 24 * 6, "Shield");
	addSkill(bar, 0xA7808 + 24 * 7, "MagickShield");
	addSkill(bar, 0xA7808 + 24 * 8, "Bow");
	addSkill(bar, 0xA7808 + 24 * 9, "Longbow");
	addSkill(bar, 0xA7808 + 24 * 10, "MagickBow");
	TwDefine("DDDAFix/'Equipped skills' opened=false");
}

void Hooks::PlayerStats() { TweakBarAdd(addPlayerStats); }