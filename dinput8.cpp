#include "dinput8.h"
#include "CreateFileHook.h"
#include "MinHook.h"

typedef HRESULT(WINAPI *tDirectInput8Create)(HINSTANCE inst_handle, DWORD version, const IID& r_iid, LPVOID* out_wrapper, LPUNKNOWN p_unk);
tDirectInput8Create oDirectInput8Create;
std::wofstream logFile;

void logStatus(LPCSTR name, MH_STATUS status) { logFile << name << ": " << MH_StatusToString(status) << std::endl; }

void Initialize()
{
	logFile.open("dinput8.log", std::ios_base::out);
	logStatus("MH_Initialize()", MH_Initialize());
	logStatus("CreateFileW hook", MH_CreateHookEx(&CreateFileW, &HCreateFileW, &oCreateFileW));
	logStatus("CreateFileW enable", MH_EnableHook(&CreateFileW));
}

void Unitialize()
{
	logStatus("CreateFileW disable", MH_DisableHook(&CreateFileW));
	logStatus("MH_Uninitialize()", MH_Uninitialize());
	if (logFile)
		logFile.close();
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