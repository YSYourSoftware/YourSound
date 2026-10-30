#include "Audio.hpp"

#include "Edit.hpp"
#include "Image.hpp"
#include "MessageBox.hpp"
#include "Renderer.hpp"

#include <YSKMH/MIDIMapping.hpp>
#include <YourSound/BinPlayerSTD.hpp>
#include <YourSound/Config.hpp>
#include <YourSound/UI/Colours.hpp>
#include <YourSound/UI/Fonts.hpp>

#include <imgui-knobs.h>
#include <imgui.h>

#include <array>
#include <fstream>

typedef struct {
	const char *name;
	uint32_t rate;
} sample_rate_option_t;

constexpr std::array<sample_rate_option_t, 5> sample_rates = {
	{{"28,000Hz", 28000}, {"32,000Hz", 32000}, {"44,100Hz", 44100}, {"48,000Hz", 48000}, {"96,000Hz", 96000}}};

std::array<float, 256 * 2> input_buffer{};
size_t input_buffer_write = 0;
bool monitor = false;

static void replace_all(std::string &str, const std::string &from, const std::string &to) {
	if (from.empty()) return;
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}
}

static float peak_sample_in_buffer(const float *buf, const uint16_t count, const uint8_t stride = 1) {
	float peak = 0.f;
	for (uint16_t i = 0; i < count; i++) peak = std::fmaxf(std::fabsf(buf[i * stride]), peak);
	return peak;
}

static void SDLCALL input_callback(void *, SDL_AudioStream *stream, int, int) {
	std::array<float, 256> chunk{};
	int available = SDL_GetAudioStreamAvailable(stream);

	while (available >= 4) {
		int bytes = std::min(available, 256 * 4);
		bytes -= bytes % 4;

		const int got = SDL_GetAudioStreamData(stream, chunk.data(), bytes);
		if (got <= 0) break;

		const int count = got / 4;
		for (int i = 0; i < count; ++i) {
			input_buffer[input_buffer_write] = chunk[i] * 2.f;
			input_buffer_write = (input_buffer_write + 1) % input_buffer.size();
		}

		new_samples_in(chunk.data(), got / 4);
		if (monitor) SDL_PutAudioStreamData(g_output_stream, chunk.data(), got);

		available -= got;
	}
}

