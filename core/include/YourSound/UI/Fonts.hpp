#pragma once

#include "YourSound/UI/Icons.hpp"

#include <imgui.h>

namespace YourSound::UI {
	inline ImFont *g_imgui_icon_font;
	inline ImFont *g_imgui_icon_font_large;

	static constexpr ImWchar ICONS_RANGES[] = { 0xe000, 0xf8ff, 0 };

	inline void imgui_load_fonts(const ImGuiIO &io) {
		g_imgui_icon_font = io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FAS, 16.f, nullptr, ICONS_RANGES);
		g_imgui_icon_font_large = io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FAS, 64.f, nullptr, ICONS_RANGES);
	}
}