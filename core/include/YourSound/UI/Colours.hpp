#pragma once

#include <imgui.h>

namespace YourSound::UI::Colours {
	constexpr ImVec4 BUTTON_GREEN = {0.f, 102.f / 255.f, 10.f / 255.f, 1.f};
	constexpr ImVec4 BUTTON_GREEN_HOVER = {3.f / 255.f, 180.f / 255.f, 67.f / 255.f, 1.f};
	constexpr ImVec4 BUTTON_GREEN_ACTIVE = {0.f, 168.f / 255.f, 39.f / 255.f, 1.f};

	constexpr ImVec4 BUTTON_RED = {156.f / 255.f, 2.f / 255.f, 0.f, 1.f};
	constexpr ImVec4 BUTTON_RED_HOVER = {243.f / 255.f, 50.f / 255.f, 1.f / 255.f, 1.f};
	constexpr ImVec4 BUTTON_RED_ACTIVE = {256.f / 255.f, 5.f / 255.f, 0.f, 1.f};
}