#include "YourSound/IntegratedLoaders/BinLoaderSArcCompression.hpp"
#include "YourSound/BinPlayerInterface.hpp"
#include "YourSound/Serialisation.hpp"

#include <SArc/Helpers.hpp>

using namespace YourSound::BinLoader::Integrated;

bool BinLoaderSArcLZ4::read_resource(const read_callback_t read_callback, const write_callback_t write_callback) {
	try {
		std::array<uint8_t, 16> temp_array{};
		YS_RUNTIME_ASSERT(read_callback(temp_array.data(), 16), std::runtime_error, "16b read failed");

		const auto compressed_size = read_integral_be<size_t>(temp_array.data());
		const auto decompressed_size = read_integral_be<size_t>(temp_array.data());

		SArc::bytes_t compressed_data;
		compressed_data.resize(compressed_size);
		YS_RUNTIME_ASSERT(read_callback(reinterpret_cast<uint8_t *>(compressed_data.data()), compressed_size),
						  std::runtime_error, std::format("{}b read failed", compressed_size));

		const SArc::bytes_t decompressed_data = SArc::helpers::lz4_decompress(compressed_data, decompressed_size);

		YS_RUNTIME_ASSERT(
			write_callback(reinterpret_cast<const uint8_t *>(decompressed_data.data()), decompressed_size),
			std::runtime_error, std::format("{}b write failed", decompressed_size));
	} catch (std::exception &e) {
		ysbp_show_error(get_error_source(),
						std::format("Exception when reading SArcLZ4 resource:\n{}", e.what()).c_str());
		return false;
	}

	return true;
}

bool BinLoaderSArcLZ4::write_resource(const write_callback_t write_callback, const read_callback_t read_callback,
									  const size_t size) {
	try {
		std::array<uint8_t, 8> temp_array{};

		write_integral_be(temp_array.data(), size);
		YS_RUNTIME_ASSERT(write_callback(temp_array.data(), 8), std::runtime_error, "8b write failed");

		SArc::bytes_t decompressed_data;
		decompressed_data.resize(size);
		YS_RUNTIME_ASSERT(read_callback(reinterpret_cast<uint8_t *>(decompressed_data.data()), size),
						  std::runtime_error, std::format("{}b read failed", size));

		const SArc::bytes_t compressed_data = SArc::helpers::lz4_compress(decompressed_data, 12);

		write_integral_be(temp_array.data(), compressed_data.size());
		YS_RUNTIME_ASSERT(write_callback(temp_array.data(), 8), std::runtime_error, "8b write failed");

		YS_RUNTIME_ASSERT(
			write_callback(reinterpret_cast<const uint8_t *>(compressed_data.data()), compressed_data.size()),
			std::runtime_error, std::format("{}b write failed", compressed_data.size()));
	} catch (std::exception &e) {
		ysbp_show_error(get_error_source(),
						std::format("Exception when writing SArcLZ4 resource:\n{}", e.what()).c_str());
		return false;
	}

	return true;
}

bool BinLoaderSArcLZMA::read_resource(const read_callback_t read_callback, const write_callback_t write_callback) {
	try {
		std::array<uint8_t, 16> temp_array{};
		YS_RUNTIME_ASSERT(read_callback(temp_array.data(), 16), std::runtime_error, "16b read failed");

		const auto compressed_size = read_integral_be<size_t>(temp_array.data());
		const auto decompressed_size = read_integral_be<size_t>(temp_array.data());

		SArc::bytes_t compressed_data;
		compressed_data.resize(compressed_size);
		YS_RUNTIME_ASSERT(read_callback(reinterpret_cast<uint8_t *>(compressed_data.data()), compressed_size),
						  std::runtime_error, std::format("{}b read failed", compressed_size));

		const SArc::bytes_t decompressed_data = SArc::helpers::lzma_decompress(compressed_data, decompressed_size);

		YS_RUNTIME_ASSERT(
			write_callback(reinterpret_cast<const uint8_t *>(decompressed_data.data()), decompressed_size),
			std::runtime_error, std::format("{}b write failed", decompressed_size));
	} catch (std::exception &e) {
		ysbp_show_error(get_error_source(),
						std::format("Exception when reading SArcLZMA resource:\n{}", e.what()).c_str());
		return false;
	}

	return true;
}

bool BinLoaderSArcLZMA::write_resource(const write_callback_t write_callback, const read_callback_t read_callback,
									   const size_t size) {
	try {
		std::array<uint8_t, 8> temp_array{};

		write_integral_be(temp_array.data(), size);
		YS_RUNTIME_ASSERT(write_callback(temp_array.data(), 8), std::runtime_error, "8b write failed");

		SArc::bytes_t decompressed_data;
		decompressed_data.resize(size);
		YS_RUNTIME_ASSERT(read_callback(reinterpret_cast<uint8_t *>(decompressed_data.data()), size),
						  std::runtime_error, std::format("{}b read failed", size));

		const SArc::bytes_t compressed_data = SArc::helpers::lzma_compress(decompressed_data, 6);

		write_integral_be(temp_array.data(), compressed_data.size());
		YS_RUNTIME_ASSERT(write_callback(temp_array.data(), 8), std::runtime_error, "8b write failed");

		YS_RUNTIME_ASSERT(
			write_callback(reinterpret_cast<const uint8_t *>(compressed_data.data()), compressed_data.size()),
			std::runtime_error, std::format("{}b write failed", compressed_data.size()));
	} catch (std::exception &e) {
		ysbp_show_error(get_error_source(),
						std::format("Exception when writing SArcLZMA resource:\n{}", e.what()).c_str());
		return false;
	}

	return true;
}
