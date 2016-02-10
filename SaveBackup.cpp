#include "stdafx.h"
#include "SaveBackup.h"

std::wstring saveDir, savePath;
int saveLimit;
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
	logFile << std::endl;
}

void clearBackups()
{
	WIN32_FIND_DATA ffd;
	HANDLE hFind = FindFirstFile((saveDir + L"*_*.sav").c_str(), &ffd);
	if (hFind == INVALID_HANDLE_VALUE)
		return;

	std::vector<std::pair<UINT64, std::wstring>> files;
	do
	{
		SYSTEMTIME t;
		if (swscanf_s(ffd.cFileName, L"ddda_%04hd-%02hd-%02hd_%02hd-%02hd-%02hd.sav",
			&t.wYear, &t.wMonth, &t.wDay, &t.wHour, &t.wMinute, &t.wSecond) == 6)
		{
			UINT64 time;
			SystemTimeToFileTime(&t, (LPFILETIME)&time);
			files.push_back({ time, ffd.cFileName });
		}
	} while (FindNextFile(hFind, &ffd));
	FindClose(hFind);

	sort(files.begin(), files.end());
	for (int i = 0; i < (int)files.size() - saveLimit; i++)
	{
		std::wstring file = saveDir + files[i].second;
		DeleteFile(file.c_str());
		logFile << "Backup deleted: " << file << std::endl;
	}
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
		swprintf_s(str, L"ddda_%04hd-%02hd-%02hd_%02hd-%02hd-%02hd.sav", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);
		std::wstring backupPath = saveDir + str;

		if (CopyFile(savePath.c_str(), backupPath.c_str(), FALSE))
		{
			logFile << "Backup created: " << backupPath.c_str() << std::endl;
			if (saveLimit > 0)
				clearBackups();
		}
		else
			printError("Creating backup", GetLastError());
	}
	else
		printError("Finding save", GetLastError());
}

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

bool findSavePath()
{
	std::wstring configDir = config.getStrW(L"main", L"savePath", std::wstring());
	if (configDir.empty())
	{
		HMODULE hModule = GetModuleHandle(L"steam_api.dll");
		if (hModule)
		{
			char buf[1024];
			tSteamUser pSteamUser = (tSteamUser)GetProcAddress(hModule, "SteamUser");
			if (pSteamUser && pSteamUser() && pSteamUser()->GetUserDataFolder(buf, 1024))
			{
				saveDir = std::wstring_convert<std::codecvt<wchar_t, char, mbstate_t>>().from_bytes(buf);
				size_t index = saveDir.rfind(L"local");
				if (index != std::string::npos)
				{
					saveDir.erase(index);
					saveDir += L"remote";
				}
			}
		}
	}
	else
		saveDir = configDir;
	saveDir.erase(saveDir.find_last_not_of('\\') + 1);

	DWORD attributes = GetFileAttributes(saveDir.c_str());
	if (attributes == INVALID_FILE_ATTRIBUTES || (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
	{
		if (configDir.empty())
			logFile << "SaveBackup path: NOT FOUND, SET MANUALLY IN DINPUT8.INI" << std::endl;
		else
			logFile << "SaveBackup path: INVALID PATH - " << saveDir << std::endl;
		return false;
	}

	saveDir.push_back('\\');
	savePath = saveDir + L"ddda.sav";
	logFile << "SaveBackup path: " << savePath << std::endl;
	return true;
}

void Hooks::SaveBackup()
{
	if (!config.getBool(L"main", L"backupSaves", false) || !findSavePath())
	{
		logFile << "SaveBackup: disabled" << std::endl;
		return;
	}

	saveLimit = config.getInt(L"main", L"saveLimit", -1);

	BYTE *pSaveName;
	BYTE saveName[] = "DDDA.sav";
	if (FindData("SaveBackup", saveName, &pSaveName))
	{
		BYTE sig[] = { 0x8D, 0x93, 0xE6, 0x09, 0x00, 0x00,
						0x68, 0x00, 0x00, 0x00, 0x00 };
		*(LPDWORD)(sig + 7) = (DWORD)pSaveName;
		BYTE *pOffset;
		if (FindSignature("SaveBackup", sig, &pOffset))
			CreateHook("SaveBackup", pOffset, &HSaveGame, &oSaveGame);
	}
}