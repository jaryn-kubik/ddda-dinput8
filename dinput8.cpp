#include "dinput8.h"
#include "include\MinHook.h"
#include "d3d9.h"
#include "SaveBackup.h"
#include "Misc.h"
#include "Cheats.h"
#include "InGameClock.h"
#include "iniConfig.h"
#include "Hotkeys.h"
#include "TweakBar.h"
#include "ItemEditor.h"
#include "PlayerStats.h"
#include "Affinity.h"
#include "Server.h"

typedef HRESULT(WINAPI *tDirectInput8Create)(HINSTANCE inst_handle, DWORD version, const IID& r_iid, LPVOID* out_wrapper, LPUNKNOWN p_unk);
tDirectInput8Create oDirectInput8Create;
std::wofstream logFile("dinput8.log", std::ios_base::out);
iniConfig config(L".\\dinput8.ini");

void Initialize()
{
	logFile << "MH_Initialize: " << MH_StatusToString(MH_Initialize()) << std::endl;

	Hooks::Utils();
	Hooks::SaveBackup();
	Hooks::Hotkeys();
	Hooks::Misc();
	Hooks::Cheats();
	Hooks::Affinity();
	Hooks::Server();
	if (Hooks::D3D9())
	{
		Hooks::PlayerStats();
		Hooks::ItemEditor();
		Hooks::InGameClock();
		Hooks::TweakBar();
	}

	wstring loadLibrary = config.getStrW(L"main", L"loadLibrary", wstring());
	if (!loadLibrary.empty())
	{
		HMODULE hMod = LoadLibrary(loadLibrary.c_str());
		tDirectInput8Create newProc = (tDirectInput8Create)GetProcAddress(hMod, "DirectInput8Create");
		if (newProc)
			oDirectInput8Create = newProc;
	}
}

void Unitialize()
{
	logFile << "MH_DisableHook: " << MH_StatusToString(MH_DisableHook(MH_ALL_HOOKS)) << std::endl;
	logFile << "MH_Uninitialize: " << MH_StatusToString(MH_Uninitialize()) << std::endl;
	if (logFile)
		logFile.close();
}

void Hooks::CreateHook(LPCSTR msg, LPVOID pTarget, LPVOID pDetour, LPVOID* ppOriginal, bool enable)
{
	logFile << msg << " hook: " << MH_StatusToString(MH_CreateHook(pTarget, pDetour, ppOriginal)) << ", ";
	if (enable)
		logFile << MH_StatusToString(MH_EnableHook(pTarget)) << std::endl;
	else
		logFile << "disabled" << std::endl;
}

void Hooks::SwitchHook(LPCSTR msg, LPVOID pTarget, bool enable)
{
	if (enable)
		logFile << msg << " enable: " << MH_StatusToString(MH_EnableHook(pTarget)) << std::endl;
	else
		logFile << msg << " disable: " << MH_StatusToString(MH_DisableHook(pTarget)) << std::endl;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);

		WCHAR syspath[512];
		GetSystemDirectory(syspath, 512);
		wcscat_s(syspath, L"\\dinput8.dll");
		HMODULE hMod = LoadLibrary(syspath);
		oDirectInput8Create = (tDirectInput8Create)GetProcAddress(hMod, "DirectInput8Create");

		Initialize();
	}
	else if (ul_reason_for_call == DLL_PROCESS_DETACH)
		Unitialize();
	return TRUE;
}

HRESULT WINAPI DirectInput8Create(HINSTANCE inst_handle, DWORD version, const IID& r_iid, LPVOID* out_wrapper, LPUNKNOWN p_unk)
{
	return oDirectInput8Create(inst_handle, version, r_iid, out_wrapper, p_unk);
}