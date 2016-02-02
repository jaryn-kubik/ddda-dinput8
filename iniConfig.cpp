#include "dinput8.h"
#include "iniConfig.h"
#include <algorithm>
#include <string>

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

iniConfig::stringType iniConfig::getStr(LPCTSTR section, LPCTSTR key, stringType defValue)
{
	if (get(section, key, true))
		return buffer;
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