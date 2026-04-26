#include "YourSound/IntegratedPlayers/ModLFO.hpp"

#include "YourSound/BinPlayerInterface.hpp"
#include "YourSound/Serialisation.hpp"
#include "YourSound/UI/Colours.hpp"
#include "YourSound/UI/Fonts.hpp"

#include <imgui-knobs.h>
#include <imgui.h>
#include <imgui_stdlib.h>

#include <iostream>

using namespace YourSound::BinPlayer::Integrated;

ModLFO::~ModLFO() {ysbp_destroy_bin_player(m_player);}

void ModLFO::note_on(const uint8_t midi_note_number, const float velocity) {m_player->note_on(midi_note_number, velocity);}
void ModLFO::note_off(const uint8_t midi_note_number) {m_player->note_off(midi_note_number);}

void ModLFO::render(float *output_buffer, const uint16_t number_samples) {
	const float sample_second = 1.f / m_sample_rate;

	for (uint16_t i = 0; i < number_samples; i++) {
		m_time_position += sample_second;
		m_time_position = m_time_position - std::floorf(m_time_position);

		p_apply_lfos();

		/*uint8_t chunk_size = m_render_chunk_size;
		if (number_samples - i > chunk_size) chunk_size = number_samples - i;*/

		m_player->render(output_buffer + (i * 2), 1/*chunk_size*/);

		//i += chunk_size;
	}
}

uint64_t ModLFO::store_calc_size(const bool store_reference) const {
	constexpr uint8_t constant_per_lfo = 1 + // BPM Synced
		1 + // Reversed
		4 + // Rate
		4 + // Anchor
		4 + // Scale
		1; // OSC

	uint32_t size = constant_per_lfo * m_lfos.size();
	for (const LFOInfo &lfo : m_lfos) size += lfo.param_id.length() + 1;

	return 1 + 1 + size + std::strlen(m_player->get_id()) + 1 + m_player->store_calc_size(store_reference);
}

void ModLFO::store(uint8_t *output_buffer, const bool store_reference) const {
	uint16_t offset = 0;

	output_buffer[offset++] = m_render_chunk_size;
	output_buffer[offset++] = m_lfos.size();

	for (const LFOInfo &lfo : m_lfos) {
		output_buffer[offset++] = lfo.bpm_synced;
		output_buffer[offset++] = lfo.reversed;
		write_float_be<float>(output_buffer + offset, lfo.rate); offset += 4;
		write_float_be<float>(output_buffer + offset, lfo.anchor); offset += 4;
		write_float_be<float>(output_buffer + offset, lfo.scale); offset += 4;
		output_buffer[offset++] = lfo.osc;
		std::memcpy(output_buffer + offset, lfo.param_id.c_str(), lfo.param_id.length() + 1); offset += lfo.param_id.length() + 1;
	}

	const uint8_t name_len = std::strlen(m_player->get_id()) + 1;
	std::memcpy(output_buffer + offset, m_player->get_id(), name_len); offset += name_len;

	m_player->store(output_buffer + offset, store_reference);
}

void ModLFO::load(const uint8_t *input_buffer) {
	uint16_t offset = 0;

	m_render_chunk_size = input_buffer[offset++];
	const uint8_t lfo_count = input_buffer[offset++];
	m_lfos.reserve(lfo_count);

	for (uint8_t i = 0; i < lfo_count; i++) {
		LFOInfo lfo;

		lfo.bpm_synced = input_buffer[offset++];
		lfo.reversed = input_buffer[offset++];
		lfo.rate = read_float_be<float>(input_buffer + offset); offset += 4;
		lfo.anchor = read_float_be<float>(input_buffer + offset); offset += 4;
		lfo.scale = read_float_be<float>(input_buffer + offset); offset += 4;
		lfo.osc = static_cast<BasicOscillator>(input_buffer[offset++]);

		lfo.param_id = std::string(reinterpret_cast<const char*>(input_buffer + offset));
		offset += lfo.param_id.length() + 1;

		m_lfos.push_back(lfo);
	}

	const std::string player_id{reinterpret_cast<const char*>(input_buffer + offset)};
	offset += player_id.length() + 1;
	try {set_wrapped_player(load_player_by_id(player_id));} catch (const std::exception &e) {ysbp_show_error(get_error_source(), (std::string("Error loading wrapped player:\n") + e.what()).c_str()); return;}
	if (!m_player) {ysbp_show_error(get_error_source(), "Player is nullptr"); return;}

	m_player->load(input_buffer + offset);
}

