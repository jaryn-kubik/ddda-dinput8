#pragma once
#include "dinput8.h"

class utils
{
	static BYTE *codeBase, *codeEnd, *dataBase, *dataEnd;
	static bool Find(BYTE* start, BYTE* end, BYTE *signature, size_t len, BYTE **offset, LPCSTR msg = nullptr);

public:
	static void Initialize();

	template<size_t len> static bool Find(BYTE* start, BYTE* end, BYTE(&signature)[len], BYTE **offset, LPCSTR msg = nullptr)
	{ return utils::Find(start, end, signature, len, offset, msg); }

	template<size_t len> static bool FindSignature(BYTE(&signature)[len], BYTE **offset, LPCSTR msg = nullptr)
	{ return utils::Find(codeBase, codeEnd, signature, len, offset, msg); }
	
	template<size_t len> static bool FindData(BYTE(&signature)[len], BYTE **offset, LPCSTR msg = nullptr)
	{ return utils::Find(dataBase, dataEnd, signature, len, offset, msg); }

	template<typename Type>	static void Set(Type *address, Type data)
	{
		DWORD oldProtect;
		VirtualProtect(address, sizeof(Type), PAGE_EXECUTE_READWRITE, &oldProtect);
		*address = data;
		VirtualProtect(address, sizeof(Type), oldProtect, &oldProtect);
	}
};
