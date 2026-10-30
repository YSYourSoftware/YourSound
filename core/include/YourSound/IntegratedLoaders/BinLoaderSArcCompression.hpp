#pragma once

#include "YourSound/BinLoaderRegistry.hpp"
#include "YourSound/CoreExportImport.hpp"
#include "YourSound/Resource.hpp"

namespace YourSound::BinLoader::Integrated {
	class YS_API BinLoaderSArcLZ4 : public ResourceLoader {
	public:
		bool read_resource(read_callback_t read_callback, write_callback_t write_callback) override;
		bool write_resource(write_callback_t write_callback, read_callback_t read_callback, size_t size) override;

		const char *get_id() override { return "org.yoursoftware.sarc.lz4"; }
	};

	class YS_API BinLoaderSArcLZMA : public ResourceLoader {
	public:
		bool read_resource(read_callback_t read_callback, write_callback_t write_callback) override;
		bool write_resource(write_callback_t write_callback, read_callback_t read_callback, size_t size) override;

		const char *get_id() override { return "org.yoursoftware.sarc.lzma"; }
	};

	inline YS_API void register_sarc_compression_loaders() {
		g_bl_registry["org.yoursoftware.sarc.lz4"] = [] { return new BinLoaderSArcLZ4(); };
		g_bl_registry["org.yoursoftware.sarc.lzma"] = [] { return new BinLoaderSArcLZMA(); };
	}
} // namespace YourSound::BinLoader::Integrated
