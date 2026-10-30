#pragma once

#include <cstdint>
#include <functional>

namespace YourSound {
	class ResourceLoader {
	public:
		virtual ~ResourceLoader() = default;

		typedef std::function<bool(const uint8_t *, size_t)> write_callback_t;
		typedef std::function<bool(uint8_t *, size_t)> read_callback_t;

		virtual bool read_resource(read_callback_t read_callback, write_callback_t write_callback) = 0;
		virtual bool write_resource(write_callback_t write_callback, read_callback_t read_callback, size_t size) = 0;

		virtual const char *get_id() = 0;
	};
} // namespace YourSound
