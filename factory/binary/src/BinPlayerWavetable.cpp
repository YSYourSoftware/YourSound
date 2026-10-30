#include "BinPlayerWavetable.hpp"

#include <YourSound/BinPlayerInterface.hpp>
#include <YourSound/Serialisation.hpp>
#include <YourSound/UI/BinPlayerInterfaceColours.hpp>
#include <YourSound/UI/Controls.hpp>
#include <YourSound/UI/Icons.hpp>

BinPlayerWavetable::BinPlayerWavetable() {
	std::fill_n(m_wavetable.data(), WAVE_SAMPLE_COUNT * WAVE_COUNT, NEUTRAL_VALUE);
}

void BinPlayerWavetable::note_on(const uint8_t midi_note_number, const float velocity) {
	m_polyphony_controller.note_on(midi_note_number, velocity);
}

void BinPlayerWavetable::note_off(const uint8_t midi_note_number) { m_polyphony_controller.note_off(midi_note_number); }

void BinPlayerWavetable::render(float *output_buffer, const uint16_t number_samples) {
	std::fill_n(output_buffer, number_samples * 2, 0.f);
	m_polyphony_controller.for_each_voice([number_samples, output_buffer, this](const auto &voice) -> bool {
		for (uint16_t i = 0; i < number_samples; i++) {
			voice->time += 1.f / m_polyphony_controller.sample_rate;

			voice->phase = std::fmodf(voice->phase + voice->phase_advance, 1.f);
			const float sample =
				((static_cast<float>(m_wavetable[m_selected_wave * WAVE_SAMPLE_COUNT +
												 static_cast<uint16_t>(std::round(voice->phase * WAVE_SAMPLE_COUNT))]) /
				  std::numeric_limits<uint16_t>::max() * 2.0) -
				 1.0) *
				YourSound::BinPlayer::calculate_amp_envelope(m_amp_envelope, voice->time, voice->releasing) *
				voice->velocity;

			// TODO: maybe add better voice mixing...
			output_buffer[i * 2] += sample;
			output_buffer[i * 2 + 1] += sample;
		}

		if (voice->releasing && voice->time > m_amp_envelope.release_time) return false;

		return true;
	});
}

uint64_t BinPlayerWavetable::store_calc_size(const bool store_reference) {
	return (WAVE_SAMPLE_COUNT * WAVE_COUNT * 2) + (1) + (4 + 4 + 4 + 4) + (4);
}

void BinPlayerWavetable::store(uint8_t *output_buffer, const bool store_reference) {
	std::memcpy(output_buffer, m_wavetable.data(), WAVE_SAMPLE_COUNT * WAVE_COUNT * 2);
	output_buffer += WAVE_SAMPLE_COUNT * WAVE_COUNT * 2;

	*(output_buffer++) = m_manual_morph;

	YourSound::write_float_be(output_buffer, m_amp_envelope.attack_time);
	output_buffer += 4;
	YourSound::write_float_be(output_buffer, m_amp_envelope.decay_time);
	output_buffer += 4;
	YourSound::write_float_be(output_buffer, m_amp_envelope.sustain_volume);
	output_buffer += 4;
	YourSound::write_float_be(output_buffer, m_amp_envelope.release_time);
	output_buffer += 4;

	YourSound::write_float_be(output_buffer, m_osc_rate);
	output_buffer += 4;
}

void BinPlayerWavetable::load(const uint8_t *input_buffer) {
	std::memcpy(m_wavetable.data(), input_buffer, WAVE_SAMPLE_COUNT * WAVE_COUNT * 2);
	input_buffer += WAVE_SAMPLE_COUNT * WAVE_COUNT * 2;

	m_manual_morph = *(input_buffer++);

	m_amp_envelope.attack_time = YourSound::read_float_be<float>(input_buffer);
	input_buffer += 4;
	m_amp_envelope.decay_time = YourSound::read_float_be<float>(input_buffer);
	input_buffer += 4;
	m_amp_envelope.sustain_volume = YourSound::read_float_be<float>(input_buffer);
	input_buffer += 4;
	m_amp_envelope.release_time = YourSound::read_float_be<float>(input_buffer);
	input_buffer += 4;

	m_osc_rate = YourSound::read_float_be<float>(input_buffer);
	input_buffer += 4;
}

