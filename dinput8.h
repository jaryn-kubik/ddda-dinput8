#pragma once
#include <windows.h>
#include <fstream>
#include "utils.h"
#include "iniConfig.h"

extern std::wofstream logFile;
extern iniConfig config;

namespace Hooks
{
	void CreateHook(LPCSTR msg, LPVOID pTarget, LPVOID pDetour, LPVOID *ppOriginal, bool enable = true);
	void SwitchHook(LPCSTR msg, LPVOID pTarget, bool enable);

	template <typename T>
	void CreateHook(LPCSTR msg, LPVOID pTarget, LPVOID pDetour, T** ppOriginal, bool enable = true)
	{
		CreateHook(msg, pTarget, pDetour, reinterpret_cast<LPVOID*>(ppOriginal), enable);
	}
}