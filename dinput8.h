#pragma once
#include <windows.h>
#include <fstream>
#include "utils.h"
#include "include\INIReader.h"

extern std::wofstream logFile;
extern INIReader config;

namespace Hooks
{
	void CreateHook(LPCSTR name, LPVOID pTarget, LPVOID pDetour, LPVOID *ppOriginal);

	template <typename T>
	void CreateHook(LPCSTR msg, LPVOID pTarget, LPVOID pDetour, T** ppOriginal)
	{
		return CreateHook(msg, pTarget, pDetour, reinterpret_cast<LPVOID*>(ppOriginal));
	}
}