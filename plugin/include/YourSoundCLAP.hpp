#pragma once

#include <YourSound/Player.hpp>
#include <YourSound/Version.hpp>
#include <clap/clap.h>
#include <clap/helpers/plugin.hh>

#include <array>

class YourSoundCLAP final : public clap::helpers::Plugin<clap::helpers::MisbehaviourHandler::Terminate,
														 clap::helpers::CheckingLevel::Maximal> {
public:
	YourSoundCLAP();
	~YourSoundCLAP() override;
protected:
	bool init() noexcept override;

	bool activate(double sample_rate, uint32_t min_frame_count, uint32_t max_frame_count) noexcept override;
	void deactivate() noexcept override;

	bool startProcessing() noexcept override;
	void stopProcessing() noexcept override;

	void reset() noexcept override;

	void onMainThread() noexcept override;

	const void *extension(const char *id) noexcept override;

	[[nodiscard]] bool implementsTail() const noexcept override { return true; }
	[[nodiscard]] uint32_t tailGet() const noexcept override;

	[[nodiscard]] bool implementsRender() const noexcept override { return true; }
	[[nodiscard]] bool renderSetMode(clap_plugin_render_mode mode) noexcept override;

	[[nodiscard]] bool implementsState() const noexcept override { return true; }
	[[nodiscard]] bool stateSave(const clap_ostream *stream) noexcept override;
	[[nodiscard]] bool stateLoad(const clap_istream *stream) noexcept override;

	[[nodiscard]] bool implementsPresetLoad() const noexcept override { return true; }
	[[nodiscard]] bool presetLoadFromLocation(uint32_t location_kind, const char *location, const char *load_key) noexcept override;

	[[nodiscard]] bool implementsTrackInfo() const noexcept override { return true; }
	void trackInfoChanged() noexcept override;

	[[nodiscard]] bool implementsAudioPorts() const noexcept override { return true; }
	[[nodiscard]] uint32_t audioPortsCount(bool is_input) const noexcept override { return 2; }
	[[nodiscard]] bool audioPortsInfo(uint32_t index, bool is_input, clap_audio_port_info *info) const noexcept override;

	[[nodiscard]] bool implementsParams() const noexcept override { return true; }
	[[nodiscard]] uint32_t paramsCount() const noexcept override;
	[[nodiscard]] bool paramsInfo(uint32_t param_index, clap_param_info *info) const noexcept override;
	[[nodiscard]] bool paramsValue(clap_id param_id, double *value) noexcept override;
	[[nodiscard]] bool paramsValueToText(clap_id param_id, double value, char *display, uint32_t size) noexcept override;
	[[nodiscard]] bool paramsTextToValue(clap_id param_id, const char *display, double *value) noexcept override;
	void paramsFlush(const clap_input_events *in, const clap_output_events *out) noexcept override;

	[[nodiscard]] int32_t getParamIndexForParamId(clap_id param_id) const noexcept override;
	[[nodiscard]] bool isValidParamId(clap_id param_id) const noexcept override;
	[[nodiscard]] bool getParamInfoForParamId(clap_id param_id, clap_param_info *info) const noexcept override;

	[[nodiscard]] bool implementsNotePorts() const noexcept override { return true; }
	[[nodiscard]] uint32_t notePortsCount(bool is_input) const noexcept override;
	[[nodiscard]] bool notePortsInfo(uint32_t index, bool is_input, clap_note_port_info *info) const noexcept override;

	[[nodiscard]] bool implementsGui() const noexcept override { return true; }
	[[nodiscard]] bool guiIsApiSupported(const char *api, bool is_floating) noexcept override;
	[[nodiscard]] bool guiGetPreferredApi(const char **api, bool *is_floating) noexcept override;
	[[nodiscard]] bool guiCreate(const char *api, bool is_floating) noexcept override;
	void guiDestroy() noexcept override;
	[[nodiscard]] bool guiSetScale(double scale) noexcept override;
	[[nodiscard]] bool guiShow() noexcept override;
	[[nodiscard]] bool guiHide() noexcept override;
	[[nodiscard]] bool guiGetSize(uint32_t *width, uint32_t *height) noexcept override;
	[[nodiscard]] bool guiAdjustSize(uint32_t *width, uint32_t *height) noexcept override;
	[[nodiscard]] bool guiSetSize(uint32_t width, uint32_t height) noexcept override;
	void guiSuggestTitle(const char *title) noexcept override;
	[[nodiscard]] bool guiSetParent(const clap_window *window) noexcept override;
	[[nodiscard]] bool guiSetTransient(const clap_window *window) noexcept override;

	[[nodiscard]] bool implementsVoiceInfo() const noexcept override { return true; }
	[[nodiscard]] bool voiceInfoGet(clap_voice_info *info) noexcept override;
private:
	void p_imgui_create_ui();

	std::array<YourSound::Player*, 16> m_rack_slots{nullptr};

	const clap_plugin_descriptor_t m_plugin_descriptor = {
		.clap_version = CLAP_VERSION,
		.id = "org.yoursoftware.sound",
		.name = "YourSound CLAP",
		.vendor = "YourSoftware Foundation",
		.url = "https://sound.yoursoftware.org/install",
		.manual_url = "https://files.yoursoftware.org/0/ysnd/usr/manual.pdf",
		.support_url = "",
		.version = YS_VERSION_STRING,
		.description = "Implementation of the YourSound engine as a CLAP plugin.",
		.features = (const char *const[]){CLAP_PLUGIN_FEATURE_INSTRUMENT, CLAP_PLUGIN_FEATURE_STEREO, nullptr}};

	clap_plugin_t m_plugin;
	const clap_host_t *m_host;
};
