#include "CreateFileHook.h"
#include "dinput8.h"
#include <ctime>
#include <cwchar>
#include <sstream>
#include <iomanip>

std::wstring findLastSave(std::wstring dir)
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
}

void CreateFileHook::Init()
{
	logStatus("CreateFileW hook", MH_CreateHookEx(&CreateFileW, &HCreateFileW, &oCreateFileW));
	logStatus("CreateFileW enable", MH_EnableHook(&CreateFileW));
}

void CreateFileHook::Uninit()
{
	logStatus("CreateFileW disable", MH_DisableHook(&CreateFileW));
}