#pragma once
#define WIN32_LEAN_AND_MEAN
#define TW_STATIC

#include <SDKDDKVer.h>
#include <windows.h>

#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <array>
#include <numeric>

using std::string;
#pragma comment(lib, "d3dx9.lib")

#include "iniConfig.h"
#include "dinput8.h"
#include "ImGui\imgui.h"
#include "InGameUI.h"
#include "steam_api.h"
#include "Hotkeys.h"

namespace Hooks
{
	extern const std::vector<std::pair<int, LPCSTR>> ListStatus;
	extern const std::vector<std::pair<int, LPCSTR>> ListVocations;
	extern const std::vector<std::pair<int, LPCSTR>> ListSkillsCore;
	extern const std::vector<std::pair<int, LPCSTR>> ListSkillsSword;
	extern const std::vector<std::pair<int, LPCSTR>> ListSkillsLongsword;
	extern const std::vector<std::pair<int, LPCSTR>> ListSkillsDagger;
	extern const std::vector<std::pair<int, LPCSTR>> ListSkillsStaves;
	extern const std::vector<std::pair<int, LPCSTR>> ListSkillsShield;
	extern const std::vector<std::pair<int, LPCSTR>> ListSkillsMagickShield;
	extern const std::vector<std::pair<int, LPCSTR>> ListSkillsBow;
	extern const std::vector<std::pair<int, LPCSTR>> ListSkillsLongbow;
	extern const std::vector<std::pair<int, LPCSTR>> ListSkillsMagickBow;
	extern const std::vector<std::pair<int, LPCSTR>> ListSkillsAugments;

	extern const std::vector<std::pair<int, LPCSTR>> ListItemMaster;
	extern const std::vector<std::pair<int, LPCSTR>> ListItemEnchant;
	extern const std::vector<std::pair<int, LPCSTR>> ListItemBonus;
	extern const std::vector<std::pair<int, LPCSTR>> ListItemId;
}