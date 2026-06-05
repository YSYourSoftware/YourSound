#include "YourSound/UI/Controls.hpp"

#include <imgui-knobs.h>
#include <imgui.h>
#include <imgui_memory_editor.h>

extern "C" {
void ysbp_ui_begin_group() { ImGui::BeginGroup(); }
void ysbp_ui_end_group() { ImGui::EndGroup(); }

void ysbp_ui_same_line() { ImGui::SameLine(); }

ysbp_ui_Vec2 ysbp_ui_get_item_rect_size() {
	ImVec2 size = ImGui::GetItemRectSize();
	return {size.x, size.y};
}

void ysbp_ui_align_cursor_for_center(const float width) {
	ImGui::SetCursorPosX(std::ceilf(ImGui::GetCursorStartPos().x + (ImGui::GetContentRegionAvail().x - width) * 0.5f));
}

void ysbp_ui_text(const char *string) { ImGui::Text(string); }
void ysbp_ui_checkbox(const char *label, bool *value) { ImGui::Checkbox(label, value); }

bool ysbp_ui_knob(const char *label, float *p_value, const float v_min, const float v_max, const float speed,
				  const char *format, const ysbp_ui_ImGuiKnobVariant variant, const float size,
				  const ysbp_ui_ImGuiKnobFlags flags, const int steps, const float angle_min, const float angle_max) {
	return ImGuiKnobs::Knob(label, p_value, v_min, v_max, speed, format, variant, size, flags, steps, angle_min,
							angle_max);
}

bool ysbp_ui_begin_combo(const char *label, const char *preview_value, const ysbp_ui_ImGuiComboFlags flags) {
	return ImGui::BeginCombo(label, preview_value, flags);
}

bool ysbp_ui_combo_selectable(const char *label, const bool selected, const ysbp_ui_ImGuiSelectableFlags flags) {
	return ImGui::Selectable(label, selected, flags);
}

void ysbp_ui_combo_set_default_item_focus() { ImGui::SetItemDefaultFocus(); }
void ysbp_ui_end_combo() { ImGui::EndCombo(); }
}