void BinPlayerWavetable::set_parameter(const char *param_id, const float value) {
	if (std::strcmp(param_id, "_pitch_wheel") == 0) {
		m_polyphony_controller.pitch_bend_update((value * 2.f) - 1.f);
		return;
	}

	if (std::strcmp(param_id, "_midi_cc_64") == 0) {
		if (value >= 0.5f) m_polyphony_controller.sustain_on();
		else m_polyphony_controller.sustain_off();
		return;
	}

	if (std::strcmp(param_id, "_qparam_buttonlike_toggle_1") == 0 || std::strcmp(param_id, "manual_morph") == 0) {
		m_manual_morph = value >= 0.5f;
		return;
	}

	if (std::strcmp(param_id, "_midi_cc_1") == 0) {
		m_selected_wave = static_cast<uint16_t>(value * (WAVE_COUNT - 1));
		return;
	}

	if (std::strcmp(param_id, "_qparam_wheellike_1") == 0 || std::strcmp(param_id, "amp_env_attack_time") == 0) {
		m_amp_envelope.attack_time = value * 10.f;
		return;
	}

	if (std::strcmp(param_id, "_qparam_wheellike_2") == 0 || std::strcmp(param_id, "amp_env_decay_time") == 0) {
		m_amp_envelope.decay_time = value * 10.f;
		return;
	}

	if (std::strcmp(param_id, "_qparam_wheellike_3") == 0 || std::strcmp(param_id, "amp_env_sustain_vol") == 0) {
		m_amp_envelope.sustain_volume = value;
		return;
	}

	if (std::strcmp(param_id, "_qparam_wheellike_4") == 0 || std::strcmp(param_id, "amp_env_release_time") == 0) {
		m_amp_envelope.release_time = value * 10.f;
		return;
	}

	if (std::strcmp(param_id, "_qparam_wheellike_5") == 0 || std::strcmp(param_id, "osc_rate") == 0) {
		m_osc_rate = value * 2.f;
		return;
	}
}

float BinPlayerWavetable::get_parameter(const char *param_id) {
	if (std::strcmp(param_id, "_qparam_buttonlike_toggle_1") == 0 || std::strcmp(param_id, "manual_morph") == 0)
		return m_manual_morph ? 1.f : 0.f;
	if (std::strcmp(param_id, "_midi_cc_1") == 0) return m_selected_wave / (WAVE_COUNT - 1.f);
	if (std::strcmp(param_id, "_qparam_wheellike_1") == 0 || std::strcmp(param_id, "amp_env_attack_time") == 0)
		return m_amp_envelope.attack_time * 0.1f;
	if (std::strcmp(param_id, "_qparam_wheellike_2") == 0 || std::strcmp(param_id, "amp_env_decay_time") == 0)
		return m_amp_envelope.decay_time * 0.1f;
	if (std::strcmp(param_id, "_qparam_wheellike_3") == 0 || std::strcmp(param_id, "amp_env_sustain_vol") == 0)
		return m_amp_envelope.sustain_volume;
	if (std::strcmp(param_id, "_qparam_wheellike_4") == 0 || std::strcmp(param_id, "amp_env_release_time") == 0)
		return m_amp_envelope.release_time * 0.1f;
	if (std::strcmp(param_id, "_qparam_wheellike_5") == 0 || std::strcmp(param_id, "osc_rate") == 0)
		return m_osc_rate * 0.5f;
	return 0.f;
}

void BinPlayerWavetable::get_parameters(const char **buffer) {
	buffer[0] = "manual_morph";
	buffer[1] = "amp_env_attack_time";
	buffer[2] = "amp_env_decay_time";
	buffer[3] = "amp_env_sustain_vol";
	buffer[4] = "amp_env_release_time";
	buffer[5] = "osc_rate";
}

