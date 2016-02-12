#pragma once

extern std::ofstream logFile;
extern iniConfig config;
extern DWORD **pBase;

namespace Hooks
{
	void CreateHook(LPCSTR msg, LPVOID pTarget, LPVOID pDetour, LPVOID *ppOriginal, bool enable = true);
	void SwitchHook(LPCSTR msg, LPVOID pTarget, bool enable);
	bool Find(LPCSTR msg, BYTE *start, BYTE *end, BYTE *signature, size_t len, BYTE **offset);
	bool FindSignature(LPCSTR msg, BYTE *signature, size_t len, BYTE **offset);
	bool FindData(LPCSTR msg, BYTE *signature, size_t len, BYTE **offset);

	template <typename T>
	void CreateHook(LPCSTR msg, LPVOID pTarget, LPVOID pDetour, T** ppOriginal, bool enable = true)
	{
		CreateHook(msg, pTarget, pDetour, reinterpret_cast<LPVOID*>(ppOriginal), enable);
	}

	template<size_t len> bool Find(LPCSTR msg, BYTE* start, BYTE* end, BYTE(&signature)[len], BYTE **offset)
	{
		return Find(start, end, signature, len, offset, msg);
	}

	template<size_t len> bool FindSignature(LPCSTR msg, BYTE(&signature)[len], BYTE **offset)
	{
		return FindSignature(msg, signature, len, offset);
	}

	template<size_t len> bool FindData(LPCSTR msg, BYTE(&signature)[len], BYTE **offset)
	{
		return FindData(msg, signature, len, offset);
	}

	template<typename T> void Set(T *address, std::initializer_list<T> args)
	{
		DWORD oldProtect;
		VirtualProtect(address, args.size() * sizeof(T), PAGE_EXECUTE_READWRITE, &oldProtect);
		for (auto &arg : args)
			*(address++) = arg;
		VirtualProtect(address, args.size() * sizeof(T), oldProtect, &oldProtect);
	}
}