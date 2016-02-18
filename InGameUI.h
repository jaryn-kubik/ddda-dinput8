#pragma once

namespace Hooks
{
	void InGameUI();
	LRESULT InGameUIEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void InGameUIAdd(void(*callback)());
};

namespace ImGui
{
	template <class T> bool ListBoxFilter(const char *label, void *v, std::pair<int, LPCSTR> *items, int count, ImGuiTextFilter &filter, bool scroll)
	{
		ListBoxHeader(label, count);
		bool value_changed = false;
		for (int i = 0; i < count; i++)
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
				*(T*)v = i;
				value_changed = true;
			}
			PopID();
		}
		ListBoxFooter();
		return value_changed;
	}

	template<class T> bool ComboEnum(const char *label, void *v, std::pair<int, LPCSTR> *items, int count)
	{
		auto items_getter = [](void* data, int idx, const char **text)
		{
			*text = ((std::pair<int, LPCSTR>*)data)[idx].second;
			return true;
		};

		int currentIndex = -1;
		for (int i = 0; i < count; i++)
			if (items[i].first == *(T*)v)
			{
				currentIndex = i;
				break;
			}
		if (Combo(label, &currentIndex, items_getter, items, count))
		{
			if (currentIndex >= 0 && currentIndex < count)
				*(T*)v = items[currentIndex].first;
			return true;
		}
		return false;
	}

	template<class T, size_t len> bool RadioButtons(void *v, std::pair<T, const char*>(&pairs)[len])
	{
		bool pressed = false;
		for (int i = 0; i < len; i++)
		{
			if (ImGui::RadioButton(pairs[i].second, *(T*)v == pairs[i].first))
			{
				*(T*)v = pairs[i].first;
				pressed |= true;
			}
			if (i < len - 1)
				SameLine();
		}
		return pressed;
	}

	template<class T> bool Drag(const char *label, void *v, float v_speed = 1.0f, float v_min = 0, float v_max = 0, const char* display_format = "%.0f")
	{
		float v_f = (float)*(T*)v;
		if (DragFloat(label, &v_f, v_speed, v_min, v_max, display_format))
		{
			*(T*)v = (T)v_f;
			return true;
		}
		return false;
	}

	template<class T> bool InputScalar(const char* label, void *v, int min, int max = INT_MAX, int step = 1, int step_fast = 100)
	{
		int v_i = (int)*(T*)v;
		if (InputInt(label, &v_i, step, step_fast))
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

	bool InputFloatN(const char* label, float* v, int components, float min, float max = FLT_MAX, int precision = -1);
}