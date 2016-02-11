#include "stdafx.h"
#include "Server.h"

string host;
const char *hostPtr;
UINT16 port;
LPVOID oConnect;

void __declspec(naked) HConnect()
{
	__asm
	{
		add		esp, 12;
		push	port;
		push	hostPtr;
		sub		esp, 4;
		jmp		oConnect;
	}
}

void Hooks::Server()
{
	if (config.getBool(L"server", L"enabled", false))
	{
		host = config.getStrA(L"server", L"host", "dune.dragonsdogma.com");
		hostPtr = host.c_str();
		port = config.getUInt(L"server", L"port", 12501);
		logFile << "Server: using " << host.c_str() << ":" << port << std::endl;

		BYTE signature[] =
		{
			0x83, 0xEC, 0x24,             // sub     esp, 24h
			0x53,                         // push    ebx
			0x66, 0x8B, 0x5C, 0x24, 0x30, // mov     bx, [esp+28h+port]
			0x56,                         // push    esi
			0x68                          // push    offset nulls_string
		};

		BYTE *pOffset;
		if (FindSignature("sDDCaProto::connect", signature, &pOffset))
			CreateHook("sDDCaProto::connect", pOffset, &HConnect, &oConnect);
	}
	else
		logFile << "Server: disabled" << std::endl;
}