static void set_all_wavetable_frames_to_basic_osc(std::array<uint16_t, WAVE_SAMPLE_COUNT * WAVE_COUNT> &wavetable,
												  const YourSound::BinPlayer::BasicOscillator osc) {
	if (!ysbp_ask_yesno("Are you sure you want to load a new wavetable?",
						"All frames of the current wavetable will be lost.", true))
		return;

	for (uint16_t i = 0; i < WAVE_COUNT; i++) {
		for (uint16_t v = 0; v < WAVE_SAMPLE_COUNT; v++) {
			wavetable[(i * WAVE_SAMPLE_COUNT) + v] = static_cast<uint16_t>(
				std::roundf(YourSound::BinPlayer::calculate_basic_osc(osc, static_cast<float>(v) / WAVE_SAMPLE_COUNT) *
							std::numeric_limits<uint16_t>::max()));
		}
	}
}

#define PLACE_BACK_BUTTON()                                                                                            \
	do {                                                                                                               \
		if (ysbp_ui_arrow_button("##wav-edit-mode-back", ysbp_ui_Direction_Left)) m_edit_mode = NONE;                  \
		ysbp_ui_same_line();                                                                                           \
	} while (false)
#define PLACE_MODE_TOOLTIP(mode)                                                                                       \
	do {                                                                                                               \
		if (ysbp_ui_is_hovering()) {                                                                                   \
			ysbp_ui_begin_tooltip();                                                                                   \
			ysbp_ui_text(mode_tooltips[mode].data());                                                                  \
			ysbp_ui_end_tooltip();                                                                                     \
		}                                                                                                              \
	} while (false)
#define PUSH_FONT_AND_COLOURS()                                                                                        \
	do {                                                                                                               \
		ysbp_ui_push_font(ysbp_ui_FONT_ICON);                                                                          \
                                                                                                                       \
		ysbp_ui_push_style_colour(ysbp_ui_ImGuiCol_Button, ysbp_ui_colours_BUTTON_GREEN);                              \
		ysbp_ui_push_style_colour(ysbp_ui_ImGuiCol_ButtonHovered, ysbp_ui_colours_BUTTON_GREEN);                       \
		ysbp_ui_push_style_colour(ysbp_ui_ImGuiCol_ButtonActive, ysbp_ui_colours_BUTTON_GREEN);                        \
	} while (false)
#define PLACE_MODE_SELECT_BUTTON(edit_mode, icon)                                                                      \
	do {                                                                                                               \
		ysbp_ui_set_frame_padding({frame_padding.x, -20.f});                                                           \
		ysbp_ui_push_font(ysbp_ui_FONT_ICON);                                                                          \
		if (ysbp_ui_button(icon, {26.f, 26.f})) { m_edit_mode = edit_mode; }                                           \
		ysbp_ui_pop_font();                                                                                            \
		ysbp_ui_set_frame_padding(frame_padding);                                                                      \
		PLACE_MODE_TOOLTIP(edit_mode);                                                                                 \
	} while (false)
#define PLACE_MODE_NOTIFY_BUTTON(icon)                                                                                 \
	do {                                                                                                               \
		ysbp_ui_set_frame_padding({frame_padding.x, -20.f});                                                           \
		PUSH_FONT_AND_COLOURS();                                                                                       \
		ysbp_ui_button(icon, {26.f, 26.f});                                                                            \
		ysbp_ui_pop_font();                                                                                            \
		ysbp_ui_set_frame_padding(frame_padding);                                                                      \
	} while (false)

