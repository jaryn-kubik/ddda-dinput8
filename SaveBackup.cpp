#include "SaveBackup.h"
#include "dinput8.h"
#include "utils.h"

void __stdcall handleSave()
{
	std::string path = config.Get("", "savePath", "");
	path.erase(path.find_last_not_of('\\') + 1);
	path.push_back('\\');

	WIN32_FILE_ATTRIBUTE_DATA fileData;
	if (GetFileAttributesExA((path + "ddda.sav").c_str(), GetFileExInfoStandard, &fileData))
	{
		SYSTEMTIME systemTime, t;
		FileTimeToSystemTime(&fileData.ftLastWriteTime, &systemTime);
		SystemTimeToTzSpecificLocalTime(nullptr, &systemTime, &t);

		CHAR str[64];
		snprintf(str, 64, "ddda_%04d-%02d-%02d_%02d-%02d-%02d.sav", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);

		if (CopyFileA((path + "ddda.sav").c_str(), (path + str).c_str(), FALSE))
			logFile << "Creating backup: " << (path + str).c_str() << std::endl;
		else
			logFile << "Creating backup: " << (LPVOID)GetLastError() << std::endl;
	}
	else
		logFile << "Finding save: " << (LPVOID)GetLastError() << std::endl;
}

BYTE *pSaveGame = nullptr;
LPVOID oSaveGame = nullptr;
void __declspec(naked) HSaveGame()
{
	__asm
	{
		pushad;
		call	handleSave;
		popad;
		jmp		oSaveGame;
	}
}

void SaveBackup::Init()
{
	BYTE *pSaveName;
	BYTE saveName[] = "DDDA.sav";
	if (utils::FindData(saveName, &pSaveName, "SaveBackup saveName"))
	{
		BYTE sig[] = { 0x8D, 0x93, 0xE6, 0x09, 0x00, 0x00,
						0x68, 0x00, 0x00, 0x00, 0x00 };
		*(LPDWORD)(sig + 7) = (DWORD)pSaveName;

		if (utils::FindSignature(sig, &pSaveGame, "SaveBackup hook"))
		{
			logStatus("SaveBackup hook", MH_CreateHook(pSaveGame, &HSaveGame, &oSaveGame));
			logStatus("SaveBackup enable", MH_EnableHook(pSaveGame));
		}
		else
			pSaveGame = nullptr;
	}
}

void SaveBackup::Uninit()
{
	if (pSaveGame)
		logStatus("SaveBackup disable", MH_DisableHook(pSaveGame));
}