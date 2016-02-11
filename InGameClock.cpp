#include "stdafx.h"
#include "InGameClock.h"
#include "d3d9.h"

DWORD **pClock;
RECT outLeft, outTop, outRight, outBottom, rect;
LPD3DXFONT pFont, pNewFont = nullptr;
LPD3DXSPRITE pSprite;
string clockFont;
bool clockEnabled;
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
	D3DXCreateSprite(pD3DDevice, &pSprite);
}

void onLostDevice(LPDIRECT3DDEVICE9 pD3DDevice, D3DPRESENT_PARAMETERS* pParams)
{
	setRectangles(pParams->BackBufferWidth, pParams->BackBufferHeight);
	pFont->OnLostDevice();
	pSprite->OnLostDevice();
}

void onResetDevice(LPDIRECT3DDEVICE9 pD3DDevice, D3DPRESENT_PARAMETERS* pParams)
{
	setRectangles(pParams->BackBufferWidth, pParams->BackBufferHeight);
	pFont->OnResetDevice();
	pSprite->OnResetDevice();
}

CHAR clockBuf[] = "00:00";
void onEndScene(LPDIRECT3DDEVICE9 pD3DDevice)
{
	if (clockEnabled && pClock && *pClock)
	{
		DWORD h = (*pClock)[0xB876C / 4];
		DWORD m = (*pClock)[0xB8770 / 4];
		if (clockTimebase != 1)
		{
			DWORD t = h * 60 + m;
			t -= t % clockTimebase;
			h = t / 60;
			m = t % 60;
		}

		clockBuf[0] = (CHAR)(0x30 + h / 10);
		clockBuf[1] = (CHAR)(0x30 + h % 10);
		clockBuf[3] = (CHAR)(0x30 + m / 10);
		clockBuf[4] = (CHAR)(0x30 + m % 10);
		DWORD format = DT_NOCLIP | clockPositionV | clockPositionH;

		pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE | D3DXSPRITE_DO_NOT_ADDREF_TEXTURE);
		pFont->DrawText(pSprite, clockBuf, -1, &outLeft, format, clockLeft);
		pFont->DrawText(pSprite, clockBuf, -1, &outTop, format, clockTop);
		pFont->DrawText(pSprite, clockBuf, -1, &outRight, format, clockRight);
		pFont->DrawText(pSprite, clockBuf, -1, &outBottom, format, clockBottom);
		pFont->DrawText(pSprite, clockBuf, -1, &rect, format, clockColor);
		pSprite->End();

		if (pNewFont)
		{
			pFont->Release();
			pFont = pNewFont;
			pNewFont = nullptr;
		}
	}
}

void Hooks::InGameClockSwitch() { clockEnabled = !clockEnabled; }
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
		if (time < minutes * 60000U)
		{
			if ((*pClock)[0xB8760 / 4] > 0)
				(*pClock)[0xB8760 / 4]--;
			time += 3600000 * 24;
		}
		(*pClock)[0xB8768 / 4] = time - minutes * 60000;
	}
}

std::pair<int, LPCSTR> clockPosVMap[] = { { DT_TOP, "top"}, { DT_BOTTOM, "bottom" } };
std::pair<int, LPCSTR> clockPosHMap[] = { { DT_LEFT, "left" }, { DT_CENTER, "center" }, { DT_RIGHT, "right" } };
void setClock(const void *value, void *clientData)
{
	if (clientData == &clockEnabled)
		config.setBool("d3d9", "inGameClock", clockEnabled = *(bool*)value);
	else if (clientData == &clockSize)
		config.setUInt("d3d9", "inGameClockSize", clockSize = *(UINT32*)value);
	else if (clientData == &clockFont)
		config.setStr("d3d9", "inGameClockFont", clockFont = *static_cast<const string*>(value));
	else if (clientData == &clockTimebase)
		config.setUInt("d3d9", "inGameClockTimebase", clockTimebase = *(UINT32*)value);
	else if (clientData == &clockPositionV)
		config.setEnum("d3d9", "inGameClockPositionVertical", clockPositionV = *(UINT32*)value, clockPosVMap, 2);
	else if (clientData == &clockPositionH)
		config.setEnum("d3d9", "inGameClockPositionHorizontal", clockPositionH = *(UINT32*)value, clockPosHMap, 3);
	else if (clientData == &clockColor)
		config.setUInt("d3d9", "inGameClockColor", clockColor = *(UINT32*)value, true);
	else if (clientData == &clockLeft)
		config.setUInt("d3d9", "inGameClockOutlineLeft", clockLeft = *(UINT32*)value, true);
	else if (clientData == &clockTop)
		config.setUInt("d3d9", "inGameClockOutlineTop", clockTop = *(UINT32*)value, true);
	else if (clientData == &clockRight)
		config.setUInt("d3d9", "inGameClockOutlineRight", clockRight = *(UINT32*)value, true);
	else if (clientData == &clockBottom)
		config.setUInt("d3d9", "inGameClockOutlineBottom", clockBottom = *(UINT32*)value, true);

	if (clientData == &clockSize || clientData == &clockFont)
	{
		LPDIRECT3DDEVICE9 device;
		pFont->GetDevice(&device);
		D3DXCreateFontA(device, clockSize, 0, FW_HEAVY, 1, FALSE, DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS,
			CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, clockFont.c_str(), &pNewFont);
	}
}

