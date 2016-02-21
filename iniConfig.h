#pragma once

class iniConfig
{
	CHAR buffer[1024];
	LPCSTR fileName;

	bool get(LPCSTR section, LPCSTR key, bool allowEmpty = false);
public:
	explicit iniConfig(LPCSTR fileName);

	void removeKey(LPCSTR section, LPCSTR key) const;
	std::vector<string> getSection(LPCSTR section);

	string getStr(LPCSTR section, LPCSTR key, string defValue = string());
	int getInt(LPCSTR section, LPCSTR key, int defValue);
	unsigned int getUInt(LPCSTR section, LPCSTR key, unsigned int defValue);
	float getFloat(LPCSTR section, LPCSTR key, float defValue);
	double getDouble(LPCSTR section, LPCSTR key, double defValue);
	bool getBool(LPCSTR section, LPCSTR key, bool defValue);
	int getEnum(LPCSTR section, LPCSTR key, int defValue, std::pair<int, LPCSTR> map[], int size);
	std::vector<int> getList(LPCSTR section, LPCSTR key);

	void setStr(LPCSTR section, LPCSTR key, string value) const;
	void setInt(LPCSTR section, LPCSTR key, int value) const;
	void setUInt(LPCSTR section, LPCSTR key, unsigned int value, bool hex = false) const;
	void setFloat(LPCSTR section, LPCSTR key, float value) const;
	void setDouble(LPCSTR section, LPCSTR key, double value) const;
	void setBool(LPCSTR section, LPCSTR key, bool value) const;
	void setEnum(LPCSTR section, LPCSTR key, int value, std::pair<int, LPCSTR> map[], int size) const;
	void setList(LPCSTR section, LPCSTR key, std::vector<int> list) const;
};