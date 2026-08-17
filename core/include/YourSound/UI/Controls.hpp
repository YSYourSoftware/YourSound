#pragma once

#include "YourSound/CoreExportImport.hpp"

#include <cstdint>

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

typedef int ysbp_ui_ImGuiSliderFlags;

enum ysbp_ui_ImGuiSliderFlags_ {
	ysbp_ui_ImGuiSliderFlags_None = 0,
	ysbp_ui_ImGuiSliderFlags_Logarithmic = 1 << 5,
	ysbp_ui_ImGuiSliderFlags_NoRoundToFormat = 1 << 6,
	ysbp_ui_ImGuiSliderFlags_NoInput = 1 << 7,
	ysbp_ui_ImGuiSliderFlags_WrapAround = 1 << 8,
	ysbp_ui_ImGuiSliderFlags_ClampOnInput = 1 << 9,
	ysbp_ui_ImGuiSliderFlags_ClampZeroRange = 1 << 10,
	ysbp_ui_ImGuiSliderFlags_NoSpeedTweaks = 1 << 11,
	ysbp_ui_ImGuiSliderFlags_ColorMarkers = 1 << 12,
	ysbp_ui_ImGuiSliderFlags_AlwaysClamp =
		ysbp_ui_ImGuiSliderFlags_ClampOnInput | ysbp_ui_ImGuiSliderFlags_ClampZeroRange,
	ysbp_ui_ImGuiSliderFlags_InvalidMask_ = 0x7000000F,
};

typedef int ysbp_ui_ImGuiChildFlags;

enum ysbp_ui_ImGuiChildFlags_ {
	ysbp_ui_ImGuiChildFlags_None = 0,
	ysbp_ui_ImGuiChildFlags_Borders = 1 << 0,
	ysbp_ui_ImGuiChildFlags_AlwaysUseWindowPadding = 1 << 1,
	ysbp_ui_ImGuiChildFlags_ResizeX = 1 << 2,
	ysbp_ui_ImGuiChildFlags_ResizeY = 1 << 3,
	ysbp_ui_ImGuiChildFlags_AutoResizeX = 1 << 4,
	ysbp_ui_ImGuiChildFlags_AutoResizeY = 1 << 5,
	ysbp_ui_ImGuiChildFlags_AlwaysAutoResize = 1 << 6,
	ysbp_ui_ImGuiChildFlags_FrameStyle = 1 << 7,
	ysbp_ui_ImGuiChildFlags_NavFlattened = 1 << 8,
};

typedef int ysbp_ui_ImGuiWindowFlags;

enum ysbp_ui_ImGuiWindowFlags_ {
	ysbp_ui_ImGuiWindowFlags_None = 0,
	ysbp_ui_ImGuiWindowFlags_NoTitleBar = 1 << 0,
	ysbp_ui_ImGuiWindowFlags_NoResize = 1 << 1,
	ysbp_ui_ImGuiWindowFlags_NoMove = 1 << 2,
	ysbp_ui_ImGuiWindowFlags_NoScrollbar = 1 << 3,
	ysbp_ui_ImGuiWindowFlags_NoScrollWithMouse = 1 << 4,
	ysbp_ui_ImGuiWindowFlags_NoCollapse = 1 << 5,
	ysbp_ui_ImGuiWindowFlags_AlwaysAutoResize = 1 << 6,
	ysbp_ui_ImGuiWindowFlags_NoBackground = 1 << 7,
	ysbp_ui_ImGuiWindowFlags_NoSavedSettings = 1 << 8,
	ysbp_ui_ImGuiWindowFlags_NoMouseInputs = 1 << 9,
	ysbp_ui_ImGuiWindowFlags_MenuBar = 1 << 10,
	ysbp_ui_ImGuiWindowFlags_HorizontalScrollbar = 1 << 11,
	ysbp_ui_ImGuiWindowFlags_NoFocusOnAppearing = 1 << 12,
	ysbp_ui_ImGuiWindowFlags_NoBringToFrontOnFocus = 1 << 13,
	ysbp_ui_ImGuiWindowFlags_AlwaysVerticalScrollbar = 1 << 14,
	ysbp_ui_ImGuiWindowFlags_AlwaysHorizontalScrollbar = 1 << 15,
	ysbp_ui_ImGuiWindowFlags_NoNavInputs = 1 << 16,
	ysbp_ui_ImGuiWindowFlags_NoNavFocus = 1 << 17,
	ysbp_ui_ImGuiWindowFlags_UnsavedDocument = 1 << 18,
	ysbp_ui_ImGuiWindowFlags_NoDocking = 1 << 19,
	ysbp_ui_ImGuiWindowFlags_NoNav = ysbp_ui_ImGuiWindowFlags_NoNavInputs | ysbp_ui_ImGuiWindowFlags_NoNavFocus,
	ysbp_ui_ImGuiWindowFlags_NoDecoration = ysbp_ui_ImGuiWindowFlags_NoTitleBar | ysbp_ui_ImGuiWindowFlags_NoResize |
											ysbp_ui_ImGuiWindowFlags_NoScrollbar | ysbp_ui_ImGuiWindowFlags_NoCollapse,
	ysbp_ui_ImGuiWindowFlags_NoInputs = ysbp_ui_ImGuiWindowFlags_NoMouseInputs | ysbp_ui_ImGuiWindowFlags_NoNavInputs |
										ysbp_ui_ImGuiWindowFlags_NoNavFocus,
	ysbp_ui_ImGuiWindowFlags_DockNodeHost = 1 << 23,
	ysbp_ui_ImGuiWindowFlags_ChildWindow = 1 << 24,
	ysbp_ui_ImGuiWindowFlags_Tooltip = 1 << 25,
	ysbp_ui_ImGuiWindowFlags_Popup = 1 << 26,
	ysbp_ui_ImGuiWindowFlags_Modal = 1 << 27,
	ysbp_ui_ImGuiWindowFlags_ChildMenu = 1 << 28,
};

