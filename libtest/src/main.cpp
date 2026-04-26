#include "MessageBox.hpp"

#include <YourSound/BinPlayerRegistry.hpp>
#include <YourSound/Player.hpp>
#include <YourSound/IntegratedPlayers/ModLFO.hpp>
#include <YourSound/UI/Fonts.hpp>

#include <imgui-knobs.h>
#include <imgui.h>
#include <ImGuiPianoKeyboard.hpp>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <imgui_memory_editor.h>
#include <SDL3/SDL.h>

#include <fstream>
#include <iostream>
#include <mutex>
#include <stacktrace>

constexpr uint16_t BUFFER_SIZE = 2048;
constexpr uint32_t SAMPLE_RATE = 32000;
const char *PLAYER_ID = "org.yoursoftware.sound.basic-osc";

constexpr SDL_AudioSpec AUDIO_SPEC = {
	SDL_AUDIO_F32,
	2,
	SAMPLE_RATE
};

int32_t prev_note_active = -1;
float *output_buffer = new float[BUFFER_SIZE * 2];
float param_value;
bool *keys_pressed = new bool[128]{};
bool run_main_loop = true;
bool show_param_edit = false;
bool bin_player_loading = true;
const char* param_edit_id = new char[32];

YourSound::Player *bin_player = nullptr;
YourSound::Player **bin_player_ref = &bin_player;

std::mutex bin_player_mutex;

bool piano_callback(void *data, const int message, const int key, const float velocity) {
	if (key >= 128) return false;
	if (message == NoteGetStatus) return keys_pressed[key];

	if (message == NoteOn) {
		keys_pressed[key] = true;
		static_cast<YourSound::Player*>(data)->note_on(key, velocity);
	}

	if (message == NoteOff) {
		keys_pressed[key] = false;
		static_cast<YourSound::Player*>(data)->note_off(key);
	}

	return false;
}

static void SDLCALL load_callback(void *userdata, const char *const *filelist, int filter) {
	if (!filelist) {std::cerr << SDL_GetError() << std::endl; return;}
	if (!*filelist) return;

	std::lock_guard bin_player_lock(bin_player_mutex);
	bin_player_loading = true;

	std::cout << *filelist << std::endl;

	std::ifstream input_file(*filelist, std::ios::binary | std::ios::ate);
	const std::streamsize file_length = input_file.tellg();
	auto *input_buffer = new uint8_t[file_length];

	std::cout << +file_length << std::endl;

	input_file.seekg(0, std::ios::beg);
	if (!input_file.read(reinterpret_cast<char*>(input_buffer), file_length)) {std::cerr << "Failed to read input file" << std::endl; return;}
	for (std::streamsize i = 0; i < file_length; i++) {
		std::cout << std::hex << +*(input_buffer + i);
	}
	std::cout << std::endl;

	delete bin_player;

	uint8_t data_start_index = 0;

	try {
		SARC_RUNTIME_ASSERT(std::memcmp(input_buffer, "YSPP", 4) == 0, std::runtime_error, "YSPP magic missing");
		data_start_index = std::strlen(reinterpret_cast<char const*>(input_buffer + 4)) + 1 + 4;
		char *buffer = new char[std::strlen(reinterpret_cast<char const*>(input_buffer + 4)) + 1];
		std::memcpy(buffer, input_buffer + 4, std::strlen(reinterpret_cast<char const*>(input_buffer + 4)) + 1);
		const std::string player_id{buffer};
		delete[] buffer;
		bin_player = YourSound::load_player_by_id(player_id);
	} catch (const std::exception &e) {
		show_error(e.what());
		return;
	}

	if (!bin_player) {show_error("Player pointer is a nullptr"); return;}

	bin_player->load(input_buffer + data_start_index);

	delete[] input_buffer;

	bin_player->set_bpm(120);
	bin_player->set_sample_rate(SAMPLE_RATE);
	bin_player->reset();

	SDL_Delay(500);

	bin_player_loading = false;
}

void yoursound_error_callback(const char *message) {
	std::stacktrace backtrace = std::stacktrace::current();
	std::ostringstream backtrace_string;

	backtrace_string << backtrace;

	const std::string final_message = "--- YSBP Error Callback ---\n\n" + std::string(message) + "\n\nThe backtrace is shown below:\n" + backtrace_string.str();

	// For testing libraries: keep a breakpoint set here so if any errors occur, we can easily debug them.
	show_error(final_message);
}

void save_state(const bool store_reference, const YourSound::Player *player) {
	const char *player_id = player->get_id();

	const uint64_t buffer_size = player->store_calc_size(store_reference);
	const uint64_t total_size = 4 + buffer_size + std::strlen(player_id) + 1;

	auto *buffer = new uint8_t[total_size];
	std::memcpy(buffer, "YSPP", 4);
	std::memcpy(buffer + 4, player_id, std::strlen(player_id) + 1);

	player->store(buffer + 4 + std::strlen(player_id) + 1, store_reference);

	std::ofstream out_file("save.yspp");
	out_file.write(reinterpret_cast<const char*>(buffer), total_size);

	delete[] buffer;
}

