#pragma once

#include <YourSound/Player.hpp>

namespace YourSound {
	class SampleResource : public Resource {
		public:
			[[nodiscard]] uint64_t store_calc_size(bool store_reference = true) const override;
			void store(uint8_t *output_buffer, bool store_reference = true) const override;
			void load(const uint8_t *input_buffer) override;

			void reload_file() override;

			void from_flac(const uint8_t *input_buffer, size_t length);
			void from_mp3(const uint8_t *input_buffer, size_t length);
			void from_ogg(const uint8_t *input_buffer, size_t length);
			void from_wav(const uint8_t *input_buffer, size_t length);

			[[nodiscard]] const float *pcm() const {return reinterpret_cast<const float*>(m_data.data());}

			bool lossless = false;
	};
}