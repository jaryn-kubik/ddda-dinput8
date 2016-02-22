#include "stdafx.h"
#include "iniConfig.h"

iniConfig::iniConfig(LPCSTR fileName)
{
	this->fileName = fileName;

	SetLastError(ERROR_SUCCESS);
	GetPrivateProfileSectionNamesA(buffer, sizeof buffer, fileName);
	if (GetLastError() == ERROR_FILE_NOT_FOUND)
		logFile << "Config: file not found!" << std::endl;
}

bool iniConfig::get(LPCSTR section, LPCSTR key, bool allowEmpty)
{
	SetLastError(ERROR_SUCCESS);
	DWORD result = GetPrivateProfileStringA(section, key, nullptr, buffer, sizeof buffer, fileName);
	return GetLastError() != ERROR_FILE_NOT_FOUND && (allowEmpty || result > 0);
}

void iniConfig::removeKey(LPCSTR section, LPCSTR key) const { WritePrivateProfileStringA(section, key, nullptr, fileName); }
std::vector<string> iniConfig::getSection(LPCSTR section)
{
	std::vector<string> keys;
	if (get(section, nullptr, false))
	{
		LPSTR nextKey = buffer;
		while (*nextKey != '\0')
		{
			keys.push_back(nextKey);
			nextKey = nextKey + strlen(nextKey) + 1;
		}
	}
	return keys;
}

template <typename T>
T printError(LPCSTR section, LPCSTR key, T defValue)
{
	logFile << "Config: " << section << "->" << key << " has invalid value, using default (" << defValue << ")" << std::endl;
	return defValue;
}

string iniConfig::getStr(LPCSTR section, LPCSTR key, string defValue)
{
	if (get(section, key, defValue.empty()))
		return buffer;
	return printError(section, key, defValue);
}

int iniConfig::getInt(LPCSTR section, LPCSTR key, int defValue)
{
	try
	{
		if (get(section, key))
			return std::stoi(buffer, nullptr, 0);
	}
	catch (...) {}
	return printError(section, key, defValue);
}

unsigned int iniConfig::getUInt(LPCSTR section, LPCSTR key, unsigned defValue)
{
	try
	{
		if (get(section, key))
			return std::stoul(buffer, nullptr, 0);
	}
	catch (...) {}
	return printError(section, key, defValue);
}

float iniConfig::getFloat(LPCSTR section, LPCSTR key, float defValue)
{
	try
	{
		if (get(section, key))
			return std::stof(buffer, nullptr);
	}
	catch (...) {}
	return printError(section, key, defValue);
}

double iniConfig::getDouble(LPCSTR section, LPCSTR key, double defValue)
{
	try
	{
		if (get(section, key))
			return std::stod(buffer, nullptr);
	}
	catch (...) {}
	return printError(section, key, defValue);
}

bool iniConfig::getBool(LPCSTR section, LPCSTR key, bool defValue)
{
	try
	{
		if (get(section, key))
		{
			if (_stricmp("true", buffer) == 0 || _stricmp("on", buffer) == 0)
				return true;
			if (_stricmp("false", buffer) == 0 || _stricmp("off", buffer) == 0)
				return false;
		}
	}
	catch (...) {}
	return printError(section, key, defValue);
}

int iniConfig::getEnum(LPCSTR section, LPCSTR key, int defValue, std::pair<int, LPCSTR> map[], int size)
{
	try
	{
		if (get(section, key))
		{
			for (int i = 0; i < size; i++)
				if (_stricmp(map[i].second, buffer) == 0)
					return map[i].first;
		}
	}
	catch (...) {}
	return printError(section, key, defValue);
}

std::vector<int> iniConfig::getInts(LPCSTR section, LPCSTR key)
{
	try
	{
		if (get(section, key, true))
		{
			std::vector<int> list;
			char *context, *token = strtok_s(buffer, ";", &context);
			while (token != nullptr)
			{
				list.push_back(std::stoi(token));
				token = strtok_s(nullptr, ";", &context);
			}
			return list;
		}
	}
	catch (...) {}
	printError(section, key, string());
	return std::vector<int>();
}

std::vector<float> iniConfig::getFloats(LPCSTR section, LPCSTR key)
{
	try
	{
		if (get(section, key, true))
		{
			std::vector<float> list;
			char *context, *token = strtok_s(buffer, ";", &context);
			while (token != nullptr)
			{
				list.push_back(std::stof(token));
				token = strtok_s(nullptr, ";", &context);
			}
			return list;
		}
	}
	catch (...) {}
	printError(section, key, string());
	return std::vector<float>();
}

void iniConfig::setStr(LPCSTR section, LPCSTR key, string value) const
{
	WritePrivateProfileStringA(section, key, (" " + value).c_str(), fileName);
}

void iniConfig::setInt(LPCSTR section, LPCSTR key, int value) const { setStr(section, key, std::to_string(value)); }
void iniConfig::setUInt(LPCSTR section, LPCSTR key, unsigned value, bool hex) const
{
	if (hex)
	{
		char buf[16];
		snprintf(buf, sizeof buf, "0x%08X", value);
		setStr(section, key, buf);
	}
	else
		setStr(section, key, std::to_string(value));
}

void iniConfig::setFloat(LPCSTR section, LPCSTR key, float value) const { setStr(section, key, std::to_string(value)); }
void iniConfig::setDouble(LPCSTR section, LPCSTR key, double value) const { setStr(section, key, std::to_string(value)); }
void iniConfig::setBool(LPCSTR section, LPCSTR key, bool value) const { setStr(section, key, value ? "on" : "off"); };
void iniConfig::setEnum(LPCSTR section, LPCSTR key, int value, std::pair<int, LPCSTR> map[], int size) const
{
	for (int i = 0; i < size; i++)
		if (map[i].first == value)
		{
			setStr(section, key, map[i].second);
			return;
		}
	setStr(section, key, std::to_string(value));
}

void iniConfig::setInts(LPCSTR section, LPCSTR key, std::vector<int> list) const
{
	string str;
	for (size_t i = 0; i < list.size(); i++)
	{
		str += std::to_string(list[i]);
		if (i < list.size() - 1)
			str += ";";
	}
	setStr(section, key, str);
}

void iniConfig::setFloats(LPCSTR section, LPCSTR key, std::vector<float> list) const
{
	string str;
	for (size_t i = 0; i < list.size(); i++)
	{
		str += std::to_string(list[i]);
		if (i < list.size() - 1)
			str += ";";
	}
	setStr(section, key, str);
}