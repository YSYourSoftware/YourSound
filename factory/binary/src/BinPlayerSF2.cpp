#include "BinPlayerSF2.hpp"

#include "Utils.hpp"

#include <imgui-knobs.h>
#include <imgui.h>
#define TSF_IMPLEMENTATION
#include <tsf.h>
#include <YourSound/BinPlayerInterface.hpp>
#include <YourSound/Config.hpp>
#include <YourSound/Serialisation.hpp>

#include <fstream>
#include <iostream>

void action_import_sf2(const char *filepath) {
	try {
		const std::filesystem::path soundfont = filepath;
		const std::filesystem::path library_location = YourSound::get_library_location() / "Imported SoundFonts" / soundfont.stem();

		std::filesystem::create_directory(library_location);
		std::filesystem::copy_file(soundfont, library_location / "source.sf2");

		tsf *sf2 = tsf_load_filename(filepath);

		for (int i = 0; i < tsf_get_presetcount(sf2); i++) {
			std::ofstream file{library_location / (std::string(tsf_get_presetname(sf2, i)) + ".yspp")};
			if (!file) throw std::runtime_error("Could not open file: " + (library_location / (std::string(tsf_get_presetname(sf2, i)) + ".yspp")).string());

			BinPlayerSF2 player;

			player.m_soundfont_resource.set_path(library_location / "source.sf2");
			player.m_preset_id = i;

			const uint64_t size = player.store_calc_size(true);
			auto *output_buffer = new uint8_t[size];

			player.store(output_buffer, true);

			file.write(reinterpret_cast<const char*>(output_buffer), size);
		}

		tsf_close(sf2);
	} catch (const std::exception &e) {
		ysbp_show_error(YourSound::get_error_source(), e.what());
	}
}

void action_load_sf2(const char *filepath, YS_RackHandle rack) {
	auto *player = new BinPlayerSF2();
	player->sf2_load_from_file(filepath);
	ysbp_rack_add_player(rack, player);
}

BinPlayerSF2::~BinPlayerSF2() {
	tsf_close(m_soundfont);
}

void BinPlayerSF2::note_on(const uint8_t midi_note_number, const float velocity) {tsf_channel_note_on(m_soundfont, 0, midi_note_number, velocity);}
void BinPlayerSF2::note_off(const uint8_t midi_note_number) {tsf_channel_note_off(m_soundfont, 0, midi_note_number);}
void BinPlayerSF2::render(float *output_buffer, const uint16_t number_samples) {tsf_render_float(m_soundfont, output_buffer, number_samples, false);}

uint64_t BinPlayerSF2::store_calc_size(const bool store_reference) const {
	// preset_id[2] + soundfont_resource[?]
	return 2 + m_soundfont_resource.store_calc_size(store_reference);
}

void BinPlayerSF2::store(uint8_t *output_buffer, const bool store_reference) const {
	YourSound::write_integral_be<uint16_t>(output_buffer, m_preset_id);
	m_soundfont_resource.store(output_buffer + 2, store_reference);
}

void BinPlayerSF2::load(const uint8_t* input_buffer) {
	m_preset_id = YourSound::read_integral_be<uint16_t>(input_buffer);
	m_soundfont_resource.load(input_buffer + 2);
}

void BinPlayerSF2::set_sample_rate(const uint32_t value) {
	m_sample_rate = value;
	tsf_set_output(m_soundfont, TSF_STEREO_INTERLEAVED, m_sample_rate, m_gain);
}

void BinPlayerSF2::reset() {tsf_reset(m_soundfont); m_pitch_bend = 0.f;}

void BinPlayerSF2::render_graphics(YS_ImContextHandle im_context) {
	ImGui::SetCurrentContext(static_cast<ImGuiContext*>(im_context));

	if (ImGui::BeginCombo("Preset", tsf_get_presetname(m_soundfont, m_preset_id))) {
		for (uint16_t i = 0; i < tsf_get_presetcount(m_soundfont); i++) {

			const bool is_selected = m_preset_id == i;

			if (ImGui::Selectable(tsf_get_presetname(m_soundfont, i), is_selected)) {
				m_preset_id = i;
				tsf_channel_set_presetnumber(m_soundfont, 0, m_preset_id);
			}

			if (is_selected) ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}

	imgui_align_cursor_for_center(m_knob_row_width);

	ImGui::BeginGroup();
	if (ImGuiKnobs::Knob("Gain", &m_gain, -20.f, 10.f, 0.1f, "%.2fdB", ImGuiKnobVariant_Wiper, 75)) {
		tsf_set_output(m_soundfont, TSF_STEREO_INTERLEAVED, m_sample_rate, m_gain);
	}
	ImGui::EndGroup();

	if (const float current_width = ImGui::GetItemRectSize().x; current_width > 0.0f) m_knob_row_width = current_width;
}

void BinPlayerSF2::sf2_load_from_file(const char *filepath) {
	m_soundfont_resource.set_path(filepath);
	m_soundfont_resource.reload_file();

	m_soundfont = tsf_load_memory(m_soundfont_resource.pointer(), m_soundfont_resource.get_length());
}