void ModLFO::render_graphics(YS_ImContextHandle im_context) {
	m_player->render_graphics(im_context);

	ImGui::SetCurrentContext(static_cast<ImGuiContext*>(im_context));

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::Text("%d / 255 LFOs", static_cast<uint8_t>(m_lfos.size()));

	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, UI::Colours::BUTTON_RED);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UI::Colours::BUTTON_RED_HOVER);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, UI::Colours::BUTTON_RED_ACTIVE);
	ImGui::PushFont(UI::g_imgui_icon_font);

	if (ImGui::Button(ICON_FA_TRASH, ImVec2(26.f, 26.f))) m_lfos.clear();

	ImGui::PopStyleColor(3);
	ImGui::PopFont();

	ImGui::SameLine();

	ImGui::SetNextItemWidth(26.f);
	ImGui::DragInt("Render Chunk Size", reinterpret_cast<int*>(&m_render_chunk_size), 1, 1, 64, "%d", ImGuiSliderFlags_ClampOnInput);
	if (ImGui::IsItemHovered()) {
		ImGui::BeginTooltip();
		ImGui::Text("LFO parameters are applied when audio is rendered.\nTo achieve this, audio is rendered in smaller chunks.\nLower = Smoother LFO\nHigher = Faster Rendering");
		ImGui::EndTooltip();
	}

	ImGui::BeginChild("##lfos", ImVec2(ImGui::GetContentRegionAvail().x, 220.f), 0, ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoTitleBar);

	std::vector<const char*> params(m_player->get_parameter_count());
	m_player->get_parameters(params.data());

	uint8_t i = 0;
	for (auto lfo = m_lfos.begin(); lfo != m_lfos.end(); i++) {
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(36.f / 255.f, 36.f / 255.f, 36.f / 255.f, 1.f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.f, 10.f));
		ImGui::BeginChild(("##lfo_" + std::to_string(i)).c_str(), ImVec2(200.f, 200.f), ImGuiChildFlags_Borders, ImGuiWindowFlags_NoDecoration);
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();

		if (ImGui::BeginCombo("##param", lfo->param_id.c_str())) {
			for (const char *param_id : params) {
				const bool is_selected = std::strcmp(param_id, lfo->param_id.c_str()) == 0;

				if (ImGui::Selectable(param_id, is_selected)) lfo->param_id = param_id;
				if (is_selected) ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, UI::Colours::BUTTON_RED);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UI::Colours::BUTTON_RED_HOVER);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, UI::Colours::BUTTON_RED_ACTIVE);
		ImGui::PushFont(UI::g_imgui_icon_font);

		if (ImGui::Button(ICON_FA_XMARK, ImVec2(26.f, 26.f))) {
			lfo = m_lfos.erase(lfo);
			ImGui::PopFont();
			ImGui::PopStyleColor(3);
			ImGui::EndChild();
			ImGui::SameLine();
			continue;
		}

		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		imgui_basic_oscillator_dropdown(&lfo->osc, "OSC");

		ImGui::Checkbox("BPM", &lfo->bpm_synced);
		ImGui::SameLine();
		ImGui::Checkbox("Reverse", &lfo->reversed);

		ImGui::DragFloat("Rate", &lfo->rate, 0.01f, 0.f, 5.f, "%.2f");
		ImGui::DragFloat("Anchor", &lfo->anchor, 0.01f, 0.f, 1.f, "%.2f");
		ImGui::DragFloat("Scale", &lfo->scale, 0.01f, 0.f, 1.f, "%.2f");

		ImGui::EndChild();

		ImGui::SameLine();
		++lfo;
	}

	if (m_lfos.size() < 255) {
		ImGui::PushStyleColor(ImGuiCol_Button, UI::Colours::BUTTON_GREEN);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, UI::Colours::BUTTON_GREEN_HOVER);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, UI::Colours::BUTTON_GREEN_ACTIVE);
		ImGui::PushFont(UI::g_imgui_icon_font_large);

		if (ImGui::Button(ICON_FA_PLUS, ImVec2(200.f, 200.f))) m_lfos.resize(m_lfos.size() + 1);

		ImGui::PopFont();
		ImGui::PopStyleColor(3);
	}

	ImGui::EndChild();
}

void ModLFO::reset() {
	m_time_position = 0.f;
	m_player->reset();
}

void ModLFO::p_apply_lfos() const {
	for (const LFOInfo &lfo : m_lfos) {
		if (lfo.bpm_synced) continue;
		const float value = lfo.reversed ?
			lfo.anchor + (1.f - calculate_basic_osc(lfo.osc, m_time_position * lfo.rate)) * lfo.scale
				: lfo.anchor + calculate_basic_osc(lfo.osc, m_time_position * lfo.rate) * lfo.scale;
		m_player->set_parameter(lfo.param_id.c_str(), value);
	}
}