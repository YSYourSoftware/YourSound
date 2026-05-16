#pragma once

#include "YourSound/Player.hpp"

namespace YourSound {
	class BinaryResource : public Resource {
		public:
			[[nodiscard]] uint64_t store_calc_size(bool store_reference) const override;
			void store(uint8_t *output_buffer, bool store_reference) const override;
			void load(const uint8_t *input_buffer) override;
			void reload_file() override;
	};
}