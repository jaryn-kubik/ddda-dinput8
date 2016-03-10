#pragma once

namespace Hooks
{
	bool InGameUI();
	void InGameUIAdd(void(*callback)());
	void InGameUIAddWindow(void(*callback)(bool getsInput));
	void InGameUIAddInit(void(*callback)());
	void InGameUIAddFont(const char *filename, float size_pixels, ImFont **font);
};

namespace ImGui
{
	template <class T> bool ListBoxFilter(const char *label, void *v, const std::vector<std::pair<int, LPCSTR>> &items, ImGuiTextFilter &filter, bool scroll)
	{
		ListBoxHeader(label, items.size());
		bool value_changed = false;
		for (size_t i = 0; i < items.size(); i++)
		{
			const bool item_selected = i == *(T*)v;
			const char* item_text = items[i].second;

			if (!filter.PassFilter(item_text))
				continue;

			if (item_selected && scroll)
				SetScrollHere();

			PushID(i);
			if (Selectable(item_text, item_selected))
			{
				*(T*)v = (T)i;
				value_changed = true;
			}
			PopID();
		}
		ListBoxFooter();
		return value_changed;
	}

	template<class T> bool ComboEnum(const char *label, void *v, const std::vector<std::pair<int, LPCSTR>> &items)
	{
		auto items_getter = [](void* data, int idx, const char **text)
		{
			*text = ((std::pair<int, LPCSTR>*)data)[idx].second;
			return true;
		};

		int currentIndex = -1;
		for (size_t i = 0; i < items.size(); i++)
			if (items[i].first == *(T*)v)
			{
				currentIndex = i;
				break;
			}
		if (Combo(label, &currentIndex, items_getter, (void*)items.data(), items.size()))
		{
			if (currentIndex >= 0 && currentIndex < (int)items.size())
				*(T*)v = items[currentIndex].first;
			return true;
		}
		return false;
	}

	template<class T, size_t len> bool RadioButtons(void *v, std::pair<T, const char*>(&pairs)[len])
	{
		bool pressed = false;
		Columns(len, nullptr, false);
		for (int i = 0; i < len; i++)
		{
			if (ImGui::RadioButton(pairs[i].second, *(T*)v == pairs[i].first))
			{
				*(T*)v = pairs[i].first;
				pressed |= true;
			}
			NextColumn();
		}
		Columns();
		return pressed;
	}

	template<class T> bool InputScalar(const char* label, void *v, int min, int max = INT_MAX, int step = 1, float item_width = -1.0f, ImGuiInputTextFlags flags = 0)
	{
		int v_i = (int)*(T*)v;
		if (item_width > 0.0f)
			PushItemWidth(item_width);
		bool changed = InputInt(label, &v_i, step, step * 10, flags);
		if (item_width > 0.0f)
			PopItemWidth();
		if (changed)
		{
			if (v_i < min)
				v_i = min;
			if (v_i > max)
				v_i = max;
			*(T*)v = (T)v_i;
			return true;
		}
		return false;
	}

	bool InputFloatN(const char* label, float* v, int count, float item_width = 0.0f, float min = 0.0f, float max = FLT_MAX, int precision = 2);
	bool InputFloatEx(const char* label, float* v, float step, float min = -FLT_MAX, float max = FLT_MAX, int precision = 2);
	void TextUnformatted(const char* label, float pos);
}