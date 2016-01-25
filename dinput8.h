#pragma once
#include <windows.h>
#include <fstream>
#include "utils.h"
#include "include\MinHook.h"
#include "include\INIReader.h"

extern std::wofstream logFile;
extern INIReader config;

void logStatus(LPCSTR name, MH_STATUS status);