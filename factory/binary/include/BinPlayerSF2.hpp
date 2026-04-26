#pragma once

#include <YourSound/BinPlayerSTD.hpp>
#include <YourSound/Rack.hpp>

YS_EXTERN_EXPORT void action_import_sf2(const char *filepath);
YS_EXTERN_EXPORT void action_load_sf2(const char *filepath, YS_RackHandle rack);

struct tsf;

class BinPlayerSF2 : public YourSound::Player {
	public:
		BinPlayerSF2() {sf2_load_from_file(R"(D:\FLStudio\Data\Patches\Soundfonts\Roland Fantom X SoundFont\00 Ac.Piano\F009 UltimatGrand.sf2)");}
		~BinPlayerSF2() override;

		void note_on(uint8_t midi_note_number, float velocity) override;
		void note_off(uint8_t midi_note_number) override;

		void render(float *output_buffer, uint16_t number_samples) override;

		uint64_t store_calc_size(bool store_reference) const override;
		void store(uint8_t *output_buffer, bool store_reference) const override;
		void load(const uint8_t *input_buffer) override;

		void set_bpm(uint16_t value) override;
		void set_sample_rate(uint32_t value) override;

		void set_parameter(const char *param_id, float value) override {}
		void get_parameters(const char **buffer) const override {}
		uint8_t get_parameter_count() const override {return 0;}

		void reset() override;

		void render_graphics(YS_ImContextHandle im_context) override;

		void sf2_load_from_file(const char *filepath);

		const char *get_id() const override {return "org.yoursoftware.sound.sf2";}
	private:
		YourSound::Resource m_soundfont_resource;

		tsf *m_soundfont = nullptr;
		uint16_t m_preset_id = 0;

		float m_knob_row_width = 0.f;

		float m_gain = 0.f;

		uint8_t m_bpm = 0;
		uint32_t m_sample_rate = 0;

		friend void action_import_sf2(const char *filepath);
};