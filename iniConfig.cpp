#include "dinput8.h"
#include "iniConfig.h"
#include <algorithm>
#include <string>
#include <locale>
#include <sstream>
#include <iomanip>

iniConfig::iniConfig(LPCWSTR fileName)
{
	this->fileName = fileName;

	SetLastError(ERROR_SUCCESS);
	GetPrivateProfileSectionNamesW(buffer, 512, fileName);
	if (GetLastError() == ERROR_FILE_NOT_FOUND)
		logFile << "Config: file not found!" << std::endl;
}

bool iniConfig::get(LPCWSTR section, LPCWSTR key, bool allowEmpty)
{
	SetLastError(ERROR_SUCCESS);
	DWORD result = GetPrivateProfileStringW(section, key, nullptr, buffer, 512, fileName);
	return GetLastError() != ERROR_FILE_NOT_FOUND && (allowEmpty || result > 0);
}

template <typename T>
T printError(LPCWSTR section, LPCWSTR key, T defValue)
{
	logFile << "Config: " << section << "->" << key << " has invalid value, using default (" << defValue << ")" << std::endl;
	return defValue;
}

template <typename T>
std::basic_string<T> printError(LPCWSTR section, LPCWSTR key, std::basic_string<T> defValue)
{
	logFile << "Config: " << section << "->" << key << " has invalid value, using default (" << defValue.c_str() << ")" << std::endl;
	return defValue;
}

string iniConfig::getStrA(LPCWSTR section, LPCWSTR key, string defValue)
{
	if (get(section, key, true))
		return std::wstring_convert<std::codecvt<wchar_t, char, mbstate_t>>().to_bytes(buffer);
	return printError(section, key, defValue);
}

wstring iniConfig::getStrW(LPCWSTR section, LPCWSTR key, wstring defValue)
{
	if (get(section, key, true))
		return buffer;
	return printError(section, key, defValue);
}

int iniConfig::getInt(LPCWSTR section, LPCWSTR key, int defValue)
{
	try
	{
		if (get(section, key))
			return std::stoi(buffer, nullptr, 0);
	}
	catch (...) {}
	return printError(section, key, defValue);
}

unsigned int iniConfig::getUInt(LPCWSTR section, LPCWSTR key, unsigned defValue)
{
	try
	{
		if (get(section, key))
			return std::stoul(buffer, nullptr, 0);
	}
	catch (...) {}
	return printError(section, key, defValue);
}

float iniConfig::getFloat(LPCWSTR section, LPCWSTR key, float defValue)
{
	try
	{
		if (get(section, key))
			return std::stof(buffer, nullptr);
	}
	catch (...) {}
	return printError(section, key, defValue);
}

double iniConfig::getDouble(LPCWSTR section, LPCWSTR key, double defValue)
{
	try
	{
		if (get(section, key))
			return std::stod(buffer, nullptr);
	}
	catch (...) {}
	return printError(section, key, defValue);
}

bool iniConfig::getBool(LPCWSTR section, LPCWSTR key, bool defValue)
{
	try
	{
		if (get(section, key))
		{
			wstring str(buffer);
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

int iniConfig::getEnum(LPCWSTR section, LPCWSTR key, int defValue, std::map<wstring, int> map)
{
	try
	{
		if (get(section, key))
		{
			wstring str(buffer);
			transform(str.begin(), str.end(), str.begin(), tolower);
			return map.at(str);
		}
	}
	catch (...) {}
	return printError(section, key, defValue);
}

void iniConfig::setStrA(LPCWSTR section, LPCWSTR key, string value) const
{
	wstring str = std::wstring_convert<std::codecvt<wchar_t, char, mbstate_t>>().from_bytes(value);
	WritePrivateProfileString(section, key, (L" " + str).c_str(), fileName);
}

void iniConfig::setStrW(LPCWSTR section, LPCWSTR key, wstring value) const
{
	WritePrivateProfileString(section, key, (L" " + value).c_str(), fileName);
}

template<typename T> wstring intToHex(T value)
{
	std::wstringstream stream;
	stream << std::showbase << std::setfill(L'0') << std::setw(sizeof(T) * 2) << std::hex << value;
	return stream.str();
}

wstring enumToString(int value, std::map<wstring, int> map)
{
	for (auto &v : map)
		if (v.second == value)
			return v.first;
	return std::to_wstring(value);
}

void iniConfig::setInt(LPCWSTR section, LPCWSTR key, int value, bool hex) const { setStrW(section, key, hex ? intToHex(value) : std::to_wstring(value)); }
void iniConfig::setUInt(LPCWSTR section, LPCWSTR key, unsigned value, bool hex) const { setStrW(section, key, hex ? intToHex(value) : std::to_wstring(value)); }
void iniConfig::setFloat(LPCWSTR section, LPCWSTR key, float value) const { setStrW(section, key, std::to_wstring(value)); }
void iniConfig::setDouble(LPCWSTR section, LPCWSTR key, double value) const { setStrW(section, key, std::to_wstring(value)); }
void iniConfig::setBool(LPCWSTR section, LPCWSTR key, bool value) const { setStrW(section, key, value ? L"on" : L"off"); };
void iniConfig::setEnum(LPCWSTR section, LPCWSTR key, int value, std::map<wstring, int> map) const { setStrW(section, key, enumToString(value, map)); }
