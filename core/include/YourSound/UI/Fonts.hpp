#pragma once

#include "YourSound/Config.hpp"
#include "YourSound/CoreExportImport.hpp"
#include "YourSound/UI/Controls.hpp"
#include "YourSound/UI/Icons.hpp"

#include <imgui.h>

namespace YourSound::UI {
	inline ImFont *g_imgui_standard_font_medium;
	inline ImFont *g_imgui_standard_font_large;
	inline ImFont *g_imgui_bold_font;
	inline ImFont *g_imgui_bold_font_large;
	inline ImFont *g_imgui_icon_font;
	inline ImFont *g_imgui_icon_font_large;
	inline ImFont *g_imgui_mono_font;
	inline ImFont *g_imgui_mono_font_bold;

	static constexpr ImWchar ICONS_RANGES[] = {0xe000, 0xf8ff, 0};

	inline YS_API ImFont *get_font(const ysbp_ui_Font font) {
		switch (font) {
		case ysbp_ui_FONT_STANDARD_LARGE: return g_imgui_standard_font_large;
		case ysbp_ui_FONT_ICON: return g_imgui_icon_font;
		case ysbp_ui_FONT_ICON_LARGE: return g_imgui_icon_font_large;
		case ysbp_ui_FONT_MONO: return g_imgui_mono_font;
		case ysbp_ui_FONT_MONO_BOLD: return g_imgui_mono_font_bold;
		case ysbp_ui_FONT_STANDARD_MEDIUM: return g_imgui_standard_font_medium;
		case ysbp_ui_FONT_BOLD: return g_imgui_bold_font;
		case ysbp_ui_FONT_BOLD_LARGE: return g_imgui_bold_font_large;
		}

		return nullptr;
	}

	inline YS_API void imgui_load_fonts(const ImGuiIO &io) {
		const std::filesystem::path exe_dir = get_current_exe_directory();

		io.Fonts->AddFontFromFileTTF((exe_dir / "font/FiraSans-Regular.ttf").generic_string().c_str());
		g_imgui_standard_font_large =
			io.Fonts->AddFontFromFileTTF((exe_dir / "font/FiraSans-Regular.ttf").generic_string().c_str(), 64.f);
		g_imgui_standard_font_medium =
			io.Fonts->AddFontFromFileTTF((exe_dir / "font/FiraSans-Regular.ttf").generic_string().c_str(), 32.f);

		g_imgui_bold_font =
			io.Fonts->AddFontFromFileTTF((exe_dir / "font/FiraSans-SemiBold.ttf").generic_string().c_str());
		g_imgui_bold_font_large =
			io.Fonts->AddFontFromFileTTF((exe_dir / "font/FiraSans-SemiBold.ttf").generic_string().c_str(), 64.f);

		ImFontConfig config;
		config.MergeMode = true;
		config.GlyphMinAdvanceX = 13.0f;
		g_imgui_icon_font = io.Fonts->AddFontFromFileTTF((exe_dir / FONT_ICON_FILE_NAME_FAS).generic_string().c_str(),
														 16.f, &config, ICONS_RANGES);
		g_imgui_icon_font_large = io.Fonts->AddFontFromFileTTF(
			(exe_dir / FONT_ICON_FILE_NAME_FAS).generic_string().c_str(), 64.f, &config, ICONS_RANGES);

		g_imgui_mono_font =
			io.Fonts->AddFontFromFileTTF((exe_dir / "font/FiraCode-Regular.ttf").generic_string().c_str());
		g_imgui_mono_font_bold =
			io.Fonts->AddFontFromFileTTF((exe_dir / "font/FiraCode-SemiBold.ttf").generic_string().c_str());

		YS_RUNTIME_ASSERT(g_imgui_standard_font_medium && g_imgui_standard_font_large && g_imgui_bold_font &&
							  g_imgui_bold_font_large && g_imgui_icon_font && g_imgui_icon_font_large &&
							  g_imgui_mono_font && g_imgui_mono_font_bold,
						  std::runtime_error, "Could not load all fonts");
	}
} // namespace YourSound::UI
