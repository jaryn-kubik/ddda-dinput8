#include "stdafx.h"
#include "PlayerStats.h"

void setSkills(const void *value, void *clientData)
{
	if (pBase && *pBase)
		memcpy(*pBase + (DWORD)clientData / 4, value, 24);
}

void getSkills(void *value, void *clientData)
{
	if (pBase && *pBase)
		memcpy(value, *pBase + (DWORD)clientData / 4, 24);
}

void skillSummary(char *str, size_t len, const void *val, void *data) { str[0] = '\0'; }
void addSkill(TwBar *bar, DWORD offset, int count, string name, TwEnumVal *enumVal, unsigned int valCount)
{
	string var = "skills" + name;
	string def = "group=Skills label='" + name + "'";
	TwType type = TwDefineEnum((var + "Enum").c_str(), enumVal, valCount);

	TwStructMember skillStruct[] =
	{
		{ "1", type, 4 * 0, "" }, { "2", type, 4 * 1, "" }, { "3", type, 4 * 2, "" },
		{ "4", type, 4 * 3, "" }, { "5", type, 4 * 4, "" }, { "6", type, 4 * 5, "" }
	};
	TwType skillType = TwDefineStruct((var + "Struct").c_str(), skillStruct, count, 4 * 6, skillSummary, nullptr);
	TwAddVarCB(bar, var.c_str(), skillType, setSkills, getSkills, (LPVOID)offset, def.c_str());
}

