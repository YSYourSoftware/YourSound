#include <YourSound/Version.hpp>
#include <clap/clap.h>
#include <clap/helpers/plugin.hxx>

class YourSoundCLAP : public clap::helpers::Plugin<clap::helpers::MisbehaviourHandler::Terminate, clap::helpers::CheckingLevel::Maximal> {
public:
	static const void *get_extension(const clap_plugin_t *plugin, const char *id) {
		CLAP_EXT_PARAMS
	}
private:
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
