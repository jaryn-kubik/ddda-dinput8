#pragma once
#define WIN32_LEAN_AND_MEAN
#define TW_STATIC

#include <SDKDDKVer.h>
#include <windows.h>

#include <fstream>
#include <functional>
#include <vector>
#include <algorithm>

using std::string;
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include "iniConfig.h"
#include "dinput8.h"
#include "include\AntTweakBar.h"
#include "include\imgui.h"
#include "InGameUI.h"
#include "steam_api.h"