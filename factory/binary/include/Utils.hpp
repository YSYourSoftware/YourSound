#pragma once

#include <cmath>

#include <imgui.h>

inline void imgui_align_cursor_for_center(const float width) {
	ImGui::SetCursorPosX(std::ceilf(ImGui::GetCursorStartPos().x + (ImGui::GetContentRegionAvail().x - width) * 0.5f));
}