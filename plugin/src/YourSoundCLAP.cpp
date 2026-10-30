#include "YourSoundCLAP.hpp"

#include <YourSound/Player.hpp>
#include <imgui.h>

YourSoundCLAP::~YourSoundCLAP() {
	for (auto &player : m_rack_slots) {
		if (player == nullptr) continue;

		delete player;

		player = nullptr;
	}
}

bool YourSoundCLAP::init() noexcept { return true; }

bool YourSoundCLAP::guiIsApiSupported(const char *api, bool is_floating) noexcept {
#ifdef _WIN32
	return std::strcmp(api, CLAP_WINDOW_API_WIN32) == 0;
#elifdef __APPLE__
	return std::strcmp(api, CLAP_WINDOW_API_COCOA) == 0;
#else
	return std::strcmp(api, CLAP_WINDOW_API_X11) == 0 || std::strcmp(api, CLAP_WINDOW_API_WAYLAND) == 0;
#endif

	return false;
}

bool YourSoundCLAP::guiGetPreferredApi(const char **api, bool *is_floating) noexcept {
	*is_floating = false;

#ifdef _WIN32
	*api = CLAP_WINDOW_API_WIN32;
#elifdef __APPLE__
	*api = CLAP_WINDOW_API_COCOA;
#else
	*is_floating = true;
	*api = CLAP_WINDOW_API_WAYLAND;
#endif

	return true;
}

void YourSoundCLAP::p_imgui_create_ui() {
	ImGui::Begin("YourSound", nullptr,
				 ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);

	if (ImGui::BeginMainMenuBar()) {
		ImGui::EndMainMenuBar();
	}
}
