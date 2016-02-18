#include "stdafx.h"
#include "InGameClock.h"
#include "d3d9.h"

RECT outLeft, outTop, outRight, outBottom, rect;
LPD3DXFONT pFont;
LPD3DXSPRITE pSprite;
char clockFont[64];
bool clockEnabled, clockReload;
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
		CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, clockFont, &pFont);
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
	if (clockEnabled && pBase && *pBase)
	{
		DWORD h = *GetBasePtr<UINT32>(0xB876C);
		DWORD m = *GetBasePtr<UINT32>(0xB8770);
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

		if (clockReload)
		{
			clockReload = false;
			pFont->Release();
			D3DXCreateFontA(pD3DDevice, clockSize, 0, FW_HEAVY, 1, FALSE, DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS,
				CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, clockFont, &pFont);
		}
	}
}

void Hooks::InGameClockSwitch() { clockEnabled = !clockEnabled; }
void Hooks::InGameClockInc(BYTE minutes)
{
	*GetBasePtr<UINT32>(0xB8768) += minutes * 60000;
}

void Hooks::InGameClockDec(BYTE minutes)
{
	DWORD time = *GetBasePtr<UINT32>(0xB8768);
	if (time < minutes * 60000U)
	{
		if (*GetBasePtr<UINT32>(0xB8760) > 0)
			(*GetBasePtr<UINT32>(0xB8760))--;
		time += 3600000 * 24;
	}
	*GetBasePtr<UINT32>(0xB8768) = time - minutes * 60000;
}

std::pair<int, LPCSTR> clockPosVMap[] = { { DT_TOP, "top"}, { DT_BOTTOM, "bottom" } };
std::pair<int, LPCSTR> clockPosHMap[] = { { DT_LEFT, "left" }, { DT_CENTER, "center" }, { DT_RIGHT, "right" } };
void renderClockColor(LPCSTR label, LPCSTR key, LPDWORD color)
{
	float s = 1.0f / 255.0f;
	float fColor[4] =
	{
		(*color >> 16 & 0xFF) * s,
		(*color >> 8 & 0xFF) * s,
		(*color >> 0 & 0xFF) * s,
		(*color >> 24 & 0xFF) * s
	};
	if (ImGui::ColorEdit4(label, fColor))
	{
		*color = (int)(fColor[0] * 255.0f + 0.5f) << 16 |
			(int)(fColor[1] * 255.0f + 0.5f) << 8 |
			(int)(fColor[2] * 255.0f + 0.5f) << 0 |
			(int)(fColor[3] * 255.0f + 0.5f) << 24;
		config.setUInt("d3d9", key, *color, true);
	}
}

void renderClockUI()
{
	if (ImGui::CollapsingHeader("Clock"))
	{
		if (ImGui::Checkbox("Enabled", &clockEnabled))
			config.setBool("d3d9", "inGameClock", clockEnabled);

		if (ImGui::DragInt("Size", (int*)&clockSize, 1.0f, 1, 1024))
		{
			config.setUInt("d3d9", "inGameClockSize", clockSize);
			clockReload = true;
		}

		if (ImGui::InputText("Font", clockFont, sizeof clockFont))
		{
			config.setStr("d3d9", "inGameClockFont", clockFont);
			clockReload = true;
		}

		if (ImGui::DragInt("Timebase", (int*)&clockTimebase, 1.0f, 1, 24 * 60))
			config.setUInt("d3d9", "inGameClockTimebase", clockTimebase);
		if (ImGui::RadioButtons(&clockPositionV, clockPosVMap))
			config.setEnum("d3d9", "inGameClockPositionVertical", clockPositionV, clockPosVMap, 2);
		if (ImGui::RadioButtons(&clockPositionH, clockPosHMap))
			config.setEnum("d3d9", "inGameClockPositionHorizontal", clockPositionH, clockPosHMap, 3);

		renderClockColor("Color", "inGameClockColor", &clockColor);
		renderClockColor("Outline Left", "inGameClockOutlineLeft", &clockLeft);
		renderClockColor("Outline Top", "inGameClockOutlineTop", &clockTop);
		renderClockColor("Outline Right", "inGameClockOutlineRight", &clockRight);
		renderClockColor("Outline Bottom", "inGameClockOutlineBottom", &clockBottom);
	}
}

void Hooks::InGameClock()
{
	strncpy_s(clockFont, config.getStr("d3d9", "inGameClockFont", "Arial").c_str(), _TRUNCATE);
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

	clockEnabled = config.getBool("d3d9", "inGameClock", false);
	if (!clockEnabled)
		logFile << "InGameClock: disabled" << std::endl;

	D3D9Add(onCreateDevice, onLostDevice, onResetDevice, onEndScene);
	InGameUIAdd(renderClockUI);
}
