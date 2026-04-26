/*
 * This file is very messy.
 * TODO: Separate code here to different files.
 */

#include "Globals.hpp"
#include "LibraryInstall.hpp"
#include "MessageBox.hpp"
#include "SmallPopups.hpp"

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <imgui_stdlib.h>

#include <SDL3/SDL.h>

#if defined(_WIN32) && defined(YSL_DEBUG)
#include <Windows.h>
void enable_console() {
	AllocConsole();
	freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
	freopen_s(reinterpret_cast<FILE**>(stderr), "CONOUT$", "w", stderr);
	freopen_s(reinterpret_cast<FILE**>(stdin),  "CONIN$",  "r", stdin);

	std::ios::sync_with_stdio();
}
#endif

static void SDLCALL callback_install(void* userdata, const char* const* filelist, int filter) {
	if (!filelist) {show_error(SDL_GetError()); return;}
	if (!*filelist) return;

	target_library_path = *filelist;

	try {target_library = SArc::SArchive(target_library_path);} catch (std::exception &e) {show_error("Error reading SArc archive:\n" + std::string(e.what())); return;}
	// Ignore the clangd error you get on the line below.
	try {target_library_info = nlohmann::json::parse(target_library.get_file_by_path("yoursound.json").data);} catch (std::exception &e) {show_error("Error reading YourSound library information:\n" + std::string(e.what())); return;}
	try {throw_if_invalid_library_info();} catch (std::exception &e) {show_error("Malformed YourSound library information:\n" + std::string(e.what())); return;}

	try {
		int width, height;
		load_texture_from_memory(target_library.get_file_by_path("64.png").data, &target_library_cover_64, width, height, renderer);
		if (width != 64 || height != 64) throw std::invalid_argument("Incorrect dimensions for 64.png (expected 64x64)");
	} catch (std::exception &e) {show_error("Texture load error:\n" + std::string(e.what())); return;}

	display_install = true;
}

int main(int argv, char *argc[]) {
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		show_error(SDL_GetError());
		return 1;
	}

	const float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
	constexpr SDL_WindowFlags window_flags = SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_BORDERLESS ;
	SDL_Window *window = SDL_CreateWindow("YourSound Library Manager", static_cast<int>(500 * main_scale), static_cast<int>(650 * main_scale), window_flags);
	if (window == nullptr) {
		show_error(SDL_GetError());
		return 1;
	}

	renderer = SDL_CreateRenderer(window, nullptr);
	SDL_SetRenderVSync(renderer, 1);
	if (renderer == nullptr) {
		show_error(SDL_GetError());
		return 1;
	}

	SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	SDL_ShowWindow(window);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGuiStyle &style = ImGui::GetStyle();
	style.ScaleAllSizes(main_scale);
	style.FontScaleDpi = main_scale;

	ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer3_Init(renderer);

	style.FontSizeBase = 20.0f;
	io.Fonts->AddFontFromFileTTF("font/FiraSans-Regular.ttf");
	font_bold = io.Fonts->AddFontFromFileTTF("font/FiraSans-Bold.ttf");
	font_light = io.Fonts->AddFontFromFileTTF("font/FiraSans-Light.ttf");
	font_heading = io.Fonts->AddFontFromFileTTF("font/FiraSans-Bold.ttf", style.FontSizeBase * 1.75f);

	ImGui::StyleColorsDark();
	style.Colors[ImGuiCol_WindowBg].w = 1.f;

	io.IniFilename = nullptr;

	std::string search_name;
	std::string search_author;

	constexpr ImGuiWindowFlags static_window_flags =
		ImGuiWindowFlags_MenuBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoDocking;

	constexpr SDL_DialogFileFilter yoursound_filetype_filters[] = {
		{"YourSound Libraries", "yslib"},
		{"All Files", "*"}
	};

	const ImGuiViewport* viewport = ImGui::GetMainViewport();

	while (run_main_loop) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			ImGui_ImplSDL3_ProcessEvent(&event);
			if (event.type == SDL_EVENT_QUIT)
				run_main_loop = false;
			if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))
				run_main_loop = false;
		}

		if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED) {
			SDL_Delay(10);
			continue;
		}

		ImGui_ImplSDLRenderer3_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();

		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGui::Begin("YourSound Library Manager", &run_main_loop, static_window_flags);

		static bool dragging = false;
		static int offset_x = 0;
		static int offset_y = 0;

		float mouse_x, mouse_y;

		if (const Uint32 buttons = SDL_GetGlobalMouseState(&mouse_x, &mouse_y); buttons & SDL_BUTTON_MASK(SDL_BUTTON_LEFT)) {
			if (!dragging) {
				if (ImGui::IsWindowHovered(ImGuiHoveredFlags_RootWindow)) {

					dragging = true;

					int win_x, win_y;
					SDL_GetWindowPosition(window, &win_x, &win_y);

					offset_x = mouse_x - win_x;
					offset_y = mouse_y - win_y;
				}
			} else SDL_SetWindowPosition(window, mouse_x - offset_x, mouse_y - offset_y);
		} else dragging = false;

		if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_I)) SDL_ShowOpenFileDialog(callback_install, nullptr, window, yoursound_filetype_filters, 2, nullptr, false);

		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("About")) display_about = true;
				if (ImGui::MenuItem("Install", "Ctrl+I", false, !display_install)) SDL_ShowOpenFileDialog(callback_install, nullptr, window, yoursound_filetype_filters, 2, nullptr, false);
				ImGui::EndMenu();
			}

			#ifdef YSL_DEBUG
			if (ImGui::BeginMenu("Debug")) {
				#ifdef _WIN32
				if (ImGui::BeginMenu("Win32")) {
					if (ImGui::MenuItem("Allocate Console")) enable_console();
					ImGui::EndMenu();
				}
				#endif

				ImGui::EndMenu();
			}
			#endif

			ImGui::EndMenuBar();
		}

		ImGui::PushFont(font_heading);
		ImGui::Text("Installed YourSound Libraries");
		ImGui::PopFont();

		ImGui::Text("Click on a library to view more information about it.");

		const float fullWidth = ImGui::GetContentRegionAvail().x - 7.5f;

		ImGui::PushItemWidth(fullWidth * 0.5f);
		ImGui::InputTextWithHint("##search_name", "Filter name...", &search_name);
		ImGui::PopItemWidth();

		ImGui::SameLine();

		ImGui::PushItemWidth(fullWidth * 0.5f);
		ImGui::InputTextWithHint("##search_author", "Filter author...", &search_author);
		ImGui::PopItemWidth();

		ImGui::End();

		if (display_about) window_about();
		if (display_install) window_install_confirm();

		ImGui::Render();

		SDL_SetRenderScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);
		ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
		SDL_RenderPresent(renderer);
	}

	ImGui_ImplSDLRenderer3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}