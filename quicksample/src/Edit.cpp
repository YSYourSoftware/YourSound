#include "Edit.hpp"

#include "Audio.hpp"
#include "MessageBox.hpp"
#include "Renderer.hpp"
#include "SampleLZ4Serialiser.hpp"

#include <YourSound/BinPlayerSTD.hpp>
#include <YourSound/Serialisation.hpp>

#include <ImGuiPianoKeyboard.hpp>
#include <imgui.h>

#include <array>
#include <filesystem>
#include <fstream>
#include <vector>

std::filesystem::path project_file;
bool work_unsaved = false;

typedef struct {
	std::vector<float> samples_pcm;
	std::string name;

	bool is_loaded;
} sample_t;

auto keys_pressed = new bool[128]{};
std::vector<sample_t> samples{{.name = "hi", .is_loaded = true}};
size_t recording_sample = -1;

static bool piano_callback(void *, const int msg, const int note, const float velocity) {
	if (note >= 128 || note < 0) return false;
	if (msg == NoteGetStatus) return keys_pressed[note];
	if (msg == NoteOn) { keys_pressed[note] = true; }
	if (msg == NoteOff) { keys_pressed[note] = false; }
	return false;
}

static void save() {
	if (project_file.empty()) {
		constexpr SDL_DialogFileFilter filters[]{{"QuickSample Project", "qspr"}, {"All Files", "*"}};

		SDL_ShowSaveFileDialog(
			[](void *path, const char *const files[], int) {
				auto *f_path = static_cast<std::filesystem::path *>(path);

				*f_path = {files[0]};
				if (!f_path->has_extension()) f_path->replace_extension(".qspr");

				save();
			},
			&project_file, g_window, filters, 2, nullptr);

		return;
	}

	if (!ask_yesno_warning("Some files may be overwritten",
						   "Every sample is saved as a YSRC file alongside the project file, if there is a file with "
						   "the same name, it'll be overwritten.\n\nAre you sure you'd like to continue?"))
		return;

	std::ofstream project{project_file, std::ios::binary};
	project.write("QSPr\x01", 5);

	std::array<uint8_t, 8> tempvar_buf;

	project.write(reinterpret_cast<const char *>(&g_channels), 1);

	YourSound::write_integral_be(tempvar_buf.data(), samples.size());
	project.write(reinterpret_cast<const char *>(tempvar_buf.data()), 8);

	for (const auto &[samples_pcm, name, is_loaded] : samples) {
		project.write(name.c_str(), name.size() + 1);
		if (!is_loaded) continue;
		write_sample(samples_pcm, project_file.parent_path() / std::format("{}.ysrc", name));
	}
}

static bool save_unsaved_work_mb() {
	const SaveWorkResponse response =
		warn_unsaved_work("Unsaved Changes", "There are unsaved changes in \"{}\".\nWant to save them?");
	if (response == SaveWorkResponse::SAVE) save();
	return response != SaveWorkResponse::CANCEL;
}

void edit_window() {
	ImGui::SetNextWindowPos({0.f, 80.f});
	ImGui::SetNextWindowSize({1280.f, 720.f});
	ImGui::PushStyleColor(ImGuiCol_WindowBg, {0.05f, 0.05f, 0.05f, 1.f});
	ImGui::Begin("Edit", nullptr,
				 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
					 ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_MenuBar);
	ImGui::PopStyleColor();

	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Save...", "Ctrl + S") || ImGui::IsKeyChordPressed(ImGuiKey_S | ImGuiMod_Ctrl)) save();

			ImGui::MenuItem("Open...", "Ctrl + O");

			if (ImGui::MenuItem("New", "Ctrl + N") || ImGui::IsKeyChordPressed(ImGuiKey_N | ImGuiMod_Ctrl)) {
				do {
					if (work_unsaved)
						if (!save_unsaved_work_mb()) break;

					samples.clear();
					project_file.clear();
				} while (false);
			}

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	ImGui::Text("%llu Samples", samples.size());

	ImGui::PlotLines("##left-rec", samples[recording_sample].samples_pcm.data(),
					 samples[recording_sample].samples_pcm.size() / g_channels, 0, nullptr, -1.f, 1.f, {1000, 200},
					 4 * g_channels);

	static int previous_note_active = -1;
	ImGui::SetCursorPos({0.f, 720.f - 125.f});
	ImGui_PianoKeyboard("##keyboard", {1280.f, 125.f}, &previous_note_active, 21, 108, piano_callback, nullptr);

	ImGui::End();
}

void new_samples_in(const float *samples_pcm, const size_t count) {
	if (recording_sample == static_cast<size_t>(-1)) return;

	YS_RUNTIME_ASSERT(samples[recording_sample].is_loaded, std::runtime_error, "Target recording sample not loaded");

	std::vector<float> &stored_samples = samples[recording_sample].samples_pcm;
	const size_t index = stored_samples.size();
	stored_samples.resize(stored_samples.size() + count);

	std::memcpy(stored_samples.data() + index, samples_pcm, count * 4);
}
