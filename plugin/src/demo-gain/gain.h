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

namespace demo {


class GainGui;

class Gain : public plum::iplugin
{
	friend class GainGui;

public:
	PLUM_IOBJECT_RC_IMPL(m_rc, Gain)

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

		return nullptr;
	}


	Gain(plum::ihost *);
	virtual ~Gain();

	const char *get_name() override;

	plum::iwindow *open_ui(plum::ihostwindow *) override;
	void on_gui_closed();

	void configure(uint32_t samplerate, uint32_t buffer_size)				{}
	void activate() override												{printf("ACTIVATE demo::Gain\n");}
	void deactivate() override												{printf("DEACTIVATE demo::Gain\n");}

	void midi_event(uint8_t *data) override									{}
	void process(uint32_t nframes, float **ins, float **outs) override;

	plum::istring* get_preset_name(uint32_t index) override					{return nullptr;}
	void set_preset_name(uint32_t index, plum::istring *) override			{}

	uint32_t count_presets() override										{return 0;}
	uint32_t get_selected_preset() override									{return 0;}	
	void set_selected_preset(uint32_t index) override						{}

	uint32_t count_inputs() override;
	plum::istring *get_input_name(uint32_t index) override;
	uint32_t count_outputs() override;
	plum::istring *get_output_name(uint32_t index) override;

	uint32_t count_parameters() override;
	float get_parameter(uint32_t index) override;
	void set_parameter(uint32_t index, float value) override;
	void get_parameter_def(uint32_t index, plum_param_def *details) override;

private:
	plum::ihost *m_host {nullptr};
	GainGui *m_gui {nullptr};

	std::array<const char *, 2> channel_names {"left", "right"};

	// PARAMETERS
	std::atomic<float> m_gain {0.1};
	std::atomic<float> m_peakl;
	std::atomic<float> m_peakr;
};




class GainGui : public abcdwindow
{
public:
	GainGui(plum::ihostwindow *hostwindow, Gain *plugin) ;
	virtual ~GainGui() ;

	void on_paste_text(plum::istring *str) override {}
	void on_timer(void *id) override;

private:

	Gain *m_plugin;
	void close();
	void do_gui(abcd::Draw &draw, abcd::rect frame) override;


	abcd::widget l_gain;
	abcd::slider_widget sl_gain;

	int m_timer;
	abcd::widget l_outs;

	float gain {0};


};


} // demo

