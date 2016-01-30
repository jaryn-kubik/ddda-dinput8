#include "dinput8.h"
#include "include\MinHook.h"
#include "d3d9.h"
#include "SaveBackup.h"
#include "CharCustomization.h"
#include "Cheats.h"
#include "InGameClock.h"

std::wofstream logFile("dinput8.log", std::ios_base::out);
INIReader config("dinput8.ini");

void Initialize()
{
	logFile << "MH_Initialize: " << MH_StatusToString(MH_Initialize()) << std::endl;
	if (config.ParseError() == 0)
	{
		Hooks::Utils();
		Hooks::SaveBackup();
		Hooks::CharCustomization();
		Hooks::Cheats();
		if (Hooks::D3D9())
		{
			Hooks::InGameClock();
		}
	}
	else if (config.ParseError() == -1)
		logFile << "Config: file not found!" << std::endl;
	else
		logFile << "Config: parse error on line " << config.ParseError() << std::endl;
}

void Unitialize()
{
	logFile << "MH_DisableHook: " << MH_StatusToString(MH_DisableHook(MH_ALL_HOOKS)) << std::endl;
	logFile << "MH_Uninitialize: " << MH_StatusToString(MH_Uninitialize()) << std::endl;
	if (logFile)
		logFile.close();
}

void Hooks::CreateHook(LPCSTR msg, LPVOID pTarget, LPVOID pDetour, LPVOID* ppOriginal)
{
	MH_STATUS create = MH_CreateHook(pTarget, pDetour, ppOriginal);
	MH_STATUS enable = MH_EnableHook(pTarget);
	logFile << msg << " hook: " << MH_StatusToString(create) << ", " << MH_StatusToString(enable) << std::endl;
}

typedef HRESULT(WINAPI *tDirectInput8Create)(HINSTANCE inst_handle, DWORD version, const IID& r_iid, LPVOID* out_wrapper, LPUNKNOWN p_unk);
tDirectInput8Create oDirectInput8Create;

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