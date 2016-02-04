#pragma once
#include <windows.h>
#include <fstream>
#include "utils.h"
#include "iniConfig.h"
using std::string;

extern std::wofstream logFile;
extern iniConfig config;

namespace Hooks
{
	void CreateHook(LPCSTR msg, LPVOID pTarget, LPVOID pDetour, LPVOID *ppOriginal);

	template <typename T>
	void CreateHook(LPCSTR msg, LPVOID pTarget, LPVOID pDetour, T** ppOriginal)
	{
		CreateHook(msg, pTarget, pDetour, reinterpret_cast<LPVOID*>(ppOriginal));
	}
}