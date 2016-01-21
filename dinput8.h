#pragma once
#include <windows.h>
#include <fstream>
#include "MinHook.h"
#include "json.hpp"

extern std::wofstream logFile;
extern nlohmann::json config;

void logStatus(LPCSTR name, MH_STATUS status);