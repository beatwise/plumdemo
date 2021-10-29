/*
 * Copyright (c) 2021 Alessandro De Santis
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include <array>
#include <atomic>

#include "plum.h"
#include "plumhelpers.h"


#include "../abcdwindow.h"

#include "voice.h"

namespace demo {


class DSynthGui;

class DSynth : public plum::iplugin, public plum::istorage
{
	friend class DSynthGui;

public:
	PLUM_IOBJECT_RC_IMPL(m_rc, DSynth)

	void *as(const char *ifid)
	{
		if (std::string(ifid) == IFID_PLUM_OBJECT)
		{
			reference(); return static_cast<plum::iplugin *>(this);
		}
		else if (std::string(ifid) == IFID_PLUM_PLUGIN)
		{
			reference(); return static_cast<plum::iplugin *>(this);
		}
		else if (std::string(ifid) == IFID_PLUM_STORAGE)
		{
			reference(); return static_cast<plum::istorage *>(this);
		}

		return nullptr;
	}


	DSynth(plum::ihost *, bool nogui = false);
	virtual ~DSynth();

	const char *get_name() override;

	// GUI
	plum::iwindow *open_ui(plum::ihostwindow *) override;
	void on_gui_closed();

	// PROCESSING
	void configure(uint32_t samplerate, uint32_t buffer_size) override;
	void activate() override;
	void deactivate() override;

	void midi_event(uint8_t *data) override;
	void process(uint32_t nframes, float **ins, float **outs) override;

	// PRESETS
	uint32_t count_presets() override;
	uint32_t get_selected_preset() override;
	void set_selected_preset(uint32_t index) override;

	plum::istring* get_preset_name(uint32_t index) override;
	void set_preset_name(uint32_t index, plum::istring *) override;

	// IO
	uint32_t count_inputs() override;
	plum::istring *get_input_name(uint32_t index) override;
	uint32_t count_outputs() override;
	plum::istring *get_output_name(uint32_t index) override;

	//PARAMETERS
	uint32_t count_parameters() override;
	float get_parameter(uint32_t index) override;
	void set_parameter(uint32_t index, float value) override;
	void get_parameter_def(uint32_t index, plum_param_def *details) override;

	// STORAGE
	uint32_t set_preset_data(plum::iblob *) override;
	plum::iblob *get_preset_data() override;
	uint32_t set_bank_data(plum::iblob *) override;
	plum::iblob *get_bank_data() override;


private:

	void note_on(int number, int velocity);
	void note_off(int number, int velocity);

	plum::ihost *m_host {nullptr};
	DSynthGui *m_gui {nullptr};
	bool m_nogui;

	std::array<const char *, 2> channel_names {"left", "right"};

	uint32_t m_current_preset {0};
	std::atomic<preset_t *> m_preset {nullptr};

	const float m_voice_count = 8;
	std::vector<voice> m_voice;
	std::vector<float> m_bleft;
	std::vector<float> m_bright;
	float *m_buffer[2];


	plum_param_def m_defs[param_count]  {
		{PLUM_INTEGER, 0, 1, "osctype", 
			[](plum_param_def *, char *str, uint32_t size, float v) 
				{snprintf(str, size, "%s", int(v) == 0 ? "SQU" : "SAW");} },

		{PLUM_FLOAT, 0.01, 0.99, "pwm",
			[](plum_param_def *, char *str, uint32_t size, float v) 
				{snprintf(str, size, "%d %%", int(0.5 + 100.f*v));} },

		{PLUM_FLOAT, 0.01, 2, "attack",
			[](plum_param_def *, char *str, uint32_t size, float v) 
				{snprintf(str, size, "%3.2f s", v);} },

		{PLUM_FLOAT, 0.01, 2, "decay",
			[](plum_param_def *, char *str, uint32_t size, float v) 
				{snprintf(str, size, "%3.2f s", v);} },

		{PLUM_FLOAT, 0.01, 1, "sustain",
			[](plum_param_def *, char *str, uint32_t size, float v) 
				{snprintf(str, size, "%3.2f", v);} },

		{PLUM_FLOAT, 0.01, 2, "release",
			[](plum_param_def *, char *str, uint32_t size, float v) 
				{snprintf(str, size, "%3.2f s", v);} },
	};

	preset_t m_bank[6];

};




class DSynthGui : public abcdwindow
{
public:
	DSynthGui(plum::ihostwindow *hostwindow, DSynth *plugin) ;
	virtual ~DSynthGui() ;

	void refresh();
	void on_data_changed();

	void on_paste_text(plum::istring *str) override {}
	void on_timer(void *id) override {};

private:

	DSynth *m_plugin;
	void close();
	void do_gui(abcd::Draw &draw, abcd::rect frame) override;



	abcd::widget l_attack, l_decay, l_sustain, l_release, l_pwm;
	abcd::widget l_vattack, l_vdecay, l_vsustain, l_vrelease, l_vpwm;
	abcd::knob_widget k_attack, k_decay, k_sustain, k_release, k_pwm;

	abcd::widget r_squ, r_saw;
	abcd::widget l_squ, l_saw;

	abcd::list_widget li_names;
	abcd::widget i_name;
	abcd::widget b_rename;
	bool m_renaming {false};
	std::string m_name;
	std::vector<std::string> m_names;
};


} // demo