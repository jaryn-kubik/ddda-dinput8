#include "dinput8.h"
#include "InGameClock.h"
#include "d3d9.h"
#include "TweakBar.h"

DWORD **pClock;
RECT outLeft, outTop, outRight, outBottom, rect;
LPD3DXFONT pFont, pNewFont = nullptr;
string clockFont;
bool clockShow = true;
DWORD clockTimebase, clockSize, clockColor, clockLeft, clockTop, clockRight, clockBottom, clockPositionV, clockPositionH;
void setRectangles(LONG width, LONG height)
{
	outLeft = { -1, 0, width - 1, height };
	outTop = { 0, -1, width, height - 1 };
	outRight = { 1, 0, width + 1, height };
	outBottom = { 0, 1, width, height + 1 };
	rect = { 0, 0, width, height };
}

void onCreateDevice(LPDIRECT3DDEVICE9 pD3DDevice, D3DPRESENT_PARAMETERS* pParams)
{
	setRectangles(pParams->BackBufferWidth, pParams->BackBufferHeight);
	D3DXCreateFontA(pD3DDevice, clockSize, 0, FW_HEAVY, 1, FALSE, DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS,
		CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, clockFont.c_str(), &pFont);
}

void onLostDevice(LPDIRECT3DDEVICE9 pD3DDevice, D3DPRESENT_PARAMETERS* pParams)
{
	setRectangles(pParams->BackBufferWidth, pParams->BackBufferHeight);
	pFont->OnLostDevice();
}

void onResetDevice(LPDIRECT3DDEVICE9 pD3DDevice, D3DPRESENT_PARAMETERS* pParams)
{
	setRectangles(pParams->BackBufferWidth, pParams->BackBufferHeight);
	pFont->OnResetDevice();
}

void onEndScene(LPDIRECT3DDEVICE9 pD3DDevice)
{
	if (clockShow && pClock && *pClock)
	{
		DWORD t = (*pClock)[0xB876C / 4] * 60 + (*pClock)[0xB8770 / 4];
		t -= t % clockTimebase;

		WCHAR buf[9];
		wsprintf(buf, L"%02d:%02d", t / 60, t % 60);
		DWORD format = DT_NOCLIP | clockPositionV | clockPositionH;
		pFont->DrawText(nullptr, buf, -1, &outLeft, format, clockLeft);
		pFont->DrawText(nullptr, buf, -1, &outTop, format, clockTop);
		pFont->DrawText(nullptr, buf, -1, &outRight, format, clockRight);
		pFont->DrawText(nullptr, buf, -1, &outBottom, format, clockBottom);
		pFont->DrawText(nullptr, buf, -1, &rect, format, clockColor);

		if (pNewFont)
		{
			pFont->Release();
			pFont = pNewFont;
			pNewFont = nullptr;
		}
	}
}

void Hooks::InGameClockSwitch() { clockShow = !clockShow; }
void Hooks::InGameClockInc(BYTE minutes)
{
	if (pClock && *pClock)
		(*pClock)[0xB8768 / 4] += minutes * 60000;
}

void Hooks::InGameClockDec(BYTE minutes)
{
	if (pClock && *pClock)
	{
		DWORD time = (*pClock)[0xB8768 / 4];
		if (time < minutes * 60000)
		{
			if ((*pClock)[0xB8760 / 4] > 0)
				(*pClock)[0xB8760 / 4]--;
			time += 3600000 * 24;
		}
		(*pClock)[0xB8768 / 4] = time - minutes * 60000;
	}
}

void setInGameClock(const void *value, void *clientData)
{
	if (clientData == &clockSize)
		clockSize = *(LPDWORD)value;
	else if (clientData == &clockFont)
		clockFont = *static_cast<const string*>(value);

	LPDIRECT3DDEVICE9 device;
	pFont->GetDevice(&device);
	D3DXCreateFontA(device, clockSize, 0, FW_HEAVY, 1, FALSE, DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS,
		CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, clockFont.c_str(), &pNewFont);
}

