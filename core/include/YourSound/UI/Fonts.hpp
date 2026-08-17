#pragma once

#include "YourSound/UI/Icons.hpp"

#include <imgui.h>

namespace YourSound::UI {
	inline ImFont *g_imgui_standard_font_medium;
	inline ImFont *g_imgui_standard_font_large;
	inline ImFont *g_imgui_icon_font;
	inline ImFont *g_imgui_icon_font_large;
	inline ImFont *g_imgui_mono_font;
	inline ImFont *g_imgui_mono_font_bold;

	static constexpr ImWchar ICONS_RANGES[] = {0xe000, 0xf8ff, 0};

	inline YS_API void imgui_load_fonts(const ImGuiIO &io) {
		io.Fonts->AddFontFromFileTTF("font/FiraSans-Regular.ttf");
		g_imgui_standard_font_large = io.Fonts->AddFontFromFileTTF("font/FiraSans-Regular.ttf", 64.f);
		g_imgui_standard_font_medium = io.Fonts->AddFontFromFileTTF("font/FiraSans-Regular.ttf", 32.f);

		ImFontConfig config;
		config.MergeMode = true;
		config.GlyphMinAdvanceX = 13.0f;
		g_imgui_icon_font = io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FAS, 16.f, &config, ICONS_RANGES);
		g_imgui_icon_font_large = io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FAS, 64.f, &config, ICONS_RANGES);

		g_imgui_mono_font = io.Fonts->AddFontFromFileTTF("font/FiraCode-Regular.ttf");
		g_imgui_mono_font_bold = io.Fonts->AddFontFromFileTTF("font/FiraCode-SemiBold.ttf");
	}
} // namespace YourSound::UI