void getClock(void *value, void *clientData)
{
	if (clientData == &clockEnabled)
		*(bool*)value = clockEnabled;
	else if (clientData == &clockFont)
		TwCopyStdStringToLibrary(*static_cast<string*>(value), clockFont);
	else
		*(UINT32*)value = *(UINT32*)clientData;
}

void addInGameClock(TwBar *bar)
{
	TwEnumVal clockPosVEV[] = { { DT_TOP, "top" }, { DT_BOTTOM, "bottom" } };
	TwEnumVal clockPosHEV[] = { { DT_LEFT, "left" }, { DT_CENTER, "center" }, { DT_RIGHT, "right" } };
	TwType clockPosV = TwDefineEnum("clockPosVEnum", clockPosVEV, 2);
	TwType clockPosH = TwDefineEnum("clockPosHEnum", clockPosHEV, 3);

	TwAddVarCB(bar, "clockEnabled", TW_TYPE_BOOLCPP, setClock, getClock, &clockEnabled, "group=Clock label=Enabled");
	TwAddVarCB(bar, "clockSize", TW_TYPE_UINT32, setClock, getClock, &clockSize, "group=Clock min=1 label=Size");
	TwAddVarCB(bar, "clockFont", TW_TYPE_STDSTRING, setClock, getClock, &clockFont, "group=Clock label=Font");
	TwAddVarCB(bar, "clockTimebase", TW_TYPE_UINT32, setClock, getClock, &clockTimebase, "group=Clock min=1 label=Timebase");
	TwAddVarCB(bar, "clockPosV", clockPosV, setClock, getClock, &clockPositionV, "group=Clock label=Vertical");
	TwAddVarCB(bar, "clockPosH", clockPosH, setClock, getClock, &clockPositionH, "group=Clock label=Horizontal");
	TwAddVarCB(bar, "clockColor", TW_TYPE_COLOR32, setClock, getClock, &clockColor, "group=Clock alpha=true label=Color");
	TwAddVarCB(bar, "clockOutlineL", TW_TYPE_COLOR32, setClock, getClock, &clockLeft, "group=Clock alpha=true label='Outline Left'");
	TwAddVarCB(bar, "clockOutlineT", TW_TYPE_COLOR32, setClock, getClock, &clockTop, "group=Clock alpha=true label='Outline Top'");
	TwAddVarCB(bar, "clockOutlineR", TW_TYPE_COLOR32, setClock, getClock, &clockRight, "group=Clock alpha=true label='Outline Right'");
	TwAddVarCB(bar, "clockOutlineB", TW_TYPE_COLOR32, setClock, getClock, &clockBottom, "group=Clock alpha=true label='Outline Bottom'");
	TwDefine("DDDAFix/Clock opened=false");
}

void Hooks::InGameClock()
{
	clockFont = config.getStr("d3d9", "inGameClockFont", "Arial");
	clockSize = config.getUInt("d3d9", "inGameClockSize", 30);
	clockTimebase = config.getUInt("d3d9", "inGameClockTimebase", 1);
	if (clockTimebase < 1)
		clockTimebase = 1;

	clockColor = config.getUInt("d3d9", "inGameClockColor", 0xFF050505);
	clockLeft = config.getUInt("d3d9", "inGameClockOutlineLeft", 0xFF222222);
	clockTop = config.getUInt("d3d9", "inGameClockOutlineTop", 0xFF444444);
	clockRight = config.getUInt("d3d9", "inGameClockOutlineRight", 0xFF666666);
	clockBottom = config.getUInt("d3d9", "inGameClockOutlineBottom", 0xFF888888);

	clockPositionV = config.getEnum("d3d9", "inGameClockPositionVertical", DT_TOP, clockPosVMap, 2);
	clockPositionH = config.getEnum("d3d9", "inGameClockPositionHorizontal", DT_RIGHT, clockPosHMap, 3);

	BYTE sig[] = { 0x8B, 0x15, 0xCC, 0xCC, 0xCC, 0xCC, 0x33, 0xDB, 0x8B, 0xF8 };
	BYTE *pOffset;
	if (!FindSignature("InGameClock", sig, &pOffset))
		return;

	pClock = (DWORD**)*(LPDWORD)(pOffset + 2);
	D3D9Add(onCreateDevice, onLostDevice, onResetDevice, onEndScene);
	TweakBarAdd(addInGameClock);

	clockEnabled = config.getBool("d3d9", "inGameClock", false);
	if (!clockEnabled)
		logFile << "InGameClock: disabled" << std::endl;
}
