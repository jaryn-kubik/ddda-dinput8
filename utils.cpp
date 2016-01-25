#include "utils.h"

BYTE *utils::codeBase, *utils::codeEnd, *utils::dataBase, *utils::dataEnd;
void utils::Initialize()
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

bool utils::Find(BYTE* start, BYTE* end, BYTE *signature, size_t len, BYTE **offset, LPCSTR msg)
{
	for (*offset = start; *offset < end; (*offset)++)
	{
		for (unsigned int i = 0; i < len; i++)
		{
			if (signature[i] != 0xCC && signature[i] != (*offset)[i])
				break;
			if (i == len - 1)
			{
				if (msg)
					logFile << msg << ": " << *offset << std::endl;
				return true;
			}
		}
	}
	if (msg)
		logFile << msg << ": not found" << std::endl;
	return false;
}