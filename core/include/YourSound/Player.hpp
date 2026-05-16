#pragma once

#include "YourSound/CoreExportImport.hpp"

#include <SArc/Helpers.hpp>

#include <cstdint>
#include <filesystem>
#include <functional>
#include <unordered_map>
#include <vector>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#endif

#define YS_LOAD_API_FN(api, name) api.name = p_get_dll_func<decltype(api.name)>("player_" #name)

typedef void *YS_ResourceHandle;
typedef void *YS_PlayerHandle;
typedef void *YS_PlayerWrapperHandle;
typedef void *YS_ImContextHandle;

namespace YourSound {
	SARC_ADD_RUNTIME_ERROR(invalid_binary_player);
	SARC_ADD_RUNTIME_ERROR(player_not_found);

#ifdef _WIN32
	inline std::unordered_map<std::filesystem::path, HMODULE> loaded_dlls;
#endif

	struct PlayerAPI {
		using note_on_t = void(*)(YS_PlayerHandle, uint8_t, float);
		using note_off_t = void(*)(YS_PlayerHandle, uint8_t);

		using render_t = void(*)(YS_PlayerHandle, float*, uint16_t);

		using store_calc_size_t = uint64_t(*)(YS_PlayerHandle, bool);
		using store_t = void(*)(YS_PlayerHandle, uint8_t*, bool);
		using load_t = void(*)(YS_PlayerHandle, const uint8_t*);

		using set_bpm_t = void(*)(YS_PlayerHandle, uint8_t);
		using set_sample_rate_t = void(*)(YS_PlayerHandle, uint32_t);

		using set_parameter_t = void(*)(YS_PlayerHandle, const char*, float);
		using get_parameters_t = void(*)(YS_PlayerHandle, const char**);
		using get_parameter_count_t = uint8_t(*)(YS_PlayerHandle);

		using render_graphics_t = void(*)(YS_PlayerHandle, YS_ImContextHandle);

		using reset_t = void(*)(YS_PlayerHandle);

		using get_id_t = const char *(*)(YS_PlayerHandle);

		note_on_t note_on = nullptr;
		note_off_t note_off = nullptr;

		render_t render = nullptr;

		store_calc_size_t store_calc_size = nullptr;
		store_t store = nullptr;
		load_t load = nullptr;

		set_bpm_t set_bpm = nullptr;
		set_sample_rate_t set_sample_rate = nullptr;

		set_parameter_t set_parameter = nullptr;
		get_parameters_t get_parameters = nullptr;
		get_parameter_count_t get_parameter_count = nullptr;

		render_graphics_t render_graphics = nullptr;

		reset_t reset = nullptr;

		get_id_t get_id = nullptr;
	};

	class Player {
		public:
			virtual ~Player() = default;

			virtual void note_on(uint8_t midi_note_number, float velocity) = 0;
			virtual void note_off(uint8_t midi_note_number) = 0;

			virtual void render(float *output_buffer, uint16_t number_samples) = 0;

			[[nodiscard]] virtual uint64_t store_calc_size(bool store_reference) const = 0;
			virtual void store(uint8_t *output_buffer, bool store_reference) const = 0;
			virtual void load(const uint8_t *input_buffer) = 0;

			virtual void set_bpm(uint16_t value) = 0;
			virtual void set_sample_rate(uint32_t value) = 0;

			virtual void set_parameter(const char *param_id, float value) = 0;
			virtual void get_parameters(const char **buffer) const = 0;
			[[nodiscard]] virtual uint8_t get_parameter_count() const = 0;

			virtual void render_graphics(YS_ImContextHandle im_context) = 0;

			virtual void reset() = 0;

			[[nodiscard]] virtual const char *get_id() const = 0;
	};

	class PlayerWrapper : public Player {
		public:
			virtual void set_wrapped_player(YS_PlayerHandle player) = 0;
	};

	class PlayerInterface : public Player {
		public:
			PlayerInterface(YS_PlayerHandle player_handle, const std::filesystem::path &dll_path);
			~PlayerInterface() override = default;

			void note_on(uint8_t midi_note_number, float velocity) override;
			void note_off(uint8_t midi_note_number) override;

			void render(float *output_buffer, uint16_t number_samples) override;

			[[nodiscard]] uint64_t store_calc_size(bool store_reference) const override;
			void store(uint8_t *output_buffer, bool store_reference) const override;
			void load(const uint8_t *input_buffer) override;

			void set_bpm(uint16_t value) override;
			void set_sample_rate(uint32_t value) override;

			void set_parameter(const char *param_id, float value) override;
			void get_parameters(const char **buffer) const override;
			[[nodiscard]] uint8_t get_parameter_count() const override;

			void render_graphics(YS_ImContextHandle im_context) override;

			void reset() override;

			[[nodiscard]] const char *get_id() const override;
		private:
			template<typename Func> Func p_get_dll_func(const char* name) const {
				static_assert(std::is_pointer_v<Func>, "Func must be a function pointer");

				void *lib = loaded_dlls.at(m_dll_path);

#if defined(_WIN32)
				void *sym = static_cast<void*>(GetProcAddress(static_cast<HMODULE>(lib), name));
#else
				void *sym = dlsym(lib, name);
#endif

				if (!sym) throw invalid_binary_player(std::string("Library ") + m_dll_path.string()
					+ std::string(" is missing the symbol: ") + name
					+ std::string("\nEnsure you have correctly created the registry using YS_START_BP_REGISTRY."));

				return reinterpret_cast<Func>(sym);
			}

			YS_PlayerHandle m_player_handle;
			const std::filesystem::path m_dll_path;
			PlayerAPI m_player_api;

			friend YS_CORE_EXPORT_NO_EXTERN void free_binary_player(const PlayerInterface *binary_player);
	};

	class YS_CORE_EXPORT_NO_EXTERN PlayerWrapperInterface : public PlayerWrapper, public PlayerInterface {
		PlayerWrapperInterface(YS_PlayerWrapperHandle player_wrapper_handle, const std::filesystem::path &dll_path);
	};

	class Resource {
		public:
			virtual ~Resource() = default;

			[[nodiscard]] virtual uint64_t store_calc_size(bool store_reference) const = 0;
			virtual void store(uint8_t *output_buffer, bool store_reference) const = 0;
			virtual void load(const uint8_t *input_buffer) = 0;
			virtual void reload_file() = 0;

			void set_path(const std::filesystem::path &path) {m_path = path;}

			size_t get_length() const {return m_data.size();}

			uint8_t *pointer() {return m_data.data();}
		protected:
			std::vector<uint8_t> m_data;
			std::filesystem::path m_path;
			bool m_force_embedded = false;
	};

	[[nodiscard]] YS_CORE_EXPORT_NO_EXTERN PlayerInterface *load_binary_player(const std::filesystem::path &dll_path, const std::string &player_id);
	[[nodiscard]] YS_CORE_EXPORT_NO_EXTERN Player *load_player_by_id(const std::string &player_id);
	[[nodiscard]] YS_CORE_EXPORT_NO_EXTERN PlayerWrapper *load_player_wrapper_by_id(const std::string &player_id);
}