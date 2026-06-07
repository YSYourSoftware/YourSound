#pragma once

#include "YourSound/BinPlayerInterface.hpp"

typedef int ysbp_ui_ImGuiKnobFlags;

enum ysbp_ui_ImGuiKnobFlags_ {
	ysbp_ui_ImGuiKnobFlags_NoTitle = 1 << 0,
	ysbp_ui_ImGuiKnobFlags_NoInput = 1 << 1,
	ysbp_ui_ImGuiKnobFlags_ValueTooltip = 1 << 2,
	ysbp_ui_ImGuiKnobFlags_DragHorizontal = 1 << 3,
	ysbp_ui_ImGuiKnobFlags_DragVertical = 1 << 4,
	ysbp_ui_ImGuiKnobFlags_Logarithmic = 1 << 5,
	ysbp_ui_ImGuiKnobFlags_AlwaysClamp = 1 << 6
};

typedef int ysbp_ui_ImGuiKnobVariant;

enum ysbp_ui_ImGuiKnobVariant_ {
	ysbp_ui_ImGuiKnobVariant_Tick = 1 << 0,
	ysbp_ui_ImGuiKnobVariant_Dot = 1 << 1,
	ysbp_ui_ImGuiKnobVariant_Wiper = 1 << 2,
	ysbp_ui_ImGuiKnobVariant_WiperOnly = 1 << 3,
	ysbp_ui_ImGuiKnobVariant_WiperDot = 1 << 4,
	ysbp_ui_ImGuiKnobVariant_Stepped = 1 << 5,
	ysbp_ui_ImGuiKnobVariant_Space = 1 << 6,
};

typedef int ysbp_ui_ImGuiComboFlags;

enum ysbp_ui_ImGuiComboFlags_ {
	ysbp_ui_ImGuiComboFlags_None = 0,
	ysbp_ui_ImGuiComboFlags_PopupAlignLeft = 1 << 0,
	ysbp_ui_ImGuiComboFlags_HeightSmall = 1 << 1,
	ysbp_ui_ImGuiComboFlags_HeightRegular = 1 << 2,
	ysbp_ui_ImGuiComboFlags_HeightLarge = 1 << 3,
	ysbp_ui_ImGuiComboFlags_HeightLargest = 1 << 4,
	ysbp_ui_ImGuiComboFlags_NoArrowButton = 1 << 5,
	ysbp_ui_ImGuiComboFlags_NoPreview = 1 << 6,
	ysbp_ui_ImGuiComboFlags_WidthFitPreview = 1 << 7,
};

typedef int ysbp_ui_ImGuiSelectableFlags;

enum ysbp_ui_ImGuiSelectableFlags_ {
	ysbp_ui_ImGuiSelectableFlags_None = 0,
	ysbp_ui_ImGuiSelectableFlags_NoAutoClosePopups = 1 << 0,
	ysbp_ui_ImGuiSelectableFlags_SpanAllColumns = 1 << 1,
	ysbp_ui_ImGuiSelectableFlags_AllowDoubleClick = 1 << 2,
	ysbp_ui_ImGuiSelectableFlags_Disabled = 1 << 3,
	ysbp_ui_ImGuiSelectableFlags_AllowOverlap = 1 << 4,
	ysbp_ui_ImGuiSelectableFlags_Highlight = 1 << 5,
	ysbp_ui_ImGuiSelectableFlags_SelectOnNav = 1 << 6,
};

struct ysbp_ui_Vec2 {
	float x, y;
};

YS_CORE_EXPORT void ysbp_ui_begin_group();
YS_CORE_EXPORT void ysbp_ui_end_group();

YS_CORE_EXPORT void ysbp_ui_same_line();
YS_CORE_EXPORT ysbp_ui_Vec2 ysbp_ui_get_item_rect_size();

YS_CORE_EXPORT void ysbp_ui_align_cursor_for_center(float width);

YS_CORE_EXPORT void ysbp_ui_text(const char *string);

YS_CORE_EXPORT void ysbp_ui_checkbox(const char *label, bool *value);
YS_CORE_EXPORT bool ysbp_ui_knob(const char *label, float *p_value, float v_min, float v_max, float speed = 0,
								 const char *format = "%.3f",
								 ysbp_ui_ImGuiKnobVariant variant = ysbp_ui_ImGuiKnobVariant_Tick, float size = 0,
								 ysbp_ui_ImGuiKnobFlags flags = 0, int steps = 10, float angle_min = -1,
								 float angle_max = -1);
YS_CORE_EXPORT bool ysbp_ui_begin_combo(const char *label, const char *preview_value,
										ysbp_ui_ImGuiComboFlags flags = 0);
YS_CORE_EXPORT bool ysbp_ui_combo_selectable(const char *label, bool selected, ysbp_ui_ImGuiSelectableFlags flags = 0);
YS_CORE_EXPORT void ysbp_ui_combo_set_default_item_focus();
YS_CORE_EXPORT void ysbp_ui_end_combo();

namespace YourSound::UI {
	YS_CORE_EXPORT_NO_EXTERN void set_imgui_context(void *im_context);
}