void getInGameClock(void *value, void *clientData)
{
	if (clientData == &clockSize)
		*(LPDWORD)value = clockSize;
	else if (clientData == &clockFont)
		TwCopyStdStringToLibrary(*static_cast<string*>(value), clockFont);
}

void addInGameClock(TwBar *bar)
{
	TwAddVarRW(bar, "enabled", TW_TYPE_BOOLCPP, &clockShow, "group='In-Game clock'");
	TwAddVarCB(bar, "size", TW_TYPE_UINT16, setInGameClock, getInGameClock, &clockSize, "group='In-Game clock' min=1");
	TwAddVarCB(bar, "font", TW_TYPE_STDSTRING, setInGameClock, getInGameClock, &clockFont, "group='In-Game clock'");
	TwAddVarRW(bar, "timebase", TW_TYPE_UINT16, &clockTimebase, "group='In-Game clock'");
	TwAddVarRW(bar, "positionV", TwDefineEnum("positionV", nullptr, 0), &clockPositionV, "group='In-Game clock' enum='0 {top}, 8 {bottom}'");
	TwAddVarRW(bar, "positionH", TwDefineEnum("positionH", nullptr, 0), &clockPositionH, "group='In-Game clock' enum='0 {left}, 1 {center}, 2 {right}'");
	TwAddVarRW(bar, "color", TW_TYPE_COLOR32, &clockColor, "group='In-Game clock' alpha=true");
	TwAddVarRW(bar, "outlineLeft", TW_TYPE_COLOR32, &clockLeft, "group='In-Game clock' alpha=true");
	TwAddVarRW(bar, "outlineTop", TW_TYPE_COLOR32, &clockTop, "group='In-Game clock' alpha=true");
	TwAddVarRW(bar, "outlineRight", TW_TYPE_COLOR32, &clockRight, "group='In-Game clock' alpha=true");
	TwAddVarRW(bar, "outlineBottom", TW_TYPE_COLOR32, &clockBottom, "group='In-Game clock' alpha=true");
}

void Hooks::InGameClock()
{
	if (config.getBool(L"d3d9", L"inGameClock", false))
	{
		clockFont = config.getStrA(L"d3d9", L"inGameClockFont", "Arial");
		clockSize = config.getUInt(L"d3d9", L"inGameClockSize", 30);
		clockTimebase = config.getUInt(L"d3d9", L"inGameClockTimebase", 1);
		if (clockTimebase < 1 || clockTimebase > 60 * 24)
			clockTimebase = 1;

		clockColor = config.getUInt(L"d3d9", L"inGameClockColor", 0xFF050505);
		clockLeft = config.getUInt(L"d3d9", L"inGameClockOutlineLeft", 0xFF222222);
		clockTop = config.getUInt(L"d3d9", L"inGameClockOutlineTop", 0xFF444444);
		clockRight = config.getUInt(L"d3d9", L"inGameClockOutlineRight", 0xFF666666);
		clockBottom = config.getUInt(L"d3d9", L"inGameClockOutlineBottom", 0xFF888888);

		clockPositionV = config.getEnum(L"d3d9", L"inGameClockPositionVertical", DT_TOP,
		{ { L"top", DT_TOP }, { L"bottom", DT_BOTTOM } });
		clockPositionH = config.getEnum(L"d3d9", L"inGameClockPositionHorizontal", DT_RIGHT,
		{ { L"left", DT_LEFT }, { L"center", DT_CENTER }, { L"right", DT_RIGHT } });

		BYTE sig[] = { 0x8B, 0x15, 0xCC, 0xCC, 0xCC, 0xCC, 0x33, 0xDB, 0x8B, 0xF8 };
		BYTE *pOffset;
		if (FindSignature("InGameClock", sig, &pOffset))
		{
			pClock = (DWORD**)*(LPDWORD)(pOffset + 2);
			D3D9Add(onCreateDevice, onLostDevice, onResetDevice, onEndScene);
			TweakBarAdd(addInGameClock);
		}
	}
	else
		logFile << "InGameClock: disabled" << std::endl;
}
