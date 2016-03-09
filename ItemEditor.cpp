#include "stdafx.h"
#include "ItemEditor.h"

bool pItemChanged = true;
LPBYTE Hooks::pItem;
LPVOID oItemIDRead;
void __declspec(naked) HItemIDRead()
{
	__asm	mov		dword ptr[Hooks::pItem], ecx;
	__asm	mov		pItemChanged, 1;
	__asm	jmp		oItemIDRead;
}

enum ItemStarType : UINT16
{
	Star0 = 0, Star1 = 1 << 3, Star2 = 1 << 4, Star3 = 1 << 5,
	Star4 = 1 << 6, Star5 = 1 << 9, Star6 = 1 << 10,
	StarMask = Star1 | Star2 | Star3 | Star4 | Star5 | Star6
};

const std::vector<std::pair<int, LPCSTR>> ListItemStarType =
{
	{ Star0, "0 stars" },{ Star1, "1 stars" },{ Star2, "2 stars" },{ Star3, "3 stars" },
	{ Star4, "Dragon forged" },{ Star5, "Silver rarified" },{ Star6, "Gold rarified" }
};

void renderItemEditorUI()
{
	using namespace Hooks;
	if (ImGui::CollapsingHeader("Item Editor") && pItem)
	{
		static ImGuiTextFilter itemFilter;
		itemFilter.Draw("Filter");
		ImGui::ListBoxFilter<UINT16>("Item", pItem + 0x10, ListItemId, itemFilter, pItemChanged);
		pItemChanged = false;

		ImGui::Separator();
		ImGui::InputScalar<UINT16>("Quantity", pItem + 0x12, 1, UINT16_MAX, 1);
		ImGui::InputFloatEx("Weight", *(float**)(pItem + 0x04) + 0x44 / 4, 0.1f, 0.0f, 100.0f, -1);
		UINT16 itemFlags = *(UINT16*)(pItem + 0x20) & StarMask;
		if (ImGui::ComboEnum<UINT16>("Quality", &itemFlags, ListItemStarType))
			*(UINT16*)(pItem + 0x20) = *(UINT16*)(pItem + 0x20) & ~StarMask | itemFlags;

		ImGui::Separator();
		if (ImGui::TreeNode("(for master rings)"))
		{
			ImGui::ComboEnum<UINT8>("Enchant 1", pItem + 0x18, ListItemMaster);
			ImGui::ComboEnum<UINT8>("Enchant 2", pItem + 0x19, ListItemMaster);
			ImGui::InputScalar<UINT16>("Power 1", pItem + 0x1A, 0, UINT16_MAX);
			ImGui::InputScalar<UINT16>("Power 2", pItem + 0x1C, 0, UINT16_MAX);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("(for skill rings)"))
		{
			ImGui::ComboEnum<UINT16>("Enchant 1", pItem + 0x18, ListItemEnchant);
			ImGui::ComboEnum<UINT16>("Enchant 2", pItem + 0x1A, ListItemEnchant);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("(for bbi armor)"))
		{
			ImGui::ComboEnum<UINT16>("Bonus 1", pItem + 0x1C, ListItemBonus);
			ImGui::ComboEnum<UINT16>("Bonus 2", pItem + 0x1E, ListItemBonus);
			ImGui::TreePop();
		}
	}
}

void Hooks::ItemEditor()
{
	BYTE *pOffset;
	BYTE sig2[] = { 0x8B, 0x44, 0x24, 0x04, 0x56, 0x57, 0x8B, 0xF9, 0x85, 0xC0 };
	if (!FindSignature("ItemEditor", sig2, &pOffset))
		return;
	CreateHook("ItemEditor", pOffset, &HItemIDRead, &oItemIDRead);
	InGameUIAdd(renderItemEditorUI);
}