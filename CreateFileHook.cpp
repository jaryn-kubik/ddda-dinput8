/*#include "CreateFileHook.h"
#include "dinput8.h"
#include <ctime>
#include <cwchar>
#include <sstream>
#include <iomanip>*/

/*std::wstring findLastSave(std::wstring dir)
{
	time_t lastTime = 0;
	WCHAR lastFile[MAX_PATH];

	WIN32_FIND_DATA ffd;
	HANDLE hFind = FindFirstFileW((dir + L"ddda_*.sav").c_str(), &ffd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			tm t = {};
			std::wistringstream ss(ffd.cFileName);
			ss >> std::get_time(&t, L"ddda_%Y-%m-%d_%H-%M-%S.sav");
			if (!ss.fail())
			{
				time_t time = mktime(&t);
				if (time > lastTime)
				{
					lastTime = time;
					wcscpy_s(lastFile, ffd.cFileName);
				}
			}
		} while (FindNextFile(hFind, &ffd) != 0);
	}
	FindClose(hFind);
	return dir + (lastTime > 0 ? lastFile : L"ddda.sav");
}

typedef HANDLE(WINAPI *tCreateFileW)(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
tCreateFileW oCreateFileW;

HANDLE WINAPI HCreateFileW(LPCWSTR fileName, DWORD access, DWORD share, LPSECURITY_ATTRIBUTES sec, DWORD disp, DWORD flags, HANDLE templ)
{
	std::wstring str(fileName);
	size_t index = str.rfind(L"ddda.sav");
	if (index != std::string::npos && str.size() - index == 8)
	{
		auto charNumber = config["charNumber"];
		if (charNumber.is_number_integer() && charNumber.get<int>() > 0)
		{
			str.insert(index, std::to_wstring(charNumber.get<int>()) + L"\\");
			index = str.size() - 8;
			CreateDirectory(str.substr(0, index).c_str(), nullptr);
		}
		if (access == GENERIC_READ)
		{
			str = findLastSave(str.erase(index));
			logFile << "Loaded: " << str << std::endl;
			return oCreateFileW(str.c_str(), access, share, sec, disp, flags, templ);
		}
		if (access == GENERIC_WRITE)
		{
			time_t t = time(nullptr);
			wchar_t wstr[32];
			wcsftime(wstr, 32, L"_%Y-%m-%d_%H-%M-%S", localtime(&t));
			logFile << "Saved: " << str.insert(index + 4, wstr).c_str() << std::endl;
			return oCreateFileW(str.c_str(), access, share, sec, disp, flags, templ);
		}
		logFile << "Unknown access: " << access << std::endl;
	}
	return oCreateFileW(fileName, access, share, sec, disp, flags, templ);
}*/

/*BYTE *pOpen, *pSave, *pLoad1, *pLoad2;
LPVOID oOpen;
CHAR pNewName[MAX_PATH] = "DDDA.sav";
void __stdcall handleSave(LPVOID returnAddress)
{
	if (returnAddress == pLoad1 || returnAddress == pLoad2)
	{
		auto module = GetModuleHandle(L"steam_api.dll");
		auto vole = GetProcAddress(module, "SteamAPI_ISteamUser_GetUserDataFolder");
		logFile << vole << std::endl;
		//str = findLastSave(str.erase(index));
		//logFile << "Loaded: " << str << std::endl;
	}
	else if (returnAddress == pSave)
	{
		time_t t = time(nullptr);
		CHAR str[MAX_PATH];
		strftime(str, MAX_PATH, "ddda_%Y-%m-%d_%H-%M-%S.sav", localtime(&t));
		strcpy(pNewName, str);
		logFile << "Saved: " << str << std::endl;
	}
	else
	{
		strcpy(pNewName, "DDDA_unknown.sav");
		logFile << "DEBUG: " << returnAddress << std::endl;
	}
}

void __declspec(naked) HOpenFile()
{
	__asm
	{
		pushad;
		mov		eax, [esp + 32];
		push	eax;
		call	handleSave;
		popad;
		jmp		oOpen;
	}
}

void CreateFileHook::Init()
{
	//sub_4B6770 - openFile
	BYTE sigOpen[] = { 0x53, 0x56, 0x57,		//push	edx, esi, edi
						0xFF, 0x24, 0x85 };		//jmp	xxx[eax*4]

	//sub_4B7740+C4 - save
	BYTE sigSave[] = { 0xB8, 0x01, 0x00, 0x00, 0x00,	//mov	eax, 1
						0xE8, 0xCC, 0xCC, 0xCC, 0xCC,	//call	openFile
						0x8B, 0xF3,						//mov	esi, ebx
						0xE8 };							//call

	//sub_4B7740+DA - load 1
	BYTE sigLoad1[] = { 0xB8, 0x01, 0x00, 0x00, 0x00,			//mov	eax, 1
						0xE8, 0xCC, 0xCC, 0xCC, 0xCC,			//call	openFile
						0x8B, 0xF3,								//mov	esi, ebx
						0x89, 0x83, 0x4C, 0x0A, 0x00, 0x00 };	//mov	[ebx+0A4Ch], eax

	//sub_4B7740+100 - load 2
	BYTE sigLoad2[] = { 0xB8, 0x04, 0x00, 0x00, 0x00,			//mov	eax, 4
						0xE8, 0xCC, 0xCC, 0xCC, 0xCC,			//call	openFile
						0x83, 0xF8, 0x01,						//cmp	eax, 1
						0x74 };									//jz

	if (utils::FindSignature(sigOpen, &pOpen, "SaveBackup signature") &
		utils::FindSignature(sigSave, &pSave, "SaveBackup signature save") &
		utils::FindSignature(sigLoad1, &pLoad1, "SaveBackup signature load1") &
		utils::FindSignature(sigLoad2, &pLoad2, "SaveBackup signature load2"))
	{
		pSave += 10;
		pLoad1 += 10;
		pLoad2 += 10;

		BYTE *pSaveName;
		BYTE saveName[] = "DDDA.sav";
		if (utils::FindData(saveName, &pSaveName, "SaveBackup saveName"))
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
		}

		logStatus("SaveBackup hook", MH_CreateHook(pOpen, &HOpenFile, &oOpen));
		logStatus("SaveBackup enable", MH_EnableHook(pOpen));
	}
	else
		pOpen = nullptr;
}

void CreateFileHook::Uninit()
{
	logStatus("SaveBackup disable", MH_DisableHook(pOpen));
}*/