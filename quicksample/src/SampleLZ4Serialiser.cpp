#include "SampleLZ4Serialiser.hpp"

#include "Audio.hpp"

#include <YourSound/Serialisation.hpp>

#include <SArc/Helpers.hpp>

#include <fstream>

void write_sample(const std::span<const float> &samples, const std::filesystem::path &file) {
	std::ofstream file_stream{file, std::ios::binary};

	file_stream.write("YSRCorg.yoursoftware.sarc.lz4\x00", 30);

	SArc::bytes_t data;
	data.resize(8 + 1 + 4 + 1 + 8 + samples.size_bytes());

	std::array<uint8_t, 8> tempvar_buf;
	SArc::bytes_t compressed;

	{
		std::memcpy(data.data(), "sampLOD\x00", 8);

		const bool stereo = g_channels == 2;
		std::memcpy(data.data() + 8, &stereo, 1);

		YourSound::write_integral_be(tempvar_buf.data(), g_sample_rate);
		std::memcpy(data.data() + 9, tempvar_buf.data(), 4);

		constexpr uint8_t sample_format = 3; // float32 (little endian)
		std::memcpy(data.data() + 13, &sample_format, 1);

		const size_t sample_count = samples.size() / g_channels;
		YourSound::write_integral_be(tempvar_buf.data(), sample_count);
		std::memcpy(data.data() + 14, tempvar_buf.data(), 8);

		std::memcpy(data.data() + 22, samples.data(), samples.size_bytes());

		compressed = SArc::helpers::lz4_compress(data, 12);
	}

	YourSound::write_integral_be(tempvar_buf.data(), compressed.size());
	file_stream.write(reinterpret_cast<const char *>(tempvar_buf.data()), 8);

	YourSound::write_integral_be(tempvar_buf.data(), data.size());
	file_stream.write(reinterpret_cast<const char *>(tempvar_buf.data()), 8);

	file_stream.write(reinterpret_cast<const char *>(compressed.data()), compressed.size());
}
