#pragma once

#include <cstdint>
#include <cstring>
#include <type_traits>

namespace YourSound {
	template<typename T> void write_integral_be(uint8_t *buffer, T value) {
		static_assert(std::is_integral_v<T>, "T must be an integral type");

		using U = std::make_unsigned_t<T>;
		U uvalue = static_cast<U>(value);

		for (size_t i = 0; i < sizeof(T); i++) buffer[i] = static_cast<uint8_t>((uvalue >> (8 * (sizeof(T) - 1 - i))) & 0xFF);
	}

	template<typename T> T read_integral_be(const uint8_t *buffer) {
		static_assert(std::is_integral_v<T>, "T must be an integral type");

		T value = 0;
		for (size_t i = 0; i < sizeof(T); i++) value |= static_cast<T>(buffer[i]) << (8 * (sizeof(T) - 1 - i));
		return value;
	}

	template<typename T> void write_float_be(uint8_t *buffer, T value) {
		static_assert(std::is_floating_point_v<T>, "T must be a floating point type");

		using Int = std::conditional_t<sizeof(T) == 4, uint32_t, uint64_t>;
		Int bits;
		std::memcpy(&bits, &value, sizeof(T));

		for (size_t i = 0; i < sizeof(T); i++) buffer[i] = static_cast<uint8_t>(bits >> (8 * (sizeof(T) - 1 - i)));
	}

	template<typename T> T read_float_be(const uint8_t *buffer) {
		static_assert(std::is_floating_point_v<T>, "T must be a floating point type");

		using Int = std::conditional_t<sizeof(T) == 4, uint32_t, uint64_t>;
		Int bits = 0;
		for (size_t i = 0; i < sizeof(T); i++) bits |= static_cast<Int>(buffer[i]) << (8 * (sizeof(T) - 1 - i));

		T value;
		std::memcpy(&value, &bits, sizeof(T));
		return value;
	}
}