bool audio_setup_window(const bool refresh) {
	typedef struct {
		std::string name;
		std::filesystem::path json_file;
	} midi_mapping_t;

	static int input_count;
	static SDL_AudioDeviceID *inputs = nullptr;

	static std::vector<midi_mapping_t> midi_mappings;
	static std::vector<std::string> midi_devices;

	if (!inputs || refresh || midi_mappings.empty()) {
		while (true) {
			inputs = SDL_GetAudioRecordingDevices(&input_count);
			YS_RUNTIME_ASSERT(inputs, std::runtime_error,
							  std::format("Could not get audio recording devices: {}", SDL_GetError()));
			if (input_count > 0) break;

			if (!ask_abort_retry_warning(
					"No audio inputs found.",
					"Ensure all input devices are connected properly and all drivers have been installed."))
				std::exit(1);
			SDL_free(inputs);
		}

		midi_mappings.clear();
		for (const auto &path :
			 std::filesystem::directory_iterator{YourSound::get_current_exe_directory() / "keyboard-mappings"}) {
			if (path.is_regular_file() && path.path().extension() == ".json") {
				midi_mappings.emplace_back(path.path().stem().generic_string(), path.path());
				continue;
			}

			if (path.is_directory()) {
				for (const auto &path1 : std::filesystem::directory_iterator{path.path()}) {
					if (!(path1.is_regular_file() && path1.path().extension() == ".json")) continue;

					std::string name = std::filesystem::relative(path1.path(), YourSound::get_current_exe_directory() /
																				   "keyboard-mappings")
										   .replace_extension("")
										   .generic_string();
					replace_all(name, "/", " > ");
					midi_mappings.emplace_back(name, path1.path());
				}
			}
		}

		midi_mappings.shrink_to_fit();

		delete g_midi_mapping;
		g_midi_mapping = new YSKMH::MIDIMapping(
			R"({"script":"null","controls":{}})", {}, [](const std::string &, float) {},
			[](const std::string &) { return 0.f; }, [](int8_t) {}, [](int8_t) {}, [] {}, [] {},
			[](uint8_t, uint8_t, float) {}, [](uint8_t, uint8_t) {});
		while (true) {
			midi_devices = g_midi_mapping->get_midi_input_names();
			if (!midi_devices.empty()) break;

			if (!ask_abort_retry_warning(
					"No MIDI inputs found.",
					"Ensure all MIDI input devices are connected properly and all drivers have been installed."))
				std::exit(1);
		}
	}

	if (!ImGui::BeginPopupModal("Audio & MIDI Setup", nullptr,
								ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
									ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::OpenPopup("Audio & MIDI Setup");
		return true;
	}

	ImGui::Text("Audio");

	static uint32_t selected_input = 0;
	if (ImGui::BeginCombo("Input##aud", SDL_GetAudioDeviceName(inputs[selected_input]))) {
		for (int i = 0; i < input_count; i++) {
			if (ImGui::Selectable(SDL_GetAudioDeviceName(inputs[i]), selected_input == i)) selected_input = i;
			if (selected_input == i) ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	static uint8_t selected_sample_rate = 3;
	if (ImGui::BeginCombo("Sample Rate", sample_rates[selected_sample_rate].name)) {
		for (int i = 0; i < sample_rates.size(); i++) {
			if (ImGui::Selectable(sample_rates[i].name, selected_sample_rate == i)) selected_sample_rate = i;
			if (selected_sample_rate == i) ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	static int channels = 1;
	ImGui::SliderInt("Channels", &channels, 1, 2, "%d", ImGuiSliderFlags_AlwaysClamp);

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::Text("MIDI");

	static uint32_t selected_midi = 0;
	if (ImGui::BeginCombo("Input##midi", midi_devices[selected_midi].c_str())) {
		for (int i = 0; i < midi_devices.size(); i++) {
			if (ImGui::Selectable(midi_devices[i].c_str(), selected_midi == i)) selected_midi = i;
			if (selected_midi == i) ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	static uint32_t selected_mapping = 0;
	if (ImGui::BeginCombo("Mapping", midi_mappings[selected_mapping].name.c_str())) {
		for (int i = 0; i < midi_mappings.size(); i++) {
			if (ImGui::Selectable(midi_mappings[i].name.c_str(), selected_mapping == i)) selected_mapping = i;
			if (selected_mapping == i) ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	ImGui::PushStyleColor(ImGuiCol_Button, YourSound::UI::Colours::BUTTON_GREEN);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, YourSound::UI::Colours::BUTTON_GREEN_HOVER);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, YourSound::UI::Colours::BUTTON_GREEN_ACTIVE);
	if (ImGui::Button(
			"OK",
			{ImGui::GetWindowSize().x - (ImGui::GetStyle().WindowPadding.x + ImGui::GetStyle().ItemSpacing.x), 26.f})) {
		ImGui::PopStyleColor(3);
		ImGui::CloseCurrentPopup();
		ImGui::EndPopup();

		g_channels = channels;
		g_sample_rate = sample_rates[selected_sample_rate].rate;

		const SDL_AudioSpec spec = {SDL_AUDIO_F32, channels, static_cast<int>(sample_rates[selected_sample_rate].rate)};
		g_input_stream = SDL_OpenAudioDeviceStream(inputs[selected_input], &spec, &input_callback, nullptr);
		g_output_stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, nullptr, nullptr);

		YS_RUNTIME_ASSERT(g_input_stream, std::runtime_error, "Failed to open input audio stream");
		YS_RUNTIME_ASSERT(g_output_stream, std::runtime_error, "Failed to open output audio stream");

		SDL_free(inputs);
		inputs = nullptr;

		SDL_ResumeAudioStreamDevice(g_input_stream);
		SDL_ResumeAudioStreamDevice(g_output_stream);

		delete g_midi_mapping;

		const std::ifstream json_file{midi_mappings[selected_mapping].json_file};
		std::ostringstream json_stream;
		json_stream << json_file.rdbuf();

		g_midi_mapping = new YSKMH::MIDIMapping(
			json_stream.str(), midi_mappings[selected_mapping].json_file.parent_path(),
			[](const std::string &, float) {}, [](const std::string &) { return 0.f; }, [](int8_t) {}, [](int8_t) {},
			[] {}, [] {}, [](uint8_t, uint8_t, float) {}, [](uint8_t, uint8_t) {});

		g_midi_mapping->set_midi_input(selected_midi);

		return false;
	}

	ImGui::PopStyleColor(3);

	ImGui::EndPopup();
	return true;
}

void audio_meter_window() {
	ImGui::SetNextWindowPos({0.f, 0.f});
	ImGui::SetNextWindowSize({1280.f, 80.f});
	ImGui::PushStyleColor(ImGuiCol_WindowBg, {0.025f, 0.025f, 0.025f, 1.f});
	ImGui::Begin("Audio Meter", nullptr,
				 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
					 ImGuiWindowFlags_NoScrollWithMouse);
	ImGui::PopStyleColor();

	{ // Gain Knob
		float gain = SDL_GetAudioStreamGain(g_input_stream) * 100.f;
		ImGui::SetCursorPosY((80.f - 60.f) / 2.f);
		if (ImGuiKnobs::Knob("Gain", &gain, 0.f, 200.f, 0.f, "IN: %.1f%% (RMB to reset)", ImGuiKnobVariant_Wiper, 60.f,
							 ImGuiKnobFlags_AlwaysClamp | ImGuiKnobFlags_ValueTooltip | ImGuiKnobFlags_NoInput |
								 ImGuiKnobFlags_NoTitle))
			SDL_SetAudioStreamGain(g_input_stream, gain / 100.f);
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsItemHovered())
			SDL_SetAudioStreamGain(g_input_stream, 1.f);
	}

	ImGui::SameLine();

	{ // Oscilloscope
		static float oscilloscope_height = 0.f;
		ImGui::SetCursorPosY((80.f - oscilloscope_height) / 2.f);
		ImGui::BeginGroup();
		ImGui::PlotLines("##input_left", input_buffer.data(), input_buffer.size() / 2, 0, nullptr, -1.f, 1.f,
						 {150.f, g_channels == 2 ? 25.f : 50.f}, g_channels * 4);
		if (g_channels == 2)
			ImGui::PlotLines("##input_right", input_buffer.data() + 1, input_buffer.size() / 2, 0, nullptr, -1.f, 1.f,
							 {150.f, 25.f}, 8);
		ImGui::EndGroup();
		oscilloscope_height = ImGui::GetItemRectSize().y;
	}

	ImGui::SameLine();

	{ // Volume Meter
		static SDL_Texture *texture_meter_grad = nullptr;
		static SDL_Texture *texture_meter_clip = nullptr;
		static SDL_Texture *texture_meter_overlay_50 = nullptr;
		static SDL_Texture *texture_meter_overlay_25 = nullptr;

		if (!texture_meter_grad)
			load_texture_from_file(YourSound::get_current_exe_directory() / "img/meter-gradient.png", g_renderer,
								   &texture_meter_grad, nullptr, nullptr);
		if (!texture_meter_clip)
			load_texture_from_file(YourSound::get_current_exe_directory() / "img/meter-clip.png", g_renderer,
								   &texture_meter_clip, nullptr, nullptr);
		if (!texture_meter_overlay_50)
			load_texture_from_file(YourSound::get_current_exe_directory() / "img/meter-overlay-50.png", g_renderer,
								   &texture_meter_overlay_50, nullptr, nullptr);
		if (!texture_meter_overlay_25)
			load_texture_from_file(YourSound::get_current_exe_directory() / "img/meter-overlay-25.png", g_renderer,
								   &texture_meter_overlay_25, nullptr, nullptr);

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ((g_channels == 2) ? 3.f : 5.f));
		ImGui::BeginGroup();

		const float peak_l = peak_sample_in_buffer(input_buffer.data(), input_buffer.size() / 2, g_channels);
		const float peak_l_clamp = std::clamp(peak_l, 0.f, 1.f);

		const ImVec2 left_meter_overlay_pos = ImGui::GetCursorPos();
		ImGui::GetWindowDrawList()->AddRectFilled(
			left_meter_overlay_pos,
			{left_meter_overlay_pos.x + 150.f, left_meter_overlay_pos.y + ((g_channels == 2) ? 25.f : 50.f)},
			0xFF1A1A1A);

		ImGui::Image(peak_l > 1.f ? texture_meter_clip : texture_meter_grad,
					 {150.f * peak_l_clamp, g_channels == 2 ? 25.f : 50.f}, {0.f, 0.f}, {peak_l_clamp, 1.f});

		ImGui::SetCursorPosY(left_meter_overlay_pos.y);
		ImGui::Image(g_channels == 2 ? texture_meter_overlay_25 : texture_meter_overlay_50,
					 {150.f, g_channels == 2 ? 25.f : 50.f});

		if (g_channels == 2) {
			const float peak_r = peak_sample_in_buffer(input_buffer.data() + 1, input_buffer.size() / 2, g_channels);
			const float peak_r_clamp = std::clamp(peak_r, 0.f, 1.f);

			const ImVec2 right_meter_overlay_pos = ImGui::GetCursorPos();
			ImGui::GetWindowDrawList()->AddRectFilled(
				right_meter_overlay_pos, {right_meter_overlay_pos.x + 150.f, right_meter_overlay_pos.y + 25.f},
				0xFF1A1A1A);

			ImGui::Image(peak_r > 1.f ? texture_meter_clip : texture_meter_grad, {150.f * peak_r_clamp, 25.f},
						 {0.f, 0.f}, {peak_r_clamp, 1.f});

			ImGui::SetCursorPosY(right_meter_overlay_pos.y);
			ImGui::Image(texture_meter_overlay_25, {150.f, 25.f});
		}

		ImGui::EndGroup();
	}

	ImGui::SameLine();

	{ // Monitoring
		static ImVec2 checkbox_size;

		ImGui::SetCursorPosY((80.f - checkbox_size.y) / 2.f);
		ImGui::SetCursorPosX(1280.f - ImGui::GetStyle().WindowPadding.x - checkbox_size.x);

		ImGui::PushFont(YourSound::UI::get_font(ysbp_ui_FONT_STANDARD_MEDIUM));
		ImGui::Checkbox("##monitor", &monitor);
		ImGui::PopFont();

		if (ImGui::IsItemHovered()) {
			ImGui::BeginTooltip();
			ImGui::Text("Monitor");
			ImGui::EndTooltip();
		}

		checkbox_size = ImGui::GetItemRectSize();
	}

	ImGui::End();
}
