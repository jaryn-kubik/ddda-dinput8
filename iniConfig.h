#pragma once

class iniConfig
{
	WCHAR buffer[512];
	LPCWSTR fileName;

	bool get(LPCWSTR section, LPCWSTR key, bool allowEmpty = false);
public:
	iniConfig(LPCWSTR fileName);

	string getStrA(LPCWSTR section, LPCWSTR key, string defValue);
	wstring getStrW(LPCWSTR section, LPCWSTR key, wstring defValue);
	int getInt(LPCWSTR section, LPCWSTR key, int defValue);
	unsigned int getUInt(LPCWSTR section, LPCWSTR key, unsigned int defValue);
	float getFloat(LPCWSTR section, LPCWSTR key, float defValue);
	double getDouble(LPCWSTR section, LPCWSTR key, double defValue);
	bool getBool(LPCWSTR section, LPCWSTR key, bool defValue);
	int getEnum(LPCWSTR section, LPCWSTR key, int defValue, std::map<wstring, int> map);

	void setStrA(LPCWSTR section, LPCWSTR key, string value) const;
	void setStrW(LPCWSTR section, LPCWSTR key, wstring value) const;
	void setInt(LPCWSTR section, LPCWSTR key, int value, bool hex = false) const;
	void setUInt(LPCWSTR section, LPCWSTR key, unsigned int value, bool hex = false) const;
	void setFloat(LPCWSTR section, LPCWSTR key, float value) const;
	void setDouble(LPCWSTR section, LPCWSTR key, double value) const;
	void setBool(LPCWSTR section, LPCWSTR key, bool value) const;
	void setEnum(LPCWSTR section, LPCWSTR key, int value, std::map<wstring, int> map) const;
};