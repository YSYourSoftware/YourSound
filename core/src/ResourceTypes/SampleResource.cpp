#include "YourSound/ResourceTypes/SampleResource.hpp"

#include "YourSound/BinPlayerSTD.hpp"
#include "YourSound/External/adpcm.h"
#include "YourSound/External/stb_vorbis.h"

#define DR_FLAC_IMPLEMENTATION
#include <dr_flac.h>
#define DR_MP3_IMPLEMENTATION
#include <dr_mp3.h>
#define DR_WAV_IMPLEMENTATION
#include <dr_wav.h>

using namespace YourSound;

void SampleResource::from_flac(const uint8_t *input_buffer, const size_t length) {
	drflac *flac = drflac_open_memory(input_buffer, length, nullptr);
	SARC_RUNTIME_ASSERT(flac, std::runtime_error, "Failed to load FLAC data, check the FLAC file is valid.");

	SARC_RUNTIME_ASSERT(flac->channels <= 2, std::runtime_error, "Too many channels in stream, max 2 supported.");

	auto *samples = new float[flac->totalPCMFrameCount * 2];
	const drflac_uint64 sample_count = drflac_read_pcm_frames_f32(flac, flac->totalPCMFrameCount, flac->channels == 2 ? samples : samples + flac->totalPCMFrameCount);

	SARC_RUNTIME_ASSERT(sample_count == flac->totalPCMFrameCount, std::runtime_error, "Samples read is not equal to total samples.");

	if (flac->channels == 1) BinPlayer::mono_to_stereo(samples + sample_count, samples, sample_count);

	m_data.reserve(sample_count * 2 * sizeof(float) * 2);
	std::memcpy(m_data.data(), samples, sample_count * 2 * sizeof(float));

	delete[] samples;
	drflac_free(flac, nullptr);
}

void SampleResource::from_mp3(const uint8_t *input_buffer, const size_t length) {
	drmp3 mp3;
	SARC_RUNTIME_ASSERT(drmp3_init_memory(&mp3, input_buffer, length, nullptr), std::runtime_error, "Failed to load MP3 data, check the MP3 file is valid.");

	SARC_RUNTIME_ASSERT(mp3.channels <= 2, std::runtime_error, "Too many channels in stream, max 2 supported.");

	auto *samples = new float[mp3.totalPCMFrameCount * 2];
	const drmp3_uint64 sample_count = drmp3_read_pcm_frames_f32(&mp3, mp3.totalPCMFrameCount, mp3.channels == 2 ? samples : samples + mp3.totalPCMFrameCount);

	SARC_RUNTIME_ASSERT(sample_count == mp3.totalPCMFrameCount, std::runtime_error, "Samples read is not equal to total samples.");

	if (mp3.channels == 1) BinPlayer::mono_to_stereo(samples + sample_count, samples, sample_count);

	m_data.reserve(sample_count * 2 * sizeof(float));
	std::memcpy(m_data.data(), samples, sample_count * 2 * sizeof(float));

	delete[] samples;
	drmp3_free(&mp3, nullptr);
}

void SampleResource::from_ogg(const uint8_t *input_buffer, const size_t length) {
	int32_t vorbis_error = 0;
	stb_vorbis *ogg = stb_vorbis_open_memory(input_buffer, length, &vorbis_error, nullptr);

	SARC_RUNTIME_ASSERT(vorbis_error == VORBIS__no_error, std::runtime_error, "Failed to load OGG Vorbis data, check the OGG file is valid.\n(error " + std::to_string(vorbis_error) + ")");

	const stb_vorbis_info ogg_info = stb_vorbis_get_info(ogg);
	SARC_RUNTIME_ASSERT(ogg_info.channels <= 2, std::runtime_error, "Too many channels in stream, max 2 supported.");

	const uint32_t sample_count = stb_vorbis_stream_length_in_samples(ogg);

	auto *samples = new float[sample_count * 2];
	SARC_RUNTIME_ASSERT(stb_vorbis_get_samples_float_interleaved(ogg, ogg_info.channels, ogg_info.channels == 2 ? samples : samples + sample_count, sample_count) == sample_count, std::runtime_error, "Samples read is not equal to total samples.");

	if (ogg_info.channels == 1) BinPlayer::mono_to_stereo(samples + sample_count, samples, sample_count);

	m_data.reserve(sample_count * 2 * sizeof(float));
	std::memcpy(m_data.data(), samples, sample_count * 2 * sizeof(float));

	delete[] samples;
	stb_vorbis_close(ogg);
}

void SampleResource::from_wav(const uint8_t *input_buffer, const size_t length) {
	drwav wav;
	SARC_RUNTIME_ASSERT(drwav_init_memory(&wav, input_buffer, length, nullptr), std::runtime_error, "Failed to load WAV data, check the WAV file is valid.");

	SARC_RUNTIME_ASSERT(wav.channels <= 2, std::runtime_error, "Too many channels in stream, max 2 supported.");

	auto *samples = new float[wav.totalPCMFrameCount * 2];
	const drmp3_uint64 sample_count = drwav_read_pcm_frames_f32(&wav, wav.totalPCMFrameCount, wav.channels == 2 ? samples : samples + wav.totalPCMFrameCount);

	SARC_RUNTIME_ASSERT(sample_count == wav.totalPCMFrameCount, std::runtime_error, "Samples read is not equal to total samples.");

	if (wav.channels == 1) BinPlayer::mono_to_stereo(samples + sample_count, samples, sample_count);

	m_data.reserve(sample_count * 2 * sizeof(float));
	std::memcpy(m_data.data(), samples, sample_count * 2 * sizeof(float));

	delete[] samples;
	drwav_free(&wav, nullptr);
}
