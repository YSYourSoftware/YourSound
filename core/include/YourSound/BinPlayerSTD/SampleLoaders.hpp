#pragma once

#include "YourSound/BinPlayerSTD.hpp"
#include "YourSound/Serialisation.hpp"

#include <array>

namespace YourSound::BinPlayer {
	class YSRCSampleLoader : public SampleLoader {
	public:
		static sample_t load(std::istream stream) {
			std::array<uint8_t, 256> temp_buf;

			stream.read(reinterpret_cast<char *>(temp_buf.data()), 8);
			YS_RUNTIME_ASSERT(std::strcmp(reinterpret_cast<char const *>(temp_buf.data()), "sampLOD") == 0,
							  std::runtime_error, "Attempted to load non-sample resource as sample.");

			stream.read(reinterpret_cast<char *>(temp_buf.data()), 1 + 4 + 1 + 8);
			sample_t out{temp_buf[0] == 1};

			p_sample_set_sample_rate(out, YourSound::read_integral_be<uint32_t>(temp_buf.data() + 1));

			const auto sample_format = static_cast<sample_t::SampleFormat>(temp_buf[5]);
			p_sample_set_sample_format(out, sample_format);

			const uint64_t sample_count = YourSound::read_integral_be<uint64_t>(temp_buf.data() + 6);

			std::vector<uint8_t> &data = p_sample_get_data(out);
			data.reserve(sample_count * sample_t::sample_format_sizes[sample_format] * (out.stereo ? 2 : 1));

			size_t to_read = data.capacity();
			for (; to_read >= temp_buf.size(); to_read -= temp_buf.size()) {
				stream.read(reinterpret_cast<char *>(temp_buf.data()), temp_buf.size());
				data.insert(data.end(), temp_buf.begin(), temp_buf.end());
			}

			stream.read(reinterpret_cast<char *>(temp_buf.data()), to_read);
			data.insert(data.end(), temp_buf.begin(), temp_buf.begin() + to_read);

			return out;
		}
	};
} // namespace YourSound::BinPlayer
