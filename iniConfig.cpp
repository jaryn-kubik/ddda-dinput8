#include "dinput8.h"
#include "iniConfig.h"
#include <algorithm>
#include <string>
#include <locale>

iniConfig::iniConfig(LPCTSTR fileName)
{
	this->fileName = fileName;

	SetLastError(ERROR_SUCCESS);
	GetPrivateProfileSectionNames(buffer, 512, fileName);
	if (GetLastError() == ERROR_FILE_NOT_FOUND)
		logFile << "Config: file not found!" << std::endl;
}

bool iniConfig::get(LPCTSTR section, LPCTSTR key, bool allowEmpty)
{
	SetLastError(ERROR_SUCCESS);
	DWORD result = GetPrivateProfileString(section, key, nullptr, buffer, 512, fileName);
	return GetLastError() != ERROR_FILE_NOT_FOUND && (allowEmpty || result > 0);
}

template <typename T>
T printError(LPCTSTR section, LPCTSTR key, T defValue)
{
	logFile << "Config: " << section << "->" << key << " has invalid value, using default (" << defValue << ")" << std::endl;
	return defValue;
}

template <typename T>
std::basic_string<T> printError(LPCTSTR section, LPCTSTR key, std::basic_string<T> defValue)
{
	logFile << "Config: " << section << "->" << key << " has invalid value, using default (" << defValue.c_str() << ")" << std::endl;
	return defValue;
}

std::string iniConfig::getStrA(LPCTSTR section, LPCTSTR key, std::string defValue)
{
	if (get(section, key, true))
#ifdef UNICODE
		return std::wstring_convert<std::codecvt<wchar_t, char, mbstate_t>>().to_bytes(buffer);
#else
		return buffer;
#endif
	return printError(section, key, defValue);
}

std::wstring iniConfig::getStrW(LPCTSTR section, LPCTSTR key, std::wstring defValue)
{
	if (get(section, key, true))
#ifdef UNICODE
		return buffer;
#else
		return std::wstring_convert<std::codecvt<wchar_t, char, mbstate_t>>().from_bytes(buffer);
#endif
	return printError(section, key, defValue);
}

int iniConfig::getInt(LPCTSTR section, LPCTSTR key, int defValue)
{
	try
	{
		if (get(section, key))
			return std::stoi(buffer, nullptr, 0);
	}
	catch (...) {}
	return printError(section, key, defValue);
}

unsigned iniConfig::getUInt(LPCTSTR section, LPCTSTR key, unsigned defValue)
{
	try
	{
		if (get(section, key))
			return std::stoul(buffer, nullptr, 0);
	}
	catch (...) {}
	return printError(section, key, defValue);
}

float iniConfig::getFloat(LPCTSTR section, LPCTSTR key, float defValue)
{
	try
	{
		if (get(section, key))
			return std::stof(buffer, nullptr);
	}
	catch (...) {}
	return printError(section, key, defValue);
}

double iniConfig::getDouble(LPCTSTR section, LPCTSTR key, double defValue)
{
	try
	{
		if (get(section, key))
			return std::stod(buffer, nullptr);
	}
	catch (...) {}
	return printError(section, key, defValue);
}

bool iniConfig::getBool(LPCTSTR section, LPCTSTR key, bool defValue)
{
	try
	{
		if (get(section, key))
		{
			std::wstring str(buffer);
			transform(str.begin(), str.end(), str.begin(), tolower);
			if (str == L"true" || str == L"yes" || str == L"on")
				return true;
			if (str == L"false" || str == L"no" || str == L"off")
				return false;
		}
	}
	catch (...) {}
	return printError(section, key, defValue);
}

int iniConfig::getEnum(LPCTSTR section, LPCTSTR key, int defValue, std::map<stringType, int> map)
{
	try
	{
		if (get(section, key))
		{
			std::wstring str(buffer);
			transform(str.begin(), str.end(), str.begin(), tolower);
			return map.at(str);
		}
	}
	catch (...) {}
	return printError(section, key, defValue);
}