#pragma once
#include "dinput8.h"

namespace Hooks
{
	void Utils();

	bool Find(LPCSTR msg, BYTE* start, BYTE* end, BYTE *signature, size_t len, BYTE **offset);
	bool FindSignature(LPCSTR msg, BYTE *signature, size_t len, BYTE **offset);
	bool FindData(LPCSTR msg, BYTE *signature, size_t len, BYTE **offset);

	template<size_t len> static bool Find(LPCSTR msg, BYTE* start, BYTE* end, BYTE(&signature)[len], BYTE **offset)
	{ return Find(start, end, signature, len, offset, msg); }

	template<size_t len> static bool FindSignature(LPCSTR msg, BYTE(&signature)[len], BYTE **offset)
	{ return FindSignature(msg, signature, len, offset); }
	
	template<size_t len> static bool FindData(LPCSTR msg, BYTE(&signature)[len], BYTE **offset)
	{ return FindData(msg, signature, len, offset); }

	template<typename Type>	static void Set(Type *address, Type data)
	{
		DWORD oldProtect;
		VirtualProtect(address, sizeof(Type), PAGE_EXECUTE_READWRITE, &oldProtect);
		*address = data;
		VirtualProtect(address, sizeof(Type), oldProtect, &oldProtect);
	}
};
