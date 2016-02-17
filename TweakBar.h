#pragma once

namespace Hooks
{
	void TweakBar();
	void TweakBarSwitch();
	LRESULT TweakBarEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void TweakBarAdd(std::function<void(TwBar*)> func);
	void setBase(const void *value, void *clientData);
	void getBase(void *value, void *clientData);
};