TwEnumVal skillsAugments[], skillsSword[], skillsLongsword[], skillsDagger[], skillsStaves[];
TwEnumVal skillsShield[], skillsMagickShield[], skillsBow[], skillsLongbow[], skillsMagickBow[];
void addPlayerStats(TwBar *bar)
{
	using namespace Hooks;
	//player
	TwAddVarCB(bar, "playerGold", TW_TYPE_UINT32, setBase, getBase, (LPVOID)0xA7A18, "group=Stats label=Gold");
	TwAddVarCB(bar, "playerRC", TW_TYPE_UINT32, setBase, getBase, (LPVOID)0xA7A1C, "group=Stats label='Rift Crystals'");
	TwDefine("DDDAFix/Stats opened=false");

	TwAddVarCB(bar, "playerLevel", TW_TYPE_UINT32, setBase, getBase, (LPVOID)0xA7DD0, "group=Player label=Level");
	TwAddVarCB(bar, "playerDCP", TW_TYPE_UINT32, setBase, getBase, (LPVOID)0xA7A14, "group=Player label='Discipline Points'");
	TwAddVarCB(bar, "playerXP", TW_TYPE_UINT32, setBase, getBase, (LPVOID)0xA7994, "group=Player label=XP");

	TwAddVarCB(bar, "playerHPC", TW_TYPE_FLOAT, setBase, getBase, (LPVOID)0xA796C, "group=PStats label='HP Current'");
	TwAddVarCB(bar, "playerHP", TW_TYPE_FLOAT, setBase, getBase, (LPVOID)0xA7970, "group=PStats label='HP Max'");
	TwAddVarCB(bar, "playerHPlus", TW_TYPE_FLOAT, setBase, getBase, (LPVOID)0xA7974, "group=PStats label='HP Max+'");
	TwAddVarCB(bar, "playerStamC", TW_TYPE_FLOAT, setBase, getBase, (LPVOID)0xA7978, "group=PStats label='Stamina Current'");
	TwAddVarCB(bar, "playerStam", TW_TYPE_FLOAT, setBase, getBase, (LPVOID)0xA797C, "group=PStats label='Stamina Max'");
	TwAddVarCB(bar, "playerStamPlus", TW_TYPE_FLOAT, setBase, getBase, (LPVOID)0xA7980, "group=PStats label='Stamina Max+'");
	TwAddVarCB(bar, "playerStr", TW_TYPE_FLOAT, setBase, getBase, (LPVOID)0xA7984, "group=PStats label=Strength");
	TwAddVarCB(bar, "playerDef", TW_TYPE_FLOAT, setBase, getBase, (LPVOID)0xA7988, "group=PStats label=Defenses");
	TwAddVarCB(bar, "playerMStr", TW_TYPE_FLOAT, setBase, getBase, (LPVOID)0xA798C, "group=PStats label=Magick");
	TwAddVarCB(bar, "playerMDef", TW_TYPE_FLOAT, setBase, getBase, (LPVOID)0xA7990, "group=PStats label='Magick Defenses'");

	TwAddVarCB(bar, "playerVocFighter", TW_TYPE_UINT32, setBase, getBase, (LPVOID)0xA79A0, "group=PVocations label=Fighter max=9");
	TwAddVarCB(bar, "playerVocStrider", TW_TYPE_UINT32, setBase, getBase, (LPVOID)0xA79A4, "group=PVocations label=Strider max=9");
	TwAddVarCB(bar, "playerVocMage", TW_TYPE_UINT32, setBase, getBase, (LPVOID)0xA79A8, "group=PVocations label=Mage max=9");
	TwAddVarCB(bar, "playerVocKnight", TW_TYPE_UINT32, setBase, getBase, (LPVOID)0xA79AC, "group=PVocations label='Mystic Knight' max=9");
	TwAddVarCB(bar, "playerVocAss", TW_TYPE_UINT32, setBase, getBase, (LPVOID)0xA79B0, "group=PVocations label=Assassin max=9");
	TwAddVarCB(bar, "playerVocArc", TW_TYPE_UINT32, setBase, getBase, (LPVOID)0xA79B4, "group=PVocations label='Magic Archer' max=9");
	TwAddVarCB(bar, "playerVocWar", TW_TYPE_UINT32, setBase, getBase, (LPVOID)0xA79B8, "group=PVocations label=Warrior max=9");
	TwAddVarCB(bar, "playerVocRang", TW_TYPE_UINT32, setBase, getBase, (LPVOID)0xA79BC, "group=PVocations label=Ranger max=9");
	TwAddVarCB(bar, "playerVocSorc", TW_TYPE_UINT32, setBase, getBase, (LPVOID)0xA79C0, "group=PVocations label=Sorcerer max=9");
	TwDefine("DDDAFix/PStats group=Player label=Stats opened=false");
	TwDefine("DDDAFix/PVocations group=Player label=Vocations opened=false");
	TwDefine("DDDAFix/Player group=Stats opened=false");

	//main pawn
	TwAddVarCB(bar, "pawnLevel", TW_TYPE_UINT32, setBase, getBase, (LPVOID)0xA85C0, "group=Pawn label=Level");
	TwAddVarCB(bar, "pawnDCP", TW_TYPE_UINT32, setBase, getBase, (LPVOID)0xA8204, "group=Pawn label='Discipline Points'");
	TwAddVarCB(bar, "pawnXP", TW_TYPE_UINT32, setBase, getBase, (LPVOID)0xA8184, "group=Pawn label=XP");

	TwAddVarCB(bar, "pawnHPC", TW_TYPE_FLOAT, setBase, getBase, (LPVOID)0xA815C, "group=MStats label='HP Current'");
	TwAddVarCB(bar, "pawnHP", TW_TYPE_FLOAT, setBase, getBase, (LPVOID)0xA8160, "group=MStats label='HP Max'");
	TwAddVarCB(bar, "pawnHPlus", TW_TYPE_FLOAT, setBase, getBase, (LPVOID)0xA8164, "group=MStats label='HP Max+'");
	TwAddVarCB(bar, "pawnStamC", TW_TYPE_FLOAT, setBase, getBase, (LPVOID)0xA8168, "group=MStats label='Stamina Current'");
	TwAddVarCB(bar, "pawnStam", TW_TYPE_FLOAT, setBase, getBase, (LPVOID)0xA816C, "group=MStats label='Stamina Max'");
	TwAddVarCB(bar, "pawnStamPlus", TW_TYPE_FLOAT, setBase, getBase, (LPVOID)0xA8170, "group=MStats label='Stamina Max+'");
	TwAddVarCB(bar, "pawnStr", TW_TYPE_FLOAT, setBase, getBase, (LPVOID)0xA8174, "group=MStats label=Strength");
	TwAddVarCB(bar, "pawnDef", TW_TYPE_FLOAT, setBase, getBase, (LPVOID)0xA8178, "group=MStats label=Defenses");
	TwAddVarCB(bar, "pawnMStr", TW_TYPE_FLOAT, setBase, getBase, (LPVOID)0xA817C, "group=MStats label=Magick");
	TwAddVarCB(bar, "pawnMDef", TW_TYPE_FLOAT, setBase, getBase, (LPVOID)0xA8180, "group=MStats label='Magick Defenses'");

	TwAddVarCB(bar, "pawnVocFighter", TW_TYPE_UINT32, setBase, getBase, (LPVOID)0xA8190, "group=MVocations label=Fighter max=9");
	TwAddVarCB(bar, "pawnVocStrider", TW_TYPE_UINT32, setBase, getBase, (LPVOID)0xA8194, "group=MVocations label=Strider max=9");
	TwAddVarCB(bar, "pawnVocMage", TW_TYPE_UINT32, setBase, getBase, (LPVOID)0xA8198, "group=MVocations label=Mage max=9");
	TwAddVarCB(bar, "pawnVocWar", TW_TYPE_UINT32, setBase, getBase, (LPVOID)0xA81A8, "group=MVocations label=Warrior max=9");
	TwAddVarCB(bar, "pawnVocRang", TW_TYPE_UINT32, setBase, getBase, (LPVOID)0xA81AC, "group=MVocations label=Ranger max=9");
	TwAddVarCB(bar, "pawnVocSorc", TW_TYPE_UINT32, setBase, getBase, (LPVOID)0xA81B0, "group=MVocations label=Sorcerer max=9");

	TwAddVarCB(bar, "pawnIScather", TW_TYPE_FLOAT, setBase, getBase, (LPVOID)0xA9380, "group=MInclination label=Scather");
	TwAddVarCB(bar, "pawnIMedicant", TW_TYPE_FLOAT, setBase, getBase, (LPVOID)0xA938C, "group=MInclination label=Medicant");
	TwAddVarCB(bar, "pawnIMitigator", TW_TYPE_FLOAT, setBase, getBase, (LPVOID)0xA9398, "group=MInclination label=Mitigator");
	TwAddVarCB(bar, "pawnIChallenger", TW_TYPE_FLOAT, setBase, getBase, (LPVOID)0xA93A4, "group=MInclination label=Challenger");
	TwAddVarCB(bar, "pawnIUtilitarian", TW_TYPE_FLOAT, setBase, getBase, (LPVOID)0xA93B0, "group=MInclination label=Utilitarian");
	TwAddVarCB(bar, "pawnIGuardian", TW_TYPE_FLOAT, setBase, getBase, (LPVOID)0xA93BC, "group=MInclination label=Guardian");
	TwAddVarCB(bar, "pawnINexus", TW_TYPE_FLOAT, setBase, getBase, (LPVOID)0xA93C8, "group=MInclination label=Nexus");
	TwAddVarCB(bar, "pawnIPioneer", TW_TYPE_FLOAT, setBase, getBase, (LPVOID)0xA93D4, "group=MInclination label=Pioneer");
	TwAddVarCB(bar, "pawnIAcquisitor", TW_TYPE_FLOAT, setBase, getBase, (LPVOID)0xA93E0, "group=MInclination label=Acquisitor");
	TwDefine("DDDAFix/MStats group=Pawn label=Stats opened=false");
	TwDefine("DDDAFix/MVocations group=Pawn label=Vocations opened=false");
	TwDefine("DDDAFix/MInclination group=Pawn label=Inclinations opened=false");
	TwDefine("DDDAFix/Pawn group=Stats opened=false");

	//skills
	addSkill(bar, 0xA7928, 6, "Augments", skillsAugments, 83);
	addSkill(bar, 0xA7808 + 24 * 0, 3, "Sword", skillsSword, 21);
	addSkill(bar, 0xA7808 + 24 * 1, 3, "Mace", skillsSword, 21);
	addSkill(bar, 0xA7808 + 24 * 2, 3, "Longsword", skillsLongsword, 11);
	addSkill(bar, 0xA7808 + 24 * 6, 3, "Warhammer", skillsLongsword, 11);
	addSkill(bar, 0xA7808 + 24 * 3, 3, "Dagger", skillsDagger, 21);
	addSkill(bar, 0xA7808 + 24 * 4, 6, "Staff", skillsStaves, 31);
	addSkill(bar, 0xA7808 + 24 * 5, 6, "Archistaff", skillsStaves, 31);
	addSkill(bar, 0xA7808 + 24 * 7, 3, "Shield", skillsShield, 10);
	addSkill(bar, 0xA7808 + 24 * 8, 3, "MagickShield", skillsMagickShield, 16);
	addSkill(bar, 0xA7808 + 24 * 9, 3, "Bow", skillsBow, 10);
	addSkill(bar, 0xA7808 + 24 * 10, 3, "Longbow", skillsLongbow, 10);
	addSkill(bar, 0xA7808 + 24 * 11, 3, "MagickBow", skillsMagickBow, 10);
	TwDefine("DDDAFix/Skills opened=false");

	//a7940 - current primary 1
	//a7944 - current primary 2
	//a7948 - current primary 3
	//a794C - current secondary 1
	//a7950 - current secondary 2
	//a7954 - current secondary 3
	//a76e4 - current primary weapon
	//a76e8 - current secondary weapon
}

