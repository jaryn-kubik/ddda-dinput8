#pragma once
#include "dinput8.h"

namespace Hooks
{
	void Utils();

	bool InCodeRange(LPVOID address);
	bool InDataRange(LPVOID address);

	bool Find(LPCSTR msg, BYTE *start, BYTE *end, BYTE *signature, size_t len, BYTE **offset);
	bool FindSignature(LPCSTR msg, BYTE *signature, size_t len, BYTE **offset);
	bool FindData(LPCSTR msg, BYTE *signature, size_t len, BYTE **offset);

	template<size_t len> static bool Find(LPCSTR msg, BYTE* start, BYTE* end, BYTE(&signature)[len], BYTE **offset)
	{ return Find(start, end, signature, len, offset, msg); }

	template<size_t len> static bool FindSignature(LPCSTR msg, BYTE(&signature)[len], BYTE **offset)
	{ return FindSignature(msg, signature, len, offset); }
	
	template<size_t len> static bool FindData(LPCSTR msg, BYTE(&signature)[len], BYTE **offset)
	{ return FindData(msg, signature, len, offset); }

	template<typename T> static void Set(T *address, std::initializer_list<T> args)
	{
		DWORD oldProtect;
		VirtualProtect(address, args.size() * sizeof(T), PAGE_EXECUTE_READWRITE, &oldProtect);
		for (auto &arg : args)
			*(address++) = arg;
		VirtualProtect(address, args.size() * sizeof(T), oldProtect, &oldProtect);
	}
};
