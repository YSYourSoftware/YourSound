#include "LibraryInstall.hpp"
#include "Globals.hpp"
#include "MessageBox.hpp"

#include <imgui.h>
#include <SDL3/SDL.h>

#define LIBMGR_RUNTIME_ASSERT(condition, exception, message) if (!(condition)) throw exception(message)
#define LIBMGR_CHECK_JSON_OBJECT_RTAS(key, validator_func, type_name) LIBMGR_RUNTIME_ASSERT(target_library_info.contains(key) && target_library_info[key].validator_func(), std::invalid_argument, "Key '" key "' should be " type_name)

void binary_player_installation() {
	if (const MBTrustPublisherResult inst_binary = ask_yesno_trust_publisher("Install binary players?", "Binary players have access to everything on your computer. Installing binary players from an untrusted source may put your computer at risk." ,"YourSound"); inst_binary == NO) {
		if (!ask_yesno_warning("Are you sure you don't want to install binary players?", "Binary players are critical to the function of some libraries. If you choose not to install them, the library may be unusable.")) return;
	} else if (inst_binary == YES_AND_TRUST) {

	}
}

void complete_library_installation() {
	binary_player_installation();
}

void window_install_confirm() {
	ImGui::Begin("Install Library", &display_install);

	ImGui::Image(target_library_cover_64, ImVec2(64, 64));

	ImGui::SameLine();

	ImGui::BeginGroup();
	ImGui::PushFont(font_heading);
	ImGui::Text("%s", target_library_info["name"].get<std::string>().c_str());
	ImGui::PopFont();

	ImGui::Text("By %s", target_library_info["author"].get<std::string>().c_str());
	ImGui::SameLine();
	ImGui::PushFont(font_light);
	ImGui::Text("v%i.%i.%i", target_library_info["version"][0].get<uint16_t>(), target_library_info["version"][1].get<uint16_t>(), target_library_info["version"][2].get<uint16_t>());
	ImGui::PopFont();
	ImGui::EndGroup();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::Text("%s", target_library_info["description"].get<std::string>().c_str());

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	const bool has_signature = false;
	const bool correct_signature = false;

	if (has_signature && correct_signature) ImGui::TextColored(COLOUR_GREEN, "This library has a valid signature.");
	if (has_signature && !correct_signature) ImGui::TextColored(COLOUR_RED, "This library has an incorrect signature.");
	if (!has_signature) ImGui::TextColored(COLOUR_ORANGE, "This library no signature.");

	if (has_signature) {
		if (correct_signature) {
			ImGui::PushStyleColor(ImGuiCol_Header, colour_scale(COLOUR_GREEN, 0.5f));
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, colour_scale(COLOUR_GREEN, 0.75f));
			ImGui::PushStyleColor(ImGuiCol_HeaderActive, colour_scale(COLOUR_GREEN, 0.5f));
		} else {
			ImGui::PushStyleColor(ImGuiCol_Header, colour_scale(COLOUR_RED, 0.5f));
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, colour_scale(COLOUR_RED, 0.75f));
			ImGui::PushStyleColor(ImGuiCol_HeaderActive, colour_scale(COLOUR_RED, 0.5f));
		}

		if (ImGui::CollapsingHeader("Signature Info")) {
			ImGui::Text("Signed: %s\nMethod: %s", "Samuel Plimmer", "GPG (no CA)");
			ImGui::TextLinkOpenURL("About archive signing", "https://YourSoftware.org/projects/YourSound/help/sarc-signing");
		}

		ImGui::PopStyleColor(3);
		ImGui::Spacing();
	}

	if (ImGui::CollapsingHeader("Extra info")) {
		ImGui::Text("Library ID: %s\nSize required: 0KB\nEstimated install time: 00:00",
			target_library_info["id"].get<std::string>().c_str()
		);
	}

	ImGui::Spacing();

	if (ImGui::Button("Install")) {
		if (has_signature && !correct_signature) {
			if (!ask_yesno_warning("Are you sure you want to install this library?", "This library has an incorrect signature, which means it has been modified, and may contain malware.")) {
				ImGui::End();
				return;
			}
		}
		display_install = false;
		complete_library_installation();
	};

	ImGui::End();
};

void throw_if_invalid_library_info() {
	LIBMGR_CHECK_JSON_OBJECT_RTAS("name", is_string, "a string");
	LIBMGR_CHECK_JSON_OBJECT_RTAS("version", is_array, "an array of 3 integers");
	LIBMGR_CHECK_JSON_OBJECT_RTAS("author", is_string, "a string");
	LIBMGR_CHECK_JSON_OBJECT_RTAS("id", is_string, "a string");
	LIBMGR_CHECK_JSON_OBJECT_RTAS("description", is_string, "a string");
}