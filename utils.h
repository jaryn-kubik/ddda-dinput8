#pragma once
#include "dinput8.h"
#include "json.hpp"

namespace utils
{
	inline nlohmann::json getDefault()
	{
		return
		{
			//{ "savePath", nullptr },
			{ "charCustomizationKeys",{ 0x23, 0x24 } }//home + end
		};
	}

	nlohmann::json LoadSettings();

	bool FindSignature(BYTE *signature, size_t len, BYTE **offset);
	template<size_t len> bool Find(BYTE(&signature)[len], BYTE **offset) { return FindSignature(signature, len, offset); }
};
