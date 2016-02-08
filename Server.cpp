#include "Server.h"
#include "dinput8.h"

std::string host;
uint16_t port;

typedef uint32_t(*tConnnect)(void* pProto, char* host, uint16_t port);
tConnnect oConnect;

uint32_t __declspec(naked) HConnect()
{
	__asm {
		push ebp
		mov ebp, esp
		sub esp, __LOCAL_SIZE
	}
	
	const char* hostArg;
	hostArg  = host.c_str();

	_asm {		
		push port     // push dword ptr[ebp + 0x04] // port		
		push hostArg; // push dword ptr[ebp + 0x08] // host
		call oConnect
	}

	_asm {
		mov esp, ebp
		pop ebp
		ret 8
	}
}

void Hooks::Server()
{
	if (config.getBool(L"server", L"enabled", false))
	{
		logFile << "Server: enabled" << std::endl;

		host = config.getStrA(L"server", L"host", "dune.dragonsdogma.com");
		port = config.getUInt(L"server", L"port", 12501);

		BYTE signature[] = {
			0x83, 0xEC, 0x24,             // sub     esp, 24h
			0x53,                         // push    ebx
			0x66, 0x8B, 0x5C, 0x24, 0x30, // mov     bx, [esp+28h+port]
			0x56,                         // push    esi
			0x68                          // push    offset nulls_string
		};		

		BYTE *pOffset;
		if (!FindSignature("sDDCaProto::connect", signature, &pOffset))
		{
			return;
		}

		CreateHook("sDDCaProto::connect", pOffset, &HConnect, &oConnect);
	}
	else
	{
		logFile << "Server: disabled" << std::endl;
	}
}