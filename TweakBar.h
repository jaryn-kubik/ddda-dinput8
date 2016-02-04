#pragma once
#include "include/AntTweakBar.h"

namespace Hooks
{
	enum TweakBarEntryType { TweakBarRW, TweakBarRO, TweakBarCB };
	struct TweakBarEntry
	{
		TweakBarEntryType type;
		string name;
		TwType varType;
		LPVOID var;
		string params;
		TwSetVarCallback cbSet;
		TwGetVarCallback cbGet;
	};

	void TweakBar();
	void TweakBarSwitch();
	LRESULT TweakBarEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void TweakBarAdd(TweakBarEntry entry);
};