void Hooks::PlayerStats() { TweakBarAdd(addPlayerStats); }

TwEnumVal skillsSword[] =
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

TwEnumVal skillsLongsword[] =
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

TwEnumVal skillsDagger[] =
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

TwEnumVal skillsStaves[] =
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

/*TwEnumVal skillsStaff[] =
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

TwEnumVal skillsArchistaff[] =
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

TwEnumVal skillsShield[] =
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

TwEnumVal skillsMagickShield[] =
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

TwEnumVal skillsBow[] =
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

TwEnumVal skillsLongbow[] =
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

TwEnumVal skillsMagickBow[] =
{
	{ -1, "-1: Empty" },
	{ 359, "359: Threefold Bolt | Sixfold Bolt" },
	{ 360, "360: Seeker | True Seeker" },
	{ 361, "361: Explosive Bolt | Explosive Rivet" },
	{ 362, "362: Ricochet Seeker | Ricochet Hunter" },
	{ 363, "363: Magickal Flare | Magickal Gleam" },
	{ 364, "364: Funnel Trail | Vortex Trail" },
	{ 365, "365: Ward Arrow | Great Ward Arrow" },
	{ 366, "366: Bracer Arrow | Great Bracer Arrow" },
	{ 367, "367: Sacrificial Bolt | Great Sacrifice" }
};

TwEnumVal skillsAugments[] =
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
	{ 91, "91: Suasion" },
	{ 92, "92: Thrift" },
	{ 93, "93: Weal" },
	{ 94, "94: Renown" },
	{ 100, "100: Predation" },
	{ 101, "101: Fortune" },
	{ 102, "102: Tenacity" },
	{ 103, "103: Conveyance" },
	{ 104, "104: Acquisition" },
	{ 105, "105: Acquisition" },
	{ 106, "106: Prolongation" },
	{ 107, "107: Mettle" },
	{ 108, "108: Athleticism" },
	{ 109, "109: Recuperation" },
	{ 110, "110: Adhesion" },
	{ 111, "111: Opportunism" },
	{ 112, "112: Flow" },
	{ 113, "113: Grace" },
	{ 114, "114: Facility" }
};