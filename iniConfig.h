#pragma once

class iniConfig
{
#ifdef UNICODE
	typedef std::wstring stringType;
#else
	typedef std::string stringType;
#endif

	TCHAR buffer[512];
	LPCTSTR fileName;

	bool get(LPCTSTR section, LPCTSTR key);
public:
	iniConfig(LPCTSTR fileName);

	stringType getStr(LPCTSTR section, LPCTSTR key, stringType defValue);
	int getInt(LPCTSTR section, LPCTSTR key, int defValue);
	unsigned int getUInt(LPCTSTR section, LPCTSTR key, unsigned int defValue);
	bool getBool(LPCTSTR section, LPCTSTR key, bool defValue);
	float getFloat(LPCTSTR section, LPCTSTR key, float defValue);
	double getDouble(LPCTSTR section, LPCTSTR key, double defValue);
};