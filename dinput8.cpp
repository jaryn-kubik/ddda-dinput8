#include "dinput8.h"
#include "SaveBackup.h"
#include "CharCustomization.h"
#include "d3d9Hook.h"
#include "Cheats.h"

std::wofstream logFile("dinput8.log", std::ios_base::out);
INIReader config("dinput8.ini");

void Initialize()
{
	utils::Initialize();
	if (config.ParseError() == 0)
	{
		logStatus("MH_Initialize()", MH_Initialize());
		SaveBackup::Init();
		CharCustomization::Init();
		d3d9Hook::Init();
		Cheats::Init();
	}
	else if (config.ParseError() == -1)
		logFile << "Config: file not found!" << std::endl;
	else
		logFile << "Config: parse error on line " << config.ParseError() << std::endl;
}

void Unitialize()
{
	if (config.ParseError() == 0)
	{
		Cheats::Uninit();
		d3d9Hook::Uninit();
		CharCustomization::Uninit();
		SaveBackup::Uninit();
		logStatus("MH_Uninitialize()", MH_Uninitialize());
	}
	if (logFile)
		logFile.close();
}

void logStatus(LPCSTR name, MH_STATUS status) { logFile << name << ": " << MH_StatusToString(status) << std::endl; }

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