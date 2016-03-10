#pragma once

extern std::ofstream logFile;
extern iniConfig config;
extern BYTE **pBase;
extern BYTE **pWorld;

template<class T = void> T* GetBasePtr(int offset) { return (T*)(*pBase + offset); }
template<class T = void> T* GetWorldPtr(std::initializer_list<int> offsets)
{
	BYTE **result = pWorld;
	for (auto offset : offsets)
		if (*result)
			result = (BYTE**)(*result + offset);
	return (T*)*result;
}

namespace Hooks
{
	void CreateHook(LPCSTR msg, LPVOID pTarget, LPVOID pDetour, LPVOID *ppOriginal, bool enable = true);
	void SwitchHook(LPCSTR msg, LPVOID pTarget, bool enable);
	bool Find(LPCSTR msg, BYTE *start, BYTE *end, BYTE *signature, size_t len, BYTE **offset);
	bool FindSignature(LPCSTR msg, BYTE *signature, size_t len, BYTE **offset);
	bool FindData(LPCSTR msg, BYTE *signature, size_t len, BYTE **offset);

	template <class T>
	void CreateHook(LPCSTR msg, LPVOID pTarget, LPVOID pDetour, T** ppOriginal, bool enable = true)
	{
		CreateHook(msg, pTarget, pDetour, reinterpret_cast<LPVOID*>(ppOriginal), enable);
	}

	template<size_t len> bool Find(LPCSTR msg, BYTE* start, BYTE* end, BYTE(&signature)[len], BYTE **offset)
	{
		return Find(msg, start, end, signature, len, offset);
	}

	template<size_t len> bool FindSignature(LPCSTR msg, BYTE(&signature)[len], BYTE **offset)
	{
		return FindSignature(msg, signature, len, offset);
	}

	template<size_t len> bool FindData(LPCSTR msg, BYTE(&signature)[len], BYTE **offset)
	{
		return FindData(msg, signature, len, offset);
	}

	template<class T> void Set(T *address, std::initializer_list<T> args)
	{
		DWORD oldProtect;
		VirtualProtect(address, args.size() * sizeof(T), PAGE_EXECUTE_READWRITE, &oldProtect);
		for (auto &arg : args)
			*(address++) = arg;
		VirtualProtect(address, args.size() * sizeof(T), oldProtect, &oldProtect);
	}
}