#include "Audio.hpp"
#include "Edit.hpp"
#include "Image.hpp"
#include "MessageBox.hpp"
#include "Renderer.hpp"

#include <YourSound/BinPlayerSTD.hpp>
#include <YourSound/UI/Fonts.hpp>
#include <YourSound/Version.hpp>

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

#include <stacktrace>

enum class QSState : uint8_t { WELCOME, EDIT };

auto state = QSState::WELCOME;
bool show_as_window = false;

void intro_window() {
	ImGui::SetNextWindowPos({0.f, 80.f});
	ImGui::SetNextWindowSize({1280.f, 720.f});
	ImGui::PushStyleColor(ImGuiCol_WindowBg, {0.05f, 0.05f, 0.05f, 1.f});
	ImGui::Begin("Welcome", nullptr,
				 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
					 ImGuiWindowFlags_NoScrollWithMouse);
	ImGui::PopStyleColor();

	{
		static SDL_Texture *texture_qsamp_title_bg = nullptr;

		if (!texture_qsamp_title_bg)
			load_texture_from_file(YourSound::get_current_exe_directory() / "img/qsamp-title-bg.png", g_renderer,
								   &texture_qsamp_title_bg, nullptr, nullptr);

		ImGui::SetCursorPos({0.f, 0.f});
		ImGui::Image(texture_qsamp_title_bg, {1280.f, 720.f});
	}

	{
		static SDL_Texture *texture_qsamp96 = nullptr;

		if (!texture_qsamp96)
			load_texture_from_file(YourSound::get_current_exe_directory() / "img/qsamp96.png", g_renderer,
								   &texture_qsamp96, nullptr, nullptr);

		static float title_height = 0.f;

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 100.f);
		ImGui::SetCursorPosY((720.f - title_height) / 2.f);
		ImGui::BeginGroup();
		ImGui::Image(texture_qsamp96, {96.f, 96.f});

		ImGui::SameLine();

		ImGui::PushFont(YourSound::UI::get_font(ysbp_ui_FONT_BOLD_LARGE));
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (96.f - 64.f) / 2.f);
		ImGui::Text("QuickSample");
		ImGui::PopFont();
		ImGui::EndGroup();

		title_height = ImGui::GetItemRectSize().y;
	}

	{
		static float options_height = 0.f;
		static float options_width = 0.f;

		ImGui::SetCursorPosX(1280.f - options_width - 175.f);
		ImGui::SetCursorPosY((720.f - options_height) / 2.f);
		ImGui::BeginGroup();
		const ImVec2 rf_title_cursor_pos = ImGui::GetCursorScreenPos();
		ImGui::Text("Options");
		ImGui::GetWindowDrawList()->AddLineH(rf_title_cursor_pos.x,
											 rf_title_cursor_pos.x + ImGui::GetItemRectSize().x - 1.f,
											 rf_title_cursor_pos.y + ImGui::GetItemRectSize().y - 1.f, 0xFF999999);
		ImGui::Indent();
		ImGui::TextLink("Open Project");
		if (ImGui::TextLink("New Project")) state = QSState::EDIT;
		ImGui::TextLinkOpenURL("Manual",
							   "https://fast.yoursoftware.org/org.yoursoftware/YourSound/QuickSample-Manual.pdf");
		ImGui::EndGroup();

		options_height = ImGui::GetItemRectSize().y;
		options_width = ImGui::GetItemRectSize().x;
	}

	{
		static float version_height = 0.f;

		ImGui::SetCursorPos({5.f, 720.f - version_height - 5.f});
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(50, 50, 50, 255));
		ImGui::Text("QuickSample 1.0.0 for YourSound %s, BPI Version %u", YS_VERSION_STRING, YS_BPI_VERSION);
		ImGui::PopStyleColor();
		version_height = ImGui::GetItemRectSize().y;
	}

	ImGui::End();
}

void main_loop() {
	if (show_as_window) { show_as_window = audio_setup_window(false); }
	audio_meter_window();
	switch (state) {
	case QSState::WELCOME: intro_window(); break;
	case QSState::EDIT: edit_window(); break;
	default: break;
	}
}

void init() {
	YS_RUNTIME_ASSERT(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO), std::runtime_error, "SDL failed to initialise");

	const float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());

	constexpr SDL_WindowFlags window_flags = SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
	g_window = SDL_CreateWindow("YourSound QuickSample", static_cast<int>(1280 * main_scale),
								static_cast<int>(800 * main_scale), window_flags);

	YS_RUNTIME_ASSERT(g_window, std::runtime_error, std::format("Failed to create window:\n{}", SDL_GetError()));

	g_renderer = SDL_CreateRenderer(g_window, nullptr);
	SDL_SetRenderVSync(g_renderer, true);

	YS_RUNTIME_ASSERT(g_renderer, std::runtime_error,
					  std::format("Failed to create SDL renderer:\n{}", SDL_GetError()));

	SDL_SetWindowPosition(g_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	SDL_ShowWindow(g_window);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO &io = ImGui::GetIO();
	io.IniFilename = nullptr;

	ImGuiStyle &style = ImGui::GetStyle();
	ImGui::StyleColorsDark();

	style.FontSizeBase = 20.f;
	style.Alpha = 1.f;

	style.ScaleAllSizes(main_scale);
	style.FontScaleDpi = main_scale;

	YourSound::UI::imgui_load_fonts(io);

	ImGui_ImplSDL3_InitForSDLRenderer(g_window, g_renderer);
	ImGui_ImplSDLRenderer3_Init(g_renderer);

	YourSound::UI::set_imgui_context(ImGui::GetCurrentContext());

	show_as_window = true;
	while (true) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			ImGui_ImplSDL3_ProcessEvent(&event);
			if (event.type == SDL_EVENT_QUIT) return;
			if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(g_window))
				return;
		}

		if (SDL_GetWindowFlags(g_window) & SDL_WINDOW_MINIMIZED) {
			SDL_Delay(10);
			continue;
		}

		ImGui_ImplSDLRenderer3_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();

		main_loop();

		ImGui::Render();
		SDL_SetRenderScale(g_renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
		SDL_SetRenderDrawColorFloat(g_renderer, 1.f, 0.f, 0.f, 1.f);
		SDL_RenderClear(g_renderer);
		ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), g_renderer);
		SDL_RenderPresent(g_renderer);
	}
}

int main(int argv, char *argc[]) {
	try {
		init();

		return 0;
	} catch (std::exception &e) {
		const std::source_location source_location = std::source_location::current();
		const std::stacktrace stacktrace = std::stacktrace::current();
		std::ostringstream stream;

		stream << stacktrace;

		std::cerr << "An unhandled exception was caught at main.cpp:main\n"
				  << e.what() << "\nStacktrace:\n"
				  << stream.str() << std::endl;

		std::cout << "Tip: Set a breakpoint at " << source_location.file_name() << "(" << source_location.line()
				  << ") for better debugging." << std::endl;

		show_error(std::format("An unexpected error occurred:\n\n{}\n\nThe program will now close.\nFor more "
							   "information, check the Standard Error Stream.",
							   e.what(), stream.str()));

		return 1;
	}
}
