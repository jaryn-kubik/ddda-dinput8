#pragma once

namespace Hooks
{
	extern LPBYTE pItem;
	void ItemEditor();

	extern const std::vector<std::pair<int, LPCSTR>> ListItemStarType;
	extern const std::vector<std::pair<int, LPCSTR>> ListItemMaster;
	extern const std::vector<std::pair<int, LPCSTR>> ListItemEnchant;
	extern const std::vector<std::pair<int, LPCSTR>> ListItemBonus;
	extern const std::vector<std::pair<int, LPCSTR>> ListItemId;
};