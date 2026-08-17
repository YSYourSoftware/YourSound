#pragma once

#include "YourSound/BinPlayerRegistry.hpp"
#include "YourSound/BinPlayerSTD.hpp"
#include "YourSound/Player.hpp"

#include <sol/sol.hpp>

#undef ERROR

class TextEditor;

namespace YourSound::BinPlayer::Integrated {
	class YS_API BinPlayerLua : public virtual Player {
	public:
		BinPlayerLua();
		~BinPlayerLua() override;

		void note_on(uint8_t midi_note_number, float velocity) override;
		void note_off(uint8_t midi_note_number) override;

		void render(float *output_buffer, uint16_t number_samples) override;

		uint64_t store_calc_size(bool store_reference) override;
		void store(uint8_t *output_buffer, bool store_reference) override;
		void load(const uint8_t *input_buffer) override;

		void set_bpm(uint16_t value) override;
		void set_sample_rate(uint32_t value) override;

		void set_parameter(const char *param_id, float value) override;
		float get_parameter(const char *param_id) override;
		void get_parameters(const char **buffer) override;
		uint8_t get_parameter_count() override;

		void render_graphics() override;

		void reset() override;

		const char *get_id() override { return "org.yoursoftware.sound.lua"; }
	protected:
		typedef struct {
			enum Severity : uint8_t {
				INFO = 0,
				WARNING = 1,
				ERROR = 2,
				EXCEPTION = 3
			};

			std::string message;
			Severity severity;
		} LogMessage;

		void p_log(const std::string &message);
		void p_log_warning(const std::string &message);
		void p_log_error(const std::string &message);
		void p_exception(const std::string &message);

		void p_write_samples(float l_sample, float r_sample);

		void p_store_string(const std::string &string);
		void p_store_number(lua_Number number);

		std::string p_load_string();
		lua_Number p_load_number();

		void p_add_parameter(const std::string &id);

		template <typename... Args> void p_emit(const std::string &name, Args &&...args) const {
			if (const auto it = m_lua_subscribers.find(name); it != m_lua_subscribers.end())
				it->second(std::forward<Args>(args)...);
		}

		void p_emit_param(const std::string &param, float value) const;

		sol::state m_lua_state;
		std::unordered_map<std::string, sol::function> m_lua_subscribers;
		std::unordered_map<std::string, sol::function> m_lua_param_subscribers;

		float *m_sample_write_pos = nullptr;
		uint16_t m_sample_write_target = 0;
		uint16_t m_samples_written = 0;

		const uint8_t *m_load_data = nullptr;
		uint8_t *m_store_data_pos = nullptr;
		uint64_t m_store_data_size = 0;
		uint64_t m_target_data_size = 0;

		std::vector<std::string> m_parameters;

		bool m_release_mode = false;
		TextEditor *m_text_editor = nullptr;
		std::vector<LogMessage> m_log_messages;
		uint32_t m_exception_line = 0;
	};

	/*class YS_API BinPlayerLuaWrapper : public PlayerWrapper, public BinPlayerLua {
	public:
		BinPlayerLuaWrapper();

		void render(float *output_buffer, uint16_t number_samples) override;

		uint64_t store_calc_size(bool store_reference) override;
		void store(uint8_t *output_buffer, bool store_reference) override;
		void load(const uint8_t *input_buffer) override;

		void set_bpm(uint16_t value) override;
		void set_sample_rate(uint32_t value) override;

		void set_parameter(const char *param_id, float value) override;

	float get_parameter(const char *param_id) override;
		void get_parameters(const char **buffer) override;
		uint8_t get_parameter_count() override;

		void render_graphics() override;

		const char *get_id() override { return "org.yoursoftware.sound.lua-wrapper"; }

		void set_wrapped_player(YS_PlayerHandle player) override { m_player = static_cast<Player *>(player); }
	private:
		void p_super_func();
		void p_store_player();

		Player *m_player = nullptr;
	};*/

	inline YS_API void register_lua_players() {
		g_bp_registry["org.yoursoftware.sound.lua"] = [] { return new BinPlayerLua(); };
		/* g_bp_registry["org.yoursoftware.sound.lua-wrapper"] = [] {
			return static_cast<Player *>(new BinPlayerLuaWrapper());
		};*/
	}
} // namespace YourSound::BinPlayer::Integrated
