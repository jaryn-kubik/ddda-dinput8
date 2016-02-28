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

using std::string;
#pragma comment(lib, "d3dx9.lib")

#include "iniConfig.h"
#include "dinput8.h"
#include "ImGui\imgui.h"
#include "InGameUI.h"
#include "steam_api.h"