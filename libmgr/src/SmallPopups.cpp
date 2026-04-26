#include "SmallPopups.hpp"
#include "MessageBox.hpp"

#include <imgui.h>

#include <YourSound/Config.hpp>
#include <YourSound/Version.hpp>

#include <exception>

#include "Globals.hpp"

void window_about() {
	ImGui::Begin("About YourSound Library Manager", &display_about, ImGuiWindowFlags_NoResize);

	ImGui::Text("YourSound Library Manager v1.0.0");
	ImGui::Text("For use with YourSound v%s", YS_VERSION_STRING);
	try {
		ImGui::Text("Library Location: %s", YourSound::get_library_location().string().c_str());
	} catch (std::exception &exception) {
		display_about = false;
		show_error(exception.what());
	}

	ImGui::End();
}
