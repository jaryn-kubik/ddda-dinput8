#include "SaveBackup.h"
#include "dinput8.h"
#include "utils.h"
#include <locale>

std::wstring saveDir, savePath;
void printError(LPCSTR msg, DWORD error)
{
	logFile << msg << ": ";
	if (error == ERROR_FILE_NOT_FOUND)
		logFile << "file not found";
	else if (error == ERROR_PATH_NOT_FOUND)
		logFile << "path not found";
	else if (error == ERROR_INVALID_NAME)
		logFile << "invalid path";
	else
		logFile << (LPVOID)error;
	logFile << " - " << savePath << std::endl;
}

void __stdcall handleSave()
{
	WIN32_FILE_ATTRIBUTE_DATA fileData;
	if (GetFileAttributesEx(savePath.c_str(), GetFileExInfoStandard, &fileData))
	{
		SYSTEMTIME systemTime, t;
		FileTimeToSystemTime(&fileData.ftLastWriteTime, &systemTime);
		SystemTimeToTzSpecificLocalTime(nullptr, &systemTime, &t);

		WCHAR str[64];
		swprintf(str, 64, L"ddda_%04d-%02d-%02d_%02d-%02d-%02d.sav", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);
		std::wstring backupPath = saveDir + str;

		if (CopyFile(savePath.c_str(), backupPath.c_str(), FALSE))
			logFile << "Creating backup: " << backupPath.c_str() << std::endl;
		else
			printError("Creating backup copy error", GetLastError());
	}
	else
		printError("Creating backup error", GetLastError());
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
	/*
	//sub_4B6770 - openFile
	BYTE sigOpen[] = { 0x53, 0x56, 0x57,		//push	edx, esi, edi
						0xFF, 0x24, 0x85 };		//jmp	xxx[eax*4]

	if (utils::FindSignature(sigOpen, &pOpen, "SaveBackup signature"))
	{
		BYTE sig1[] = { 0x68, 0x00, 0x00, 0x00, 0x00 };
		*(LPDWORD)(sig1 + 1) = (DWORD)pSaveName;

		BYTE *pOffset;
		if (utils::Find(pOpen, pOpen + 0x100, sig1, &pOffset, "SaveBackup hook1"))
			utils::Set((DWORD*)++pOffset, (DWORD)pNewName);
		if (utils::Find(pOffset, pOffset + 0x100, sig1, &pOffset, "SaveBackup hook2"))
			utils::Set((DWORD*)++pOffset, (DWORD)pNewName);
		if (utils::Find(pOffset, pOffset + 0x100, sig1, &pOffset, "SaveBackup hook3"))
			utils::Set((DWORD*)++pOffset, (DWORD)pNewName);

		BYTE sig2[] = { 0x8D, 0x93, 0xE6, 0x09, 0x00, 0x00,
			0x68, 0x00, 0x00, 0x00, 0x00 };
			*(LPDWORD)(sig2 + 7) = (DWORD)pSaveName;

		if (utils::FindSignature(sig2, &pOffset, "SaveBackup hook4"))
			utils::Set((DWORD*)(pOffset += 7), (DWORD)pNewName);
		if (utils::Find(pOffset, pOffset + 0x1000, sig2, &pOffset, "SaveBackup hook5"))
			utils::Set((DWORD*)(pOffset += 7), (DWORD)pNewName);
	}*/

	std::wstring_convert<std::codecvt<wchar_t, char, mbstate_t>> conv;
	saveDir = conv.from_bytes(config.Get("", "savePath", "null"));
	saveDir.erase(saveDir.find_last_not_of('\\') + 1);
	saveDir.push_back('\\');
	savePath = saveDir + L"ddda.sav";

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