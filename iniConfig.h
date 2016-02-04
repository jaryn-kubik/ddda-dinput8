#pragma once
#include <map>

class iniConfig
{
#ifdef UNICODE
	typedef std::wstring stringType;
#else
	typedef std::string stringType;
#endif

	TCHAR buffer[512];
	LPCTSTR fileName;

	bool get(LPCTSTR section, LPCTSTR key, bool allowEmpty = false);
public:
	iniConfig(LPCTSTR fileName);

	std::string getStrA(LPCTSTR section, LPCTSTR key, std::string defValue);
	std::wstring getStrW(LPCTSTR section, LPCTSTR key, std::wstring defValue);
	int getInt(LPCTSTR section, LPCTSTR key, int defValue);
	unsigned int getUInt(LPCTSTR section, LPCTSTR key, unsigned int defValue);
	float getFloat(LPCTSTR section, LPCTSTR key, float defValue);
	double getDouble(LPCTSTR section, LPCTSTR key, double defValue);
	bool getBool(LPCTSTR section, LPCTSTR key, bool defValue);
	int getEnum(LPCTSTR section, LPCTSTR key, int defValue, std::map<stringType, int> map);
};