typedef int ysbp_ui_ImGuiCol;

enum ysbp_ui_ImGuiCol_ {
	ysbp_ui_ImGuiCol_Text,
	ysbp_ui_ImGuiCol_TextDisabled,
	ysbp_ui_ImGuiCol_WindowBg,
	ysbp_ui_ImGuiCol_ChildBg,
	ysbp_ui_ImGuiCol_PopupBg,
	ysbp_ui_ImGuiCol_Border,
	ysbp_ui_ImGuiCol_BorderShadow,
	ysbp_ui_ImGuiCol_FrameBg,
	ysbp_ui_ImGuiCol_FrameBgHovered,
	ysbp_ui_ImGuiCol_FrameBgActive,
	ysbp_ui_ImGuiCol_TitleBg,
	ysbp_ui_ImGuiCol_TitleBgActive,
	ysbp_ui_ImGuiCol_TitleBgCollapsed,
	ysbp_ui_ImGuiCol_MenuBarBg,
	ysbp_ui_ImGuiCol_ScrollbarBg,
	ysbp_ui_ImGuiCol_ScrollbarGrab,
	ysbp_ui_ImGuiCol_ScrollbarGrabHovered,
	ysbp_ui_ImGuiCol_ScrollbarGrabActive,
	ysbp_ui_ImGuiCol_CheckMark,
	ysbp_ui_ImGuiCol_CheckboxSelectedBg,
	ysbp_ui_ImGuiCol_SliderGrab,
	ysbp_ui_ImGuiCol_SliderGrabActive,
	ysbp_ui_ImGuiCol_Button,
	ysbp_ui_ImGuiCol_ButtonHovered,
	ysbp_ui_ImGuiCol_ButtonActive,
	ysbp_ui_ImGuiCol_Header,
	ysbp_ui_ImGuiCol_HeaderHovered,
	ysbp_ui_ImGuiCol_HeaderActive,
	ysbp_ui_ImGuiCol_Separator,
	ysbp_ui_ImGuiCol_SeparatorHovered,
	ysbp_ui_ImGuiCol_SeparatorActive,
	ysbp_ui_ImGuiCol_ResizeGrip,
	ysbp_ui_ImGuiCol_ResizeGripHovered,
	ysbp_ui_ImGuiCol_ResizeGripActive,
	ysbp_ui_ImGuiCol_InputTextCursor,
	ysbp_ui_ImGuiCol_TabHovered,
	ysbp_ui_ImGuiCol_Tab,
	ysbp_ui_ImGuiCol_TabSelected,
	ysbp_ui_ImGuiCol_TabSelectedOverline,
	ysbp_ui_ImGuiCol_TabDimmed,
	ysbp_ui_ImGuiCol_TabDimmedSelected,
	ysbp_ui_ImGuiCol_TabDimmedSelectedOverline,
	ysbp_ui_ImGuiCol_DockingPreview,
	ysbp_ui_ImGuiCol_DockingEmptyBg,
	ysbp_ui_ImGuiCol_PlotLines,
	ysbp_ui_ImGuiCol_PlotLinesHovered,
	ysbp_ui_ImGuiCol_PlotHistogram,
	ysbp_ui_ImGuiCol_PlotHistogramHovered,
	ysbp_ui_ImGuiCol_TableHeaderBg,
	ysbp_ui_ImGuiCol_TableBorderStrong,
	ysbp_ui_ImGuiCol_TableBorderLight,
	ysbp_ui_ImGuiCol_TableRowBg,
	ysbp_ui_ImGuiCol_TableRowBgAlt,
	ysbp_ui_ImGuiCol_TextLink,
	ysbp_ui_ImGuiCol_TextSelectedBg,
	ysbp_ui_ImGuiCol_TreeLines,
	ysbp_ui_ImGuiCol_DragDropTarget,
	ysbp_ui_ImGuiCol_DragDropTargetBg,
	ysbp_ui_ImGuiCol_UnsavedMarker,
	ysbp_ui_ImGuiCol_NavCursor,
	ysbp_ui_ImGuiCol_NavWindowingHighlight,
	ysbp_ui_ImGuiCol_NavWindowingDimBg,
	ysbp_ui_ImGuiCol_ModalWindowDimBg,
	ysbp_ui_ImGuiCol_COUNT
};

typedef int ysbp_ui_ImGuiStyleVar;

enum ysbp_ui_ImGuiStyleVar_ {
	ysbp_ui_ImGuiStyleVar_Alpha,
	ysbp_ui_ImGuiStyleVar_DisabledAlpha,
	ysbp_ui_ImGuiStyleVar_WindowPadding,
	ysbp_ui_ImGuiStyleVar_WindowRounding,
	ysbp_ui_ImGuiStyleVar_WindowBorderSize,
	ysbp_ui_ImGuiStyleVar_WindowMinSize,
	ysbp_ui_ImGuiStyleVar_WindowTitleAlign,
	ysbp_ui_ImGuiStyleVar_ChildRounding,
	ysbp_ui_ImGuiStyleVar_ChildBorderSize,
	ysbp_ui_ImGuiStyleVar_PopupRounding,
	ysbp_ui_ImGuiStyleVar_PopupBorderSize,
	ysbp_ui_ImGuiStyleVar_FramePadding,
	ysbp_ui_ImGuiStyleVar_FrameRounding,
	ysbp_ui_ImGuiStyleVar_FrameBorderSize,
	ysbp_ui_ImGuiStyleVar_ItemSpacing,
	ysbp_ui_ImGuiStyleVar_ItemInnerSpacing,
	ysbp_ui_ImGuiStyleVar_IndentSpacing,
	ysbp_ui_ImGuiStyleVar_CellPadding,
	ysbp_ui_ImGuiStyleVar_ScrollbarSize,
	ysbp_ui_ImGuiStyleVar_ScrollbarRounding,
	ysbp_ui_ImGuiStyleVar_ScrollbarPadding,
	ysbp_ui_ImGuiStyleVar_GrabMinSize,
	ysbp_ui_ImGuiStyleVar_GrabRounding,
	ysbp_ui_ImGuiStyleVar_ImageRounding,
	ysbp_ui_ImGuiStyleVar_ImageBorderSize,
	ysbp_ui_ImGuiStyleVar_TabRounding,
	ysbp_ui_ImGuiStyleVar_TabBorderSize,
	ysbp_ui_ImGuiStyleVar_TabMinWidthBase,
	ysbp_ui_ImGuiStyleVar_TabMinWidthShrink,
	ysbp_ui_ImGuiStyleVar_TabBarBorderSize,
	ysbp_ui_ImGuiStyleVar_TabBarOverlineSize,
	ysbp_ui_ImGuiStyleVar_TableAngledHeadersAngle,
	ysbp_ui_ImGuiStyleVar_TableAngledHeadersTextAlign,
	ysbp_ui_ImGuiStyleVar_TreeLinesSize,
	ysbp_ui_ImGuiStyleVar_TreeLinesRounding,
	ysbp_ui_ImGuiStyleVar_DragDropTargetRounding,
	ysbp_ui_ImGuiStyleVar_ButtonTextAlign,
	ysbp_ui_ImGuiStyleVar_SelectableTextAlign,
	ysbp_ui_ImGuiStyleVar_SeparatorSize,
	ysbp_ui_ImGuiStyleVar_SeparatorTextBorderSize,
	ysbp_ui_ImGuiStyleVar_SeparatorTextAlign,
	ysbp_ui_ImGuiStyleVar_SeparatorTextPadding,
	ysbp_ui_ImGuiStyleVar_DockingSeparatorSize,
	ysbp_ui_ImGuiStyleVar_COUNT
};

enum ysbp_ui_Font : uint8_t {
	ysbp_ui_FONT_STANDARD_LARGE,
	ysbp_ui_FONT_ICON,
	ysbp_ui_FONT_ICON_LARGE,
	ysbp_ui_FONT_MONO,
	ysbp_ui_FONT_MONO_BOLD,
	ysbp_ui_FONT_STANDARD_MEDIUM
};

typedef struct ysbp_ui_Vec2 {
	float x, y;
} ysbp_ui_Vec2;

typedef struct ysbp_ui_Vec4 {
	float x, y, z, w;
} ysbp_ui_Vec4;

YS_BPI void ysbp_ui_begin_group();
YS_BPI void ysbp_ui_end_group();

YS_BPI void ysbp_ui_begin_child(const char *str_id, ysbp_ui_Vec2 size = {0, 0},
										ysbp_ui_ImGuiChildFlags child_flags = 0,
										ysbp_ui_ImGuiWindowFlags window_flags = 0);
YS_BPI void ysbp_ui_end_child();

YS_BPI void ysbp_ui_same_line();
YS_BPI ysbp_ui_Vec2 ysbp_ui_get_item_rect_size();
YS_BPI ysbp_ui_Vec2 ysbp_ui_get_content_region_avail();

YS_BPI void ysbp_ui_align_cursor_for_center(float width);
YS_BPI bool ysbp_ui_is_hovering();

YS_BPI void ysbp_ui_text(const char *string);
YS_BPI bool ysbp_ui_button(const char *label, ysbp_ui_Vec2 size = {0, 0});

YS_BPI bool ysbp_ui_checkbox(const char *label, bool *value);
YS_BPI bool ysbp_ui_knob(const char *label, float *p_value, float v_min, float v_max, float speed = 0,
								 const char *format = "%.3f",
								 ysbp_ui_ImGuiKnobVariant variant = ysbp_ui_ImGuiKnobVariant_Tick, float size = 0,
								 ysbp_ui_ImGuiKnobFlags flags = 0, int steps = 10, float angle_min = -1,
								 float angle_max = -1);
YS_BPI bool ysbp_ui_float_slider(const char *label, float *v, float v_min, float v_max,
										 const char *format = "%.3f", ysbp_ui_ImGuiSliderFlags flags = 0);
YS_BPI bool ysbp_ui_float_slider_vertical(const char *label, ysbp_ui_Vec2 size, float *v, float v_min,
												  float v_max, const char *format = "%.3f",
												  ysbp_ui_ImGuiSliderFlags flags = 0);
YS_BPI bool ysbp_ui_float_drag(const char *label, float *v, float v_speed, float v_min, float v_max,
									   const char *format = "%.3f", ysbp_ui_ImGuiSliderFlags flags = 0);
YS_BPI bool ysbp_ui_int_slider(const char *label, int *v, int v_min, int v_max, const char *format = "%d",
									   ysbp_ui_ImGuiSliderFlags flags = 0);
YS_BPI bool ysbp_ui_int_drag(const char *label, int *v, int v_speed, int v_min, int v_max,
									 const char *format = "%d", ysbp_ui_ImGuiSliderFlags flags = 0);

YS_BPI bool ysbp_ui_begin_combo(const char *label, const char *preview_value,
										ysbp_ui_ImGuiComboFlags flags = 0);
YS_BPI bool ysbp_ui_combo_selectable(const char *label, bool selected, ysbp_ui_ImGuiSelectableFlags flags = 0);
YS_BPI void ysbp_ui_combo_set_default_item_focus();
YS_BPI void ysbp_ui_end_combo();
YS_BPI bool ysbp_ui_begin_tooltip();
YS_BPI void ysbp_ui_end_tooltip();

YS_BPI void ysbp_ui_push_style_colour(ysbp_ui_ImGuiCol idx, ysbp_ui_Vec4 col);
YS_BPI void ysbp_ui_pop_style_colour(int count = 1);
YS_BPI void ysbp_ui_push_style_var(ysbp_ui_ImGuiStyleVar idx, ysbp_ui_Vec2 val);
YS_BPI void ysbp_ui_pop_style_var(int count = 1);
YS_BPI void ysbp_ui_push_font(ysbp_ui_Font font);
YS_BPI void ysbp_ui_pop_font();

YS_BPI void ysbp_ui_line_graph_data(const char *label, const float *values, int values_count, int values_offset,
											const char *overlay_text, float scale_min, float scale_max,
											ysbp_ui_Vec2 graph_size, int stride);

YS_BPI void ysbp_ui_line_graph(const char *label, float (*values_getter)(void *data, int idx), void *data,
									   int values_count, int values_offset, const char *overlay_text, float scale_min,
									   float scale_max, ysbp_ui_Vec2 graph_size);

YS_BPI void ysbp_ui_set_next_item_width(float width);

YS_BPI ysbp_ui_Vec2 ysbp_ui_get_item_spacing();
YS_BPI float ysbp_ui_get_frame_height();

YS_BPI void ysbp_ui_spacing();

namespace YourSound::UI {
	YS_API void set_imgui_context(void *im_context);
}