void BinPlayerWavetable::render_graphics() {
	ysbp_ui_align_cursor_for_center(m_wavetable_view_width);
	ysbp_ui_begin_group();

	constexpr static uint8_t top_row_button_count = 4;
	const static float top_row_button_size =
		(500.f - (ysbp_ui_get_item_spacing().x * (top_row_button_count - 1))) / top_row_button_count;

	if (ysbp_ui_button("Edit...", {top_row_button_size, 0})) {
		ysbp_ui_open_popup("Wavetable Editor");
		m_edit_open = true;
	}

	if (ysbp_ui_begin_modal_popup("Wavetable Editor", &m_edit_open, ysbp_ui_ImGuiWindowFlags_NoResize)) {
		if (ysbp_ui_button(m_edit_3d ? "3D" : "2D", {26.f, 26.f})) m_edit_3d = !m_edit_3d;
		if (ysbp_ui_is_hovering()) {
			ysbp_ui_begin_tooltip();
			ysbp_ui_text("2D: One frame can be edited at a time, used for drawing, etc.\n3D: All frames are edited at "
						 "once, used for smoothing, jumble, etc.");
			ysbp_ui_end_tooltip();
		}

		ysbp_ui_spacing();
		ysbp_ui_separator();
		ysbp_ui_spacing();

		if (m_edit_3d) {

		} else {
			constexpr static std::array<std::string_view, EDIT_MODE_MAXIMUM> mode_tooltips = {
				"",
				"Pencil mode:\nDrag on the wave with the mouse to move points.",
				"Line mode:\nDrag from one point to another to create a flat line.",
				"Eyedropper mode:\nSelect a point to use as its value as the centre for flatten & smooth operations.",
				"Flatten mode:\nDrag from one point to another to flatten that area.",
				"Quantize mode:\nDrag from one point to another to quantize that area."};

			const static ysbp_ui_Vec2 frame_padding = ysbp_ui_get_frame_padding();
			const static ysbp_ui_Vec2 graph_size = {500.f - (frame_padding.x * 2.f), 200.f - (frame_padding.y * 2.f)};
			const ysbp_ui_Vec2 mouse_pos = ysbp_ui_get_mouse_pos();

			switch (m_edit_mode) {
			case PENCIL_MODE: {
				PLACE_BACK_BUTTON();
				PLACE_MODE_NOTIFY_BUTTON(ICON_FA_PENCIL);
				PLACE_MODE_TOOLTIP(PENCIL_MODE);

				ysbp_ui_pop_style_colour(3);

				const ysbp_ui_Vec2 graph_pos = {ysbp_ui_get_cursor_pos().x + frame_padding.x,
												ysbp_ui_get_cursor_pos().y + frame_padding.y};

				static bool has_previous_mouse = false;
				static ysbp_ui_Vec2 previous_mouse_pos{};

				if (std::clamp(mouse_pos.x, graph_pos.x, graph_pos.x + graph_size.x) != mouse_pos.x ||
					std::clamp(mouse_pos.y, graph_pos.y, graph_pos.y + graph_size.y) != mouse_pos.y ||
					!ysbp_ui_is_mouse_down(ysbp_ui_MouseButton_Left)) {
					has_previous_mouse = false;
					break;
				}

				if (!has_previous_mouse) previous_mouse_pos = mouse_pos;

				const float graph_hover_x = std::clamp((mouse_pos.x - graph_pos.x) / graph_size.x, 0.f, 1.f);
				const float graph_hover_y =
					std::clamp((mouse_pos.y - graph_pos.y) / graph_size.y, 0.f, 1.f) * -1.f + 1.f;

				const float graph_prev_hover_x =
					std::clamp((previous_mouse_pos.x - graph_pos.x) / graph_size.x, 0.f, 1.f);
				const float graph_prev_hover_y =
					std::clamp((previous_mouse_pos.y - graph_pos.y) / graph_size.y, 0.f, 1.f) * -1.f + 1.f;

				auto first = static_cast<uint16_t>(std::round(graph_hover_x * (WAVE_SAMPLE_COUNT - 1)));
				auto last = static_cast<uint16_t>(std::round(graph_prev_hover_x * (WAVE_SAMPLE_COUNT - 1)));

				auto first_val =
					static_cast<uint16_t>(std::round(graph_hover_y * std::numeric_limits<uint16_t>::max()));
				auto last_val =
					static_cast<uint16_t>(std::round(graph_prev_hover_y * std::numeric_limits<uint16_t>::max()));

				if (first > last) {
					// xor swap
					first ^= last;
					last ^= first;
					first ^= last;

					first_val ^= last_val;
					last_val ^= first_val;
					first_val ^= last_val;
				}

				for (uint16_t i = first; i < last; i++) {
					m_wavetable[(WAVE_SAMPLE_COUNT * m_edit_selected_wave) + i] = static_cast<uint16_t>(
						std::round(std::lerp(static_cast<float>(first_val), static_cast<float>(last_val),
											 static_cast<float>(i - first) / static_cast<float>(last - first))));
				}

				previous_mouse_pos = mouse_pos;
				has_previous_mouse = true;
				break;
			}
			case LINE_MODE: {
				PLACE_BACK_BUTTON();
				PLACE_MODE_NOTIFY_BUTTON(ICON_FA_PEN_RULER);
				PLACE_MODE_TOOLTIP(LINE_MODE);

				ysbp_ui_pop_style_colour(3);

				const ysbp_ui_Vec2 graph_pos = {ysbp_ui_get_cursor_pos().x + frame_padding.x,
												ysbp_ui_get_cursor_pos().y + frame_padding.y};

				static ysbp_ui_Vec2 start_pos = {INFINITY, INFINITY};

				if (std::clamp(mouse_pos.x, graph_pos.x, graph_pos.x + graph_size.x) != mouse_pos.x ||
					std::clamp(mouse_pos.y, graph_pos.y, graph_pos.y + graph_size.y) != mouse_pos.y ||
					!ysbp_ui_is_mouse_down(ysbp_ui_MouseButton_Left)) {
					if (start_pos.x != INFINITY) {
						const float graph_hover_x = std::clamp((mouse_pos.x - graph_pos.x) / graph_size.x, 0.f, 1.f);
						const float graph_hover_y =
							std::clamp((mouse_pos.y - graph_pos.y) / graph_size.y, 0.f, 1.f) * -1.f + 1.f;

						const float graph_start_hover_x =
							std::clamp((start_pos.x - graph_pos.x) / graph_size.x, 0.f, 1.f);
						const float graph_start_hover_y =
							std::clamp((start_pos.y - graph_pos.y) / graph_size.y, 0.f, 1.f) * -1.f + 1.f;

						auto first = static_cast<uint16_t>(std::round(graph_start_hover_x * (WAVE_SAMPLE_COUNT - 1)));
						auto last = static_cast<uint16_t>(std::round(graph_hover_x * (WAVE_SAMPLE_COUNT - 1)));

						auto first_val = static_cast<uint16_t>(
							std::round(graph_start_hover_y * std::numeric_limits<uint16_t>::max()));
						auto last_val =
							static_cast<uint16_t>(std::round(graph_hover_y * std::numeric_limits<uint16_t>::max()));

						if (first > last) {
							// xor swap
							first ^= last;
							last ^= first;
							first ^= last;

							first_val ^= last_val;
							last_val ^= first_val;
							first_val ^= last_val;
						}

						for (uint16_t i = first; i < last; i++) {
							m_wavetable[(WAVE_SAMPLE_COUNT * m_edit_selected_wave) + i] =
								static_cast<uint16_t>(std::round(
									std::lerp(static_cast<float>(first_val), static_cast<float>(last_val),
											  static_cast<float>(i - first) / static_cast<float>(last - first))));
						}

						start_pos = {INFINITY, INFINITY};
					}

					break;
				}

				if (start_pos.x == INFINITY) start_pos = mouse_pos;

				ysbp_ui_draw_line(start_pos, mouse_pos, {0.f, 255.f, 0.f, 255.f});
				ysbp_ui_draw_line({start_pos.x, graph_pos.y}, {start_pos.x, graph_pos.y + graph_size.y},
								  {255.f, 0.f, 0.f, 128.f});
				ysbp_ui_draw_line({mouse_pos.x, graph_pos.y}, {mouse_pos.x, graph_pos.y + graph_size.y},
								  {255.f, 0.f, 0.f, 128.f});

				break;
			}
			case EYEDROPPER_MODE: {
				PLACE_BACK_BUTTON();
				PLACE_MODE_NOTIFY_BUTTON(ICON_FA_EYE_DROPPER);
				PLACE_MODE_TOOLTIP(EYEDROPPER_MODE);

				ysbp_ui_pop_style_colour(3);

				ysbp_ui_same_line();
				ysbp_ui_text(std::format("Selected: {:.2f}", m_edit_mode_eyedropper_value).c_str());

				const ysbp_ui_Vec2 graph_pos = {ysbp_ui_get_cursor_pos().x + frame_padding.x,
												ysbp_ui_get_cursor_pos().y + frame_padding.y};

				if (std::clamp(mouse_pos.x, graph_pos.x, graph_pos.x + graph_size.x) != mouse_pos.x ||
					std::clamp(mouse_pos.y, graph_pos.y, graph_pos.y + graph_size.y) != mouse_pos.y) {
					break;
				}

				ysbp_ui_Vec4 line_colour{};
				if (ysbp_ui_is_mouse_down(ysbp_ui_MouseButton_Left)) line_colour = {0.f, 255.f, 0.f, 128.f};
				else line_colour = {255.f, 255.f, 0.f, 128.f};

				const float graph_hover_x = std::clamp((mouse_pos.x - graph_pos.x) / graph_size.x, 0.f, 1.f);
				const float graph_hover_y =
					(static_cast<float>(
						 m_wavetable[(WAVE_SAMPLE_COUNT * m_edit_selected_wave) +
									 static_cast<uint16_t>(std::roundf(graph_hover_x * WAVE_SAMPLE_COUNT))]) /
						 std::numeric_limits<uint16_t>::max() * -1.f +
					 1.f) *
					graph_size.y;

				ysbp_ui_draw_line({mouse_pos.x, graph_pos.y}, {mouse_pos.x, graph_pos.y + graph_size.y}, line_colour);
				ysbp_ui_draw_line({graph_pos.x, graph_hover_y + graph_pos.y},
								  {graph_pos.x + graph_size.x, graph_hover_y + graph_pos.y}, line_colour);

				if (ysbp_ui_is_mouse_down(ysbp_ui_MouseButton_Left)) {
					m_edit_mode_eyedropper_value =
						static_cast<float>(
							m_wavetable[(WAVE_SAMPLE_COUNT * m_edit_selected_wave) +
										static_cast<uint16_t>(std::roundf(graph_hover_x * WAVE_SAMPLE_COUNT))]) /
						std::numeric_limits<uint16_t>::max();
				}
				break;
			}
			case FLATTEN_MODE: {
				PLACE_BACK_BUTTON();
				PLACE_MODE_NOTIFY_BUTTON(ICON_FA_ARROWS_DOWN_TO_LINE);
				PLACE_MODE_TOOLTIP(FLATTEN_MODE);

				ysbp_ui_pop_style_colour(3);

				ysbp_ui_same_line();
				ysbp_ui_set_next_item_width(50.f);
				ysbp_ui_float_drag("Target", &m_edit_mode_eyedropper_value, 0.01f, 0.f, 1.f, "%.2f",
								   ysbp_ui_ImGuiSliderFlags_AlwaysClamp);

				const ysbp_ui_Vec2 graph_pos = {ysbp_ui_get_cursor_pos().x + frame_padding.x,
												ysbp_ui_get_cursor_pos().y + frame_padding.y};

				const bool mouse_over_graph =
					(std::clamp(mouse_pos.x, graph_pos.x, graph_pos.x + graph_size.x) == mouse_pos.x &&
					 std::clamp(mouse_pos.y, graph_pos.y, graph_pos.y + graph_size.y) == mouse_pos.y);

				static float start_point = -1.f;
				static float end_point = -1.f;

				if (ysbp_ui_is_mouse_down(ysbp_ui_MouseButton_Left) && mouse_over_graph) {
					const float graph_hover_x = std::clamp((mouse_pos.x - graph_pos.x) / graph_size.x, 0.f, 1.f);

					if (start_point == -1.f) start_point = graph_hover_x;
					end_point = graph_hover_x;

					ysbp_ui_draw_line({graph_pos.x + (graph_size.x * start_point),
									   graph_pos.y + (graph_size.y * (m_edit_mode_eyedropper_value * -1.f + 1.f))},
									  {graph_pos.x + (graph_size.x * end_point),
									   graph_pos.y + (graph_size.y * (m_edit_mode_eyedropper_value * -1.f + 1.f))},
									  {0.f, 255.f, 0.f, 255.f});
					ysbp_ui_draw_line({graph_pos.x + (graph_size.x * start_point), graph_pos.y},
									  {graph_pos.x + (graph_size.x * start_point), graph_pos.y + graph_size.y},
									  {255.f, 0.f, 0.f, 128.f});
					ysbp_ui_draw_line({graph_pos.x + (graph_size.x * end_point), graph_pos.y},
									  {graph_pos.x + (graph_size.x * end_point), graph_pos.y + graph_size.y},
									  {255.f, 0.f, 0.f, 128.f});
				} else {
					if (start_point != -1.f && end_point != -1.f) {
						uint16_t start_u16 = static_cast<uint16_t>(start_point * WAVE_SAMPLE_COUNT);
						uint16_t end_u16 = static_cast<uint16_t>(end_point * WAVE_SAMPLE_COUNT);

						if (start_point > end_point) {
							// xor swap
							start_u16 ^= end_u16;
							end_u16 ^= start_u16;
							start_u16 ^= end_u16;
						}

						for (uint16_t i = start_u16; i < end_u16; i++)
							m_wavetable[(WAVE_SAMPLE_COUNT * m_edit_selected_wave) + i] = static_cast<uint16_t>(
								std::round(m_edit_mode_eyedropper_value * std::numeric_limits<uint16_t>::max()));
					}

					start_point = -1.f;
					end_point = -1.f;
				}

				ysbp_ui_draw_line(
					{graph_pos.x, graph_pos.y + (graph_size.y * (m_edit_mode_eyedropper_value * -1.f + 1.f))},
					{graph_pos.x + graph_size.x,
					 graph_pos.y + (graph_size.y * (m_edit_mode_eyedropper_value * -1.f + 1.f))},
					{255.f, 255.f, 0.f, 64.f});

				break;
			}
			case QUANTIZE_MODE: {
				PLACE_BACK_BUTTON();
				PLACE_MODE_NOTIFY_BUTTON(ICON_FA_WAVE_SQUARE);
				PLACE_MODE_TOOLTIP(QUANTIZE_MODE);

				ysbp_ui_pop_style_colour(3);
				break;
			}
			default: {
				PLACE_MODE_SELECT_BUTTON(PENCIL_MODE, ICON_FA_PENCIL);
				ysbp_ui_same_line();
				PLACE_MODE_SELECT_BUTTON(LINE_MODE, ICON_FA_PEN_RULER);
				ysbp_ui_same_line();
				PLACE_MODE_SELECT_BUTTON(EYEDROPPER_MODE, ICON_FA_EYE_DROPPER);
				ysbp_ui_same_line();
				PLACE_MODE_SELECT_BUTTON(FLATTEN_MODE, ICON_FA_ARROWS_DOWN_TO_LINE);
				ysbp_ui_same_line();
				PLACE_MODE_SELECT_BUTTON(QUANTIZE_MODE, ICON_FA_WAVE_SQUARE);
				break;
			}
			}

			ysbp_ui_line_graph("",
							   [](void *data, const int idx) -> float {
								   const auto *samples = static_cast<uint16_t *>(data);
								   return samples[idx] / static_cast<float>(UINT16_MAX);
							   },
							   m_wavetable.data() + (WAVE_SAMPLE_COUNT * m_edit_selected_wave), WAVE_SAMPLE_COUNT, 0,
							   "", 0.f, 1.f, {500.f, 200.f});

			if (ysbp_ui_arrow_button("##wav-edit-select-decrement", ysbp_ui_Direction_Left))
				m_edit_selected_wave = std::clamp(m_edit_selected_wave - 1, 0, WAVE_COUNT - 1);

			ysbp_ui_same_line();

			const static float edit_select_slider_width = 500.f - (ysbp_ui_get_item_spacing().x * 2.f) - (26.f * 2.f);

			int32_t select_u32 = m_edit_selected_wave + 1;
			ysbp_ui_set_next_item_width(edit_select_slider_width);
			if (ysbp_ui_int_slider("##wav-edit-select", &select_u32, 1, WAVE_COUNT, "Frame %d",
								   ysbp_ui_ImGuiSliderFlags_AlwaysClamp))
				m_edit_selected_wave = std::clamp(select_u32 - 1, 0, WAVE_COUNT - 1);

			ysbp_ui_same_line();

			if (ysbp_ui_arrow_button("##wav-edit-select-increment", ysbp_ui_Direction_Right))
				m_edit_selected_wave = std::clamp(m_edit_selected_wave + 1, 0, WAVE_COUNT - 1);
		}

		ysbp_ui_end_popup();
	}

	ysbp_ui_same_line();
	if (ysbp_ui_button("Load...", {top_row_button_size, 0})) { ysbp_ui_open_popup("##load-menu-wavtable"); }
	if (ysbp_ui_begin_popup("##load-menu-wavtable")) {
		ysbp_ui_menu_item("From File...");
		if (ysbp_ui_begin_menu("Basic Shapes")) {
			if (ysbp_ui_menu_item("Square"))
				set_all_wavetable_frames_to_basic_osc(m_wavetable, YourSound::BinPlayer::SQUARE);
			if (ysbp_ui_menu_item("Triangle"))
				set_all_wavetable_frames_to_basic_osc(m_wavetable, YourSound::BinPlayer::TRIANGLE);
			if (ysbp_ui_menu_item("Sine"))
				set_all_wavetable_frames_to_basic_osc(m_wavetable, YourSound::BinPlayer::SINE);
			if (ysbp_ui_menu_item("Sawtooth"))
				set_all_wavetable_frames_to_basic_osc(m_wavetable, YourSound::BinPlayer::SAWTOOTH);
			if (ysbp_ui_menu_item("Noise"))
				set_all_wavetable_frames_to_basic_osc(m_wavetable, YourSound::BinPlayer::NOISE);
			ysbp_ui_end_menu();
		}

		ysbp_ui_end_popup();
	}

	ysbp_ui_same_line();
	ysbp_ui_button("Save...", {top_row_button_size, 0});
	ysbp_ui_same_line();
	ysbp_ui_push_style_colour(ysbp_ui_ImGuiCol_Button, ysbp_ui_colours_BUTTON_RED);
	ysbp_ui_push_style_colour(ysbp_ui_ImGuiCol_ButtonHovered, ysbp_ui_colours_BUTTON_RED_HOVER);
	ysbp_ui_push_style_colour(ysbp_ui_ImGuiCol_ButtonActive, ysbp_ui_colours_BUTTON_RED_ACTIVE);
	if (ysbp_ui_button("Reset", {top_row_button_size, 0})) {
		if (ysbp_ask_yesno("Are you sure you want to reset this wavetable?",
						   "You'll lose all your work, and won't be able to recover it.", true)) {
			std::fill_n(m_wavetable.data(), WAVE_SAMPLE_COUNT * WAVE_COUNT, NEUTRAL_VALUE);
		}
	}

	ysbp_ui_pop_style_colour(3);

	ysbp_ui_line_graph("",
					   [](void *data, const int idx) -> float {
						   const auto *samples = static_cast<uint16_t *>(data);
						   return samples[idx] / static_cast<float>(UINT16_MAX);
					   },
					   m_wavetable.data() + (WAVE_SAMPLE_COUNT * m_selected_wave), WAVE_SAMPLE_COUNT, 0, "", 0.f, 1.f,
					   {500.f, 200.f});

	if (m_manual_morph) {
		int32_t manual_morph_u32 = m_selected_wave;
		ysbp_ui_set_next_item_width(500.f);
		if (ysbp_ui_int_slider("##manual-morph", &manual_morph_u32, 0, WAVE_COUNT - 1, "%d",
							   ysbp_ui_ImGuiSliderFlags_AlwaysClamp))
			m_selected_wave = manual_morph_u32;
	}
	ysbp_ui_end_group();

	if (const float current_width = ysbp_ui_get_item_rect_size().x; current_width > 0.0f)
		m_wavetable_view_width = current_width;

	ysbp_ui_align_cursor_for_center(m_checkbox_width);
	ysbp_ui_begin_group();
	ysbp_ui_checkbox("Manual Morph", &m_manual_morph);
	ysbp_ui_end_group();
	if (const float current_width = ysbp_ui_get_item_rect_size().x; current_width > 0.0f)
		m_checkbox_width = current_width;
}