void SDLCALL audio_callback(void *userdata, SDL_AudioStream *stream, const int additional_amount, int total_amount) {
	if (bin_player_loading) return;
	if (additional_amount < 1) return;

	std::lock_guard bin_player_lock(bin_player_mutex);

	const auto player = *static_cast<YourSound::Player**>(userdata);
	player->render(output_buffer, additional_amount / (sizeof(float) * 2));

	SDL_PutAudioStreamData(stream, output_buffer, additional_amount);
}

int main(int argv, char *argc[]) {
	YourSound::BinPlayer::set_error_func(yoursound_error_callback);

	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) return 1;

	const float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
	constexpr SDL_WindowFlags window_flags = SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
	SDL_Window *window = SDL_CreateWindow("YourSound Library Tester", static_cast<int>(1280 * main_scale), static_cast<int>(950 * main_scale), window_flags);
	if (window == nullptr) return 1;

	SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);
	SDL_SetRenderVSync(renderer, 1);
	if (renderer == nullptr) return 1;

	SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	SDL_ShowWindow(window);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	io.IniFilename = nullptr;

	ImGuiStyle &style = ImGui::GetStyle();
	style.ScaleAllSizes(main_scale);
	style.FontScaleDpi = main_scale;

	ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer3_Init(renderer);

	style.FontSizeBase = 20.0f;
	io.Fonts->AddFontFromFileTTF("font/FiraSans-Regular.ttf");
	YourSound::UI::imgui_load_fonts(io);
	io.Fonts->Build();

	ImGui::StyleColorsDark();
	style.Colors[ImGuiCol_WindowBg].w = 1.f;

	YourSound::BinPlayer::Integrated::register_mod_lfo();
	YourSound::BinPlayer::register_player(PLAYER_ID, [] {return YourSound::load_binary_player(R"(D:\dev\YourSoftware\Sound\build\factory\binary\Debug\binplayers-x86-64.dll)", PLAYER_ID);});

	try {
		YourSound::Player *bin_player_internal = YourSound::load_player_by_id(PLAYER_ID);
		bin_player = bin_player_internal;

		bin_player = YourSound::load_player_wrapper_by_id("org.yoursoftware.sound.mod.lfo");
		dynamic_cast<YourSound::PlayerWrapper*>(bin_player)->set_wrapped_player(bin_player_internal);
	} catch (const std::exception &e) {
		show_error(e.what());
		return 1;
	}

	if (!bin_player) {std::cerr << "Player pointer null" << std::endl; return 1;}

	bin_player->set_bpm(120);
	bin_player->set_sample_rate(SAMPLE_RATE);
	bin_player->reset();

	bin_player_loading = false;

	MemoryEditor buffer_edit;
	SDL_AudioStream *audio_stream = SDL_OpenAudioDeviceStream(
		SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
		&AUDIO_SPEC,
		&audio_callback,
		bin_player_ref
	);

	if (!audio_stream) {std::cerr << "Audio stream not opened" << std::endl; return 1;}

	SDL_ResumeAudioDevice(SDL_GetAudioStreamDevice(audio_stream));

	constexpr SDL_DialogFileFilter yoursound_player_preset_filetype_filters[] = {
		{"YourSound Player Preset", "yspp"},
		{"All Files", "*"}
	};

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

		ImGui::Begin("Binary Player", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar);
		ImGui::BeginMenuBar();
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Save State")) save_state(true, bin_player);
			if (ImGui::MenuItem("Save State (embed resources)")) save_state(true, bin_player);
			if (ImGui::MenuItem("Load State")) SDL_ShowOpenFileDialog(load_callback, nullptr, window, yoursound_player_preset_filetype_filters, 2, nullptr, false);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit")) {
			if (ImGui::MenuItem("Parameters...")) show_param_edit = true;
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();

		if (bin_player_loading) {
			ImGui::Text("Loading Player State...");
		} else {
			bin_player->render_graphics(ImGui::GetCurrentContext());

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			ImGui_PianoKeyboard("Piano", ImVec2(1024, 100), &prev_note_active, 21, 108, piano_callback, bin_player);
		}
		ImGui::End();

		ImGui::Begin("Audio Wave", nullptr, ImGuiWindowFlags_NoResize);
		ImGui::PlotLines(
			"",
			output_buffer,
			128,
			0,
			nullptr,
			-1.0f,
			1.0f,
			ImVec2(150, 50)
		);
		ImGui::End();

		//buffer_edit.DrawWindow("Buffer", output_buffer, BUFFER_SIZE * 2);

		if (show_param_edit) {
			ImGui::Begin("Edit Parameters", &show_param_edit);
			if (ImGui::BeginCombo("##param_id", param_edit_id)) {
				const size_t count = bin_player->get_parameter_count();
				const char** parameters = new const char*[count];
				for (size_t i = 0; i < count; i++) {
					parameters[i] = new char[32];
				}

				bin_player->get_parameters(parameters);

				for (int i = 0; i < bin_player->get_parameter_count(); i++) {
					const bool is_selected = param_edit_id == parameters[i];
					if (ImGui::Selectable(parameters[i], is_selected)) param_edit_id = parameters[i];
					if (is_selected) ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			if (ImGuiKnobs::Knob("Value", &param_value, 0.f, 1.f, 0.01f, "%.2f", ImGuiKnobVariant_Wiper, 75)) {
				bin_player->set_parameter(param_edit_id, param_value);
			}

			ImGui::End();
		}

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