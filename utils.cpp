#include "utils.h"

nlohmann::json utils::LoadSettings()
{
	std::string err;
	auto file = std::ifstream("dinput8.json", std::ios_base::in | std::ios::binary);
	if (!file.fail())
	{
		try
		{
			nlohmann::json result = nlohmann::json::parse(file);
			if (result.is_object())
				return result;
		}
		catch (const std::exception& ex) { err = ex.what(); }
		catch (const std::string& ex) { err = ex; }
		catch (...) { err = "unknown error"; }
	}
	else
		err = strerror(errno);


	logFile << "Settings failed to open: " << err.c_str() << std::endl;
	std::ofstream("dinput8.json", std::ios_base::out | std::ios::trunc) << getDefault().dump(4);
	return getDefault();
}

bool init = true;
BYTE *codeBase, *codeEnd;
bool utils::FindSignature(BYTE *signature, size_t len, BYTE **offset)
{
	if (init)
	{
		DWORD base = (DWORD)GetModuleHandle(nullptr);
		auto idh = (PIMAGE_DOS_HEADER)base;
		auto inh = (PIMAGE_NT_HEADERS)(base + idh->e_lfanew);
		auto ioh = &inh->OptionalHeader;
		codeBase = (BYTE*)(base + ioh->BaseOfCode);
		codeEnd = codeBase + ioh->SizeOfCode;
		init = false;
	}

	for (*offset = codeBase; *offset < codeEnd; (*offset)++)
	{
		for (unsigned int i = 0; i < len; i++)
		{
			if (signature[i] != 0xCC && signature[i] != (*offset)[i])
				break;
			if (i == len - 1)
				return true;
		}
	}
	return false;
}