#include "YourSound/Player.hpp"

#include "YourSound/Serialisation.hpp"

#include <SArc/Helpers.hpp>

#include <fstream>

using namespace YourSound;

uint64_t Resource::store_calc_size(const bool store_reference) const {
	// data_embedded[1] + decompressed_data_length[8] + compressed_data_length[8] + compressed_data[?]
	if (!store_reference || m_force_embedded) return 1 + 8 + 8 + SArc::helpers::lzma_get_compressed_size(std::span(reinterpret_cast<const std::byte*>(m_data.data()), m_data.size()), m_data.size());

	// data_embedded[1] + utf8_path[?] + null_terminator[1]
	return 1 + m_path.u8string().size() + 1;
}

void Resource::store(uint8_t *output_buffer, const bool store_reference) const {
	if (store_reference && !m_force_embedded) {
		*output_buffer = 0;

		const std::u8string path = m_path.u8string();
		std::memcpy(output_buffer + 1, path.c_str(), path.size() + 1);

		return;
	}

	*output_buffer = 1;

	const SArc::bytes_t compressed = SArc::helpers::lzma_compress(std::span(reinterpret_cast<const std::byte*>(m_data.data()), m_data.size()), m_data.size());

	YourSound::write_integral_be<uint64_t>(output_buffer + 1, m_data.size());
	YourSound::write_integral_be<uint64_t>(output_buffer + 9, compressed.size());
	std::memcpy(output_buffer + 17, compressed.data(), compressed.size());
}

void Resource::load(const uint8_t *input_buffer) {
	if (*input_buffer == 0) {
		m_path = std::filesystem::path(reinterpret_cast<const char*>(input_buffer + 1));

		reload_file();

		return;
	}

	const auto decompressed_size = YourSound::read_integral_be<uint64_t>(input_buffer + 1);
	const auto compressed_size = YourSound::read_integral_be<uint64_t>(input_buffer + 9);
	const SArc::bytes_t decompressed_data = SArc::helpers::lzma_decompress(std::span(reinterpret_cast<const std::byte*>(input_buffer + 17), compressed_size), decompressed_size);

	m_data.resize(decompressed_size);
	std::memcpy(m_data.data(), decompressed_data.data(), decompressed_size);
	m_force_embedded = true;
}

void Resource::reload_file() {
	if (m_force_embedded) return;

	std::ifstream file{m_path, std::ios::binary | std::ios::ate};
	if (!file) throw SArc::io_error("Could not open file: " + m_path.string());

	file.seekg(0, std::ios::end);
	const std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	m_data.resize(size);
	if (!file.read(reinterpret_cast<char*>(m_data.data()), size)) throw SArc::io_error("Could not read file: " + m_path.string());
}

char *Resource::get_char_pointer() {return reinterpret_cast<char*>(m_data.data());}
unsigned char *Resource::get_uchar_pointer() {return m_data.data();}

size_t Resource::get_length() const {return m_data.size();}
