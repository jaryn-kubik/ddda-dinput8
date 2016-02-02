#include "utils.h"

BYTE *codeBase, *codeEnd, *dataBase, *dataEnd;
void Hooks::Utils()
{
	DWORD base = (DWORD)GetModuleHandle(nullptr);
	auto idh = (PIMAGE_DOS_HEADER)base;
	auto inh = (PIMAGE_NT_HEADERS)(base + idh->e_lfanew);
	auto ioh = &inh->OptionalHeader;
	codeBase = (BYTE*)(base + ioh->BaseOfCode);
	codeEnd = codeBase + ioh->SizeOfCode;
	dataBase = (BYTE*)(base + ioh->BaseOfData);
	dataEnd = dataBase + ioh->SizeOfInitializedData;
}

bool Hooks::InCodeRange(LPVOID address) { return address >= codeBase && address <= codeEnd; }
bool Hooks::InDataRange(LPVOID address) { return address >= dataBase && address <= dataEnd; }
bool Hooks::Find(LPCSTR msg, BYTE* start, BYTE* end, BYTE *signature, size_t len, BYTE **offset)
{
	for (*offset = start; *offset < end; (*offset)++)
	{
		for (unsigned int i = 0; i < len; i++)
		{
			if (signature[i] != 0xCC && signature[i] != (*offset)[i])
				break;
			if (i == len - 1)
			{
				logFile << msg << " pointer: " << *offset << std::endl;
				return true;
			}
		}
	}
	logFile << msg << " pointer: not found" << std::endl;
	return false;
}

bool Hooks::FindSignature(LPCSTR msg, BYTE* signature, size_t len, BYTE** offset)
{
	return Find(msg, codeBase, codeEnd, signature, len, offset);
}

bool Hooks::FindData(LPCSTR msg, BYTE* signature, size_t len, BYTE** offset)
{
	return Find(msg, dataBase, dataEnd, signature, len, offset);
}
