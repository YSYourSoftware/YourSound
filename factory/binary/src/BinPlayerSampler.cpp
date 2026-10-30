#include "BinPlayerSampler.hpp"

#include <YourSound/BinPlayerInterface.hpp>
#include <YourSound/BinPlayerSTD.hpp>

YS_START_BP_REGISTRY
	YS_REGISTER_BP("org.yoursoftware.sound.sampler", BinPlayerSampler)
YS_END_BP_REGISTRY

void BinPlayerSampler::note_on(const uint8_t midi_note_number, const float velocity) {
	sample_t *sample = p_get_sample_by_note_info(midi_note_number, velocity);
	if (!sample) {
		m_polyphony_controller.note_on(midi_note_number, velocity);
		return;
	}

	if (sample->pcm.empty() && !sample->loaded_from.empty())
		ysbp_load_resource(sample->loaded_from.generic_string().c_str(), [](const uint8_t *data, size_t size) {
			// TODO: load resource
		});

	m_polyphony_controller.note_on(midi_note_number, velocity);
}

void BinPlayerSampler::render(float *output_buffer, const uint16_t number_samples) {
	m_polyphony_controller.for_each_voice([this](const auto &voice) -> bool {
		if (voice->releasing && voice->time > m_amp_envelope.release_time) return false;

		return true;
	});
}

void BinPlayerSampler::set_parameter(const char *param_id, const float value) {
	if (std::strcmp(param_id, "_pitch_wheel") == 0) {
		m_polyphony_controller.pitch_bend_update(value * 2.f - 1.f);
		return;
	}

	if (std::strcmp(param_id, "_midi_cc_64") == 0) {
		if (value > 0.5f) m_polyphony_controller.sustain_on();
		else m_polyphony_controller.sustain_off();
		return;
	}
}

BinPlayerSampler::sample_t *BinPlayerSampler::p_get_sample_by_note_info(const uint8_t note, const float velocity) {
	for (auto &sample : m_samples)
		if (sample.note_start <= note && sample.note_end >= note && sample.velocity_start <= velocity &&
			sample.velocity_end >= velocity)
			return &sample;

	return nullptr;
}
