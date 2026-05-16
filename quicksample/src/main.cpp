#include "MessageBox.hpp"

#include <YourSound/Player.hpp>
#include <YourSound/UI/Fonts.hpp>

#include <ImGuiPianoKeyboard.hpp>
#include <SDL3/SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <imgui_stdlib.h>

#include <filesystem>
#include <mutex>
#include <stacktrace>

#include "YourSound/BinPlayerRegistry.hpp"

ImFont *font_bold;
ImFont *font_light;
ImFont *font_heading;

SDL_Renderer *renderer;

bool run_main_loop = true;
bool bin_player_loading = true;
bool chosen_dll = false;

std::filesystem::path binplayer_dll_path;

constexpr uint16_t BUFFER_SIZE = 2048;
constexpr uint32_t SAMPLE_RATE = 3200;

constexpr SDL_AudioSpec AUDIO_SPEC = {SDL_AUDIO_F32, 2, SAMPLE_RATE};

auto keys_pressed = new bool[128]{};
auto *output_buffer = new float[BUFFER_SIZE * 2];
int32_t prev_note_active = -1;

YourSound::Player *bin_player = nullptr;
YourSound::Player **bin_player_ref = &bin_player;

std::mutex bin_player_mutex;

bool piano_callback(void *data, const int message, const int key, const float velocity) {
	if (key >= 128) return false;
	if (message == NoteGetStatus) return keys_pressed[key];

	if (message == NoteOn) {
		keys_pressed[key] = true;
		static_cast<YourSound::Player *>(data)->note_on(key, velocity);
	}

	if (message == NoteOff) {
		keys_pressed[key] = false;
		static_cast<YourSound::Player *>(data)->note_off(key);
	}

	return false;
}

void yoursound_error_callback(const char *message) {
	std::stacktrace backtrace = std::stacktrace::current();
	std::ostringstream backtrace_string;

	backtrace_string << backtrace;

	const std::string final_message = "--- YSBP Error Callback ---\n\n" + std::string(message) +
									  "\n\nThe backtrace is shown below:\n" + backtrace_string.str();

	// For testing libraries: keep a breakpoint set here so if any errors occur, we can easily debug them.
	show_error(final_message);
}

static void SDLCALL callback_dll(void *userdata, const char *const *filelist, int filter) {
	if (!filelist) {
		show_error(SDL_GetError());
		return;
	}
	if (!*filelist) return;

	binplayer_dll_path = *filelist;

	chosen_dll = true;

	YourSound::BinPlayer::set_error_func(yoursound_error_callback);
	YourSound::BinPlayer::register_player("org.yoursoftware.sound.sampler", [] {
		return YourSound::load_binary_player(binplayer_dll_path, "org.yoursoftware.sound.sampler");
	});

	try {
		bin_player = YourSound::load_player_by_id("org.yoursoftware.sound.sampler");
		bin_player_ref = &bin_player;

		bin_player->set_bpm(120);
		bin_player->set_sample_rate(SAMPLE_RATE);
		bin_player->reset();

		bin_player_loading = false;
	} catch (std::exception &e) {
		show_error(e.what());
		std::exit(1);
	}
}

void SDLCALL audio_callback(void *userdata, SDL_AudioStream *stream, const int additional_amount, int total_amount) {
	if (bin_player_loading) return;
	if (additional_amount < 1) return;

	std::lock_guard bin_player_lock(bin_player_mutex);

	const auto player = *static_cast<YourSound::Player **>(userdata);
	player->render(output_buffer, additional_amount / (sizeof(float) * 2));

	SDL_PutAudioStreamData(stream, output_buffer, additional_amount);
}

int main(int argv, char *argc[]) {
	std::set_terminate([] {
		show_error("Unhandled exception.");
		std::exit(1);
	});

	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
		show_error(SDL_GetError());
		return 1;
	}

	const float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
	constexpr SDL_WindowFlags window_flags = SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_BORDERLESS;
	SDL_Window *window = SDL_CreateWindow("YourSound QuickSample", static_cast<int>(1280 * main_scale),
										  static_cast<int>(720 * main_scale), window_flags);
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
	YourSound::UI::imgui_load_fonts(io);
	io.Fonts->Build();

	ImGui::StyleColorsDark();
	style.Colors[ImGuiCol_WindowBg].w = 1.f;

	io.IniFilename = nullptr;

	std::string search_name;
	std::string search_author;

	constexpr ImGuiWindowFlags static_window_flags =
		ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDocking;

	constexpr SDL_DialogFileFilter yspp_filters[] = {{"YourSound Player Presets", "yspp"}, {"All Files", "*"}};
	constexpr SDL_DialogFileFilter yssps_filters[] = {{"YourSound Sample Player Sample", "yssps"}, {"All Files", "*"}};

	constexpr SDL_DialogFileFilter dll_filters[] = {{"Windows Dynamically-Linked Library", "dll"},
													{"MacOS Dynamic Library", "dylib"},
													{"Unix Shared Object", "so"},
													{"All Files", "*"}};

	SDL_ShowOpenFileDialog(callback_dll, nullptr, window, dll_filters, 4, nullptr, false);

	const ImGuiViewport *viewport = ImGui::GetMainViewport();

	SDL_AudioStream *audio_stream =
		SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &AUDIO_SPEC, &audio_callback, bin_player_ref);

	SDL_ResumeAudioDevice(SDL_GetAudioStreamDevice(audio_stream));

	while (run_main_loop) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			ImGui_ImplSDL3_ProcessEvent(&event);
			if (event.type == SDL_EVENT_QUIT) run_main_loop = false;
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

		ImGui::Begin("YourSound QuickSample", &run_main_loop, static_window_flags);

		static bool dragging = false;
		static int offset_x = 0;
		static int offset_y = 0;

		float mouse_x, mouse_y;

		if (const Uint32 buttons = SDL_GetGlobalMouseState(&mouse_x, &mouse_y);
			buttons & SDL_BUTTON_MASK(SDL_BUTTON_LEFT)) {
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

		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("New Project")) return 0;
				if (ImGui::MenuItem("Save Samples")) return 0;
				if (ImGui::MenuItem("Save Presets")) return 0;
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::End();

		ImGui::Begin("Sample Player");

		if (!bin_player_loading) bin_player->render_graphics(ImGui::GetCurrentContext());
		ImGui_PianoKeyboard("Piano", ImVec2(1024, 100), &prev_note_active, 21, 108, piano_callback, bin_player);

		ImGui::End();

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
