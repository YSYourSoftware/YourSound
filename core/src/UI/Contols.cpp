#include "YourSound/UI/Controls.hpp"

#include "YourSound/UI/Fonts.hpp"

#include <imgui-knobs.h>
#include <imgui.h>

#include <cmath>

extern "C" {
void ysbp_ui_begin_group() { ImGui::BeginGroup(); }
void ysbp_ui_end_group() { ImGui::EndGroup(); }
void ysbp_ui_begin_child(const char *str_id, const ysbp_ui_Vec2 size, const ysbp_ui_ImGuiChildFlags child_flags,
						 const ysbp_ui_ImGuiWindowFlags window_flags) {
	ImGui::BeginChild(str_id, {size.x, size.y}, child_flags, window_flags);
}

void ysbp_ui_end_child() { ImGui::EndChild(); }
void ysbp_ui_same_line() { ImGui::SameLine(); }
ysbp_ui_Vec2 ysbp_ui_get_item_rect_size() {
	const ImVec2 size = ImGui::GetItemRectSize();
	return {size.x, size.y};
}

ysbp_ui_Vec2 ysbp_ui_get_content_region_avail() {
	const ImVec2 size = ImGui::GetContentRegionAvail();
	return {size.x, size.y};
}

void ysbp_ui_align_cursor_for_center(const float width) {
	ImGui::SetCursorPosX(std::ceilf(ImGui::GetCursorStartPos().x + (ImGui::GetContentRegionAvail().x - width) * 0.5f));
}

bool ysbp_ui_is_hovering() { return ImGui::IsItemHovered(); }
void ysbp_ui_text(const char *string) { ImGui::Text(string); }
bool ysbp_ui_button(const char *label, const ysbp_ui_Vec2 size) { return ImGui::Button(label, {size.x, size.y}); }
bool ysbp_ui_checkbox(const char *label, bool *value) { return ImGui::Checkbox(label, value); }
bool ysbp_ui_knob(const char *label, float *p_value, const float v_min, const float v_max, const float speed,
				  const char *format, const ysbp_ui_ImGuiKnobVariant variant, const float size,
				  const ysbp_ui_ImGuiKnobFlags flags, const int steps, const float angle_min, const float angle_max) {
	return ImGuiKnobs::Knob(label, p_value, v_min, v_max, speed, format, variant, size, flags, steps, angle_min,
							angle_max);
}

bool ysbp_ui_float_slider(const char *label, float *v, const float v_min, const float v_max, const char *format,
						  const ysbp_ui_ImGuiSliderFlags flags) {
	return ImGui::SliderFloat(label, v, v_min, v_max, format, flags);
}

bool ysbp_ui_float_slider_vertical(const char *label, const ysbp_ui_Vec2 size, float *v, const float v_min,
								   const float v_max, const char *format, const ysbp_ui_ImGuiSliderFlags flags) {
	return ImGui::VSliderFloat(label, {size.x, size.y}, v, v_min, v_max, format, flags);
}

bool ysbp_ui_float_drag(const char *label, float *v, const float v_speed, const float v_min, const float v_max,
						const char *format, const ysbp_ui_ImGuiSliderFlags flags) {
	return ImGui::DragFloat(label, v, v_speed, v_min, v_max, format, flags);
}

bool ysbp_ui_int_slider(const char *label, int *v, const int v_min, const int v_max, const char *format,
						const ysbp_ui_ImGuiSliderFlags flags) {
	return ImGui::SliderInt(label, v, v_min, v_max, format, flags);
}

bool ysbp_ui_int_drag(const char *label, int *v, const int v_speed, const int v_min, const int v_max,
					  const char *format, const ysbp_ui_ImGuiSliderFlags flags) {
	return ImGui::DragInt(label, v, v_speed, v_min, v_max, format, flags);
}

bool ysbp_ui_begin_combo(const char *label, const char *preview_value, const ysbp_ui_ImGuiComboFlags flags) {
	return ImGui::BeginCombo(label, preview_value, flags);
}

bool ysbp_ui_combo_selectable(const char *label, const bool selected, const ysbp_ui_ImGuiSelectableFlags flags) {
	return ImGui::Selectable(label, selected, flags);
}

void ysbp_ui_combo_set_default_item_focus() { ImGui::SetItemDefaultFocus(); }
void ysbp_ui_end_combo() { ImGui::EndCombo(); }
bool ysbp_ui_begin_tooltip() { return ImGui::BeginTooltip(); }
void ysbp_ui_end_tooltip() { ImGui::EndTooltip(); }

void ysbp_ui_push_style_colour(const ysbp_ui_ImGuiCol idx, const ysbp_ui_Vec4 col) {
	ImGui::PushStyleColor(idx, {col.x, col.y, col.z, col.w});
}

void ysbp_ui_pop_style_colour(const int count) { ImGui::PopStyleColor(count); }
void ysbp_ui_push_style_var(const ysbp_ui_ImGuiStyleVar idx, const ysbp_ui_Vec2 val) {
	ImGui::PushStyleVar(idx, {val.x, val.y});
}

void ysbp_ui_pop_style_var(const int count) { ImGui::PopStyleVar(count); }

void ysbp_ui_push_font(const ysbp_ui_Font font) {
	switch (font) {
	case ysbp_ui_FONT_STANDARD_LARGE: ImGui::PushFont(YourSound::UI::g_imgui_standard_font_large); break;
	case ysbp_ui_FONT_ICON: ImGui::PushFont(YourSound::UI::g_imgui_icon_font); break;
	case ysbp_ui_FONT_ICON_LARGE: ImGui::PushFont(YourSound::UI::g_imgui_icon_font_large); break;
	case ysbp_ui_FONT_MONO: ImGui::PushFont(YourSound::UI::g_imgui_mono_font); break;
	case ysbp_ui_FONT_MONO_BOLD: ImGui::PushFont(YourSound::UI::g_imgui_mono_font_bold); break;
	case ysbp_ui_FONT_STANDARD_MEDIUM: ImGui::PushFont(YourSound::UI::g_imgui_standard_font_medium); break;
	}
}

void ysbp_ui_pop_font() { ImGui::PopFont(); }

void ysbp_ui_line_graph_data(const char *label, const float *values, const int values_count, const int values_offset,
							 const char *overlay_text, const float scale_min, const float scale_max,
							 ysbp_ui_Vec2 graph_size, const int stride) {
	ImGui::PlotLines(label, values, values_count, values_offset, overlay_text, scale_min, scale_max,
					 {graph_size.x, graph_size.y}, stride);
}

void ysbp_ui_line_graph(const char *label, float (*values_getter)(void *data, int idx), void *data,
						const int values_count, const int values_offset, const char *overlay_text,
						const float scale_min, const float scale_max, ysbp_ui_Vec2 graph_size) {
	ImGui::PlotLines(label, values_getter, data, values_count, values_offset, overlay_text, scale_min, scale_max,
					 {graph_size.x, graph_size.y});
}

void ysbp_ui_set_next_item_width(const float width) { ImGui::SetNextItemWidth(width); }

ysbp_ui_Vec2 ysbp_ui_get_item_spacing() {
	const ImVec2 &item_spacing = ImGui::GetStyle().ItemSpacing;
	return {item_spacing.x, item_spacing.y};
}

float ysbp_ui_get_frame_height() { return ImGui::GetFrameHeight(); }
void ysbp_ui_spacing() { ImGui::Spacing(); }
}

namespace YourSound::UI {
	void set_imgui_context(void *im_context) { ImGui::SetCurrentContext(static_cast<ImGuiContext *>(im_context)); }
} // namespace YourSound::UI
