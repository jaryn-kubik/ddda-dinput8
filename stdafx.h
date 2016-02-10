#pragma once
#define WIN32_LEAN_AND_MEAN
#define TW_STATIC

#include <SDKDDKVer.h>
#include <windows.h>

#include <fstream>
#include <functional>
#include <map>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <locale>
#include <sstream>
#include <iomanip>

using std::string;
using std::wstring;
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include "iniConfig.h"
#include "dinput8.h"
#include "include\AntTweakBar.h"
#include "TweakBar.h"
#include "steam_api.h"