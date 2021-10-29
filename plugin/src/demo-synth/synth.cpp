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

#include <fstream>
#include <sstream>

#include "synth.h"
#include "../utils.h"

namespace demo {


DSynth::DSynth(plum::ihost *host, bool nogui)
{ 
	printf("NEW demo::DSynth\n"); 

	m_host = host;
	m_nogui = nogui;
	m_voice.resize(m_voice_count);

	m_bank[0].define(m_defs, {0, 0.10, 0.25, 0.25, 0.5, 2}, "Square 1");
	m_bank[1].define(m_defs, {0, 0.35, 0.25, 0.25, 0.5, 2}, "Square 2");
	m_bank[2].define(m_defs, {0, 0.50, 0.25, 0.25, 0.5, 2}, "Square 3");
	m_bank[3].define(m_defs, {0, 0.75, 0.25, 0.25, 0.5, 2}, "Square 4");
	m_bank[4].define(m_defs, {1, 0.1, 0.25, 0.25, 0.5, 2}, "Sawtooth 1");
	m_bank[5].define(m_defs, {1, 0.1, 0.10, 0.25, 0.5, 1}, "Sawtooth 2");

	set_selected_preset(0);
}

DSynth::~DSynth()
{ 
	if (m_preset)
	{
		delete m_preset;
	}

	printf("DEL demo::DSynth\n"); 
}

const char *DSynth::get_name()
{
	return "DSynth";
}

// -----------------------------------------------------------
// GUI

plum::iwindow *DSynth::open_ui(plum::ihostwindow *hostwindow)
{
	if (m_nogui)
	{
		return nullptr;
	}

	if (m_gui)
	{
		return nullptr;
	}

	m_gui = new DSynthGui(hostwindow, this);

	return (plum::iwindow *)m_gui->as(IFID_PLUM_WINDOW);
}

void DSynth::on_gui_closed()
{
	m_gui->release();
	m_gui = nullptr;
}


// -----------------------------------------------------------
// IO

uint32_t DSynth::count_inputs()
{
	return 0;
}

plum::istring *DSynth::get_input_name(uint32_t index)
{
	return nullptr;
}

uint32_t DSynth::count_outputs()
{
	return channel_names.size();
}

plum::istring *DSynth::get_output_name(uint32_t index)
{
	return new plum::string(channel_names[index]);
}

// -----------------------------------------------------------
// PRESETS

plum::istring* DSynth::get_preset_name(uint32_t index)
{
	return new plum::string(m_bank[index].name.c_str());
}

void DSynth::set_preset_name(uint32_t index, plum::istring *name)
{
	m_bank[index].name = name->text();
	name->release();

	m_host->plugin_preset_changed(this, index);
}

uint32_t DSynth::count_presets()					
{
	return sizeof(m_bank) / sizeof(preset_t);
}

uint32_t DSynth::get_selected_preset()
{
	return m_current_preset;
}	

void DSynth::set_selected_preset(uint32_t index)
{
	m_current_preset = index;
	auto new_sel = m_bank[index].clone();

	auto old = m_preset.exchange(new_sel);
	delete old;

	if (m_gui)
	{
		m_gui->refresh();
	}

	m_host->plugin_preset_selected(this);
}


// -----------------------------------------------------------
// PARAMETERS

uint32_t DSynth::count_parameters()
{
	return sizeof(m_defs) / sizeof(plum_param_def);
}

float DSynth::get_parameter(uint32_t index)
{
	return m_bank[m_current_preset].get(index);
}

void DSynth::set_parameter(uint32_t index, float value)
{
	m_bank[m_current_preset].set(index, value);
	m_preset.load()->set(index, value);
}

void DSynth::get_parameter_def(uint32_t index, plum_param_def *def)
{
	*def = m_defs[index];
}


// -----------------------------------------------------------
// PROCESSING

void DSynth::activate()
{
	printf("ACTIVATE demo::DSynth\n"); 
}

void DSynth::deactivate()
{
	printf("DEACTIVATE demo::DSynth\n"); 
}


void DSynth::midi_event(uint8_t *data)
{
	switch (*data & 0xF0)
	{
		case 0x90:  
			if (data[2] > 0)
				note_on(data[1], data[2]);
			else		
				note_on(data[1], 0);
			break;
		
		case 0x80:
			note_off(data[1], 0); 
			break;

	}
}

void DSynth::note_on(int number, int velocity)
{
	auto it = std::find_if(m_voice.begin(), m_voice.end(), 
		[](voice &voice) {return voice.is_free();});

	if (it != m_voice.end())
	{ 
		it->start(number, velocity, 1, &m_preset);
//printf("NOTE ON %d %ld\n", number, it - m_voice.begin());
	}

}

void DSynth::note_off(int number, int velocity)
{
	auto it = std::find_if(m_voice.begin(), m_voice.end(), 
		[number](voice &voice) 
		{
			return voice.is_held() && (voice.midi_note() == number);
		}
	);

	if (it != m_voice.end())
	{
		it->release(velocity);
//printf("NOTE OFF %d %ld\n", number, it - m_voice.begin());
	}

}


void DSynth::configure(uint32_t samplerate, uint32_t buffer_size)
{
	Tonic::setSampleRate(samplerate);

	m_bleft.resize(buffer_size);
	m_bright.resize(buffer_size);
	m_buffer[0] = m_bleft.data();
	m_buffer[1] = m_bright.data();
}

void DSynth::process(uint32_t nframes, float **ins, float **outs)
{
	std::fill(outs[0], outs[0] + nframes, 0);
	std::fill(outs[1], outs[1] + nframes, 0);

	for (auto &v : m_voice)
	{
		if (!v.is_free())
		{
			v.process(m_buffer, nframes);

			for (size_t i = 0; i < nframes; ++i)
			{
				outs[0][i] += m_bleft[i] / m_voice_count;
				outs[1][i] += m_bright[i] / m_voice_count;
			}
		}
	}
}



// -----------------------------------------------------------
// STORAGE

/*
	SINGLE PRESET DATA
----------------------------
	plum 1.0
	dsynth 1.0
    preset 
	name
    0.0
    0.0
    0.0
    0.0
    0.0
    0.0

	BANK DATA
----------------------------
	plum 1.0
	dsynth 1.0
    bank 6
	name
    0.0
    0.0
    0.0
    0.0
    0.0
    0.0
	name
    0.0
    0.0
    0.0
    0.0
    0.0
    0.0
	...

*/



uint32_t DSynth::set_preset_data(plum::iblob *blob)
{
	//TODO: validate

	std::vector<uint8_t> buffer((uint8_t *)blob->data(), (uint8_t *)blob->data() + blob->size());
	blob->release();

	uint32_t vmaj, vmin;
	std::string s, name;

	size_t pos = 0;

	// PLUM
	s = read_string(pos, buffer);	

	vmaj = read_uint32(pos, buffer);
	vmin = read_uint32(pos, buffer);
	if (vmaj != 1 || vmin != 0) return false;
	

	// DSYNT
	s = read_string(pos, buffer);	

	vmaj = read_uint32(pos, buffer);
	vmin = read_uint32(pos, buffer);
	if (vmaj != 1 || vmin != 0) return false;

	// PRESET
	s = read_string(pos, buffer);	

	// NAME
	name = read_string(pos, buffer);	

	for (int i = 0; i < param_count; ++i)
	{
		float v = read_float32(pos, buffer);
		m_bank[m_current_preset].data[i] = v;
	}

	m_bank[m_current_preset].name = name;

	set_selected_preset(m_current_preset);

	m_host->plugin_preset_changed(this, m_current_preset);

	if (m_gui)
	{
		m_gui->on_data_changed();
	}

	return true;
}

plum::iblob *DSynth::get_preset_data()
{
	preset_t &preset = m_bank[m_current_preset];

	std::vector<uint8_t> buffer;
	append_string("plum", buffer);
	append_uint32(1, buffer);
	append_uint32(0, buffer);
	append_string("dsynth", buffer);
	append_uint32(1, buffer);
	append_uint32(0, buffer);
	append_string("preset", buffer);
	append_string(preset.name, buffer);
	for (auto &p : preset.data)
	{
		auto v = p.load();
		append_float32(v, buffer);			
	}

	return new plum::blob(buffer.data(), buffer.size());
}

uint32_t DSynth::set_bank_data(plum::iblob *blob)
{
	//TODO: validate

	std::vector<uint8_t> buffer((uint8_t *)blob->data(), (uint8_t *)blob->data() + blob->size());
	blob->release();

	uint32_t vmaj, vmin;
	std::string s, name;

	size_t pos = 0;

	// PLUM
	s = read_string(pos, buffer);	

	vmaj = read_uint32(pos, buffer);
	vmin = read_uint32(pos, buffer);
	if (vmaj != 1 || vmin != 0) return false;

	// DSYNT
	s = read_string(pos, buffer);	

	vmaj = read_uint32(pos, buffer);
	vmin = read_uint32(pos, buffer);
	if (vmaj != 1 || vmin != 0) return false;

	// BANK
	s = read_string(pos, buffer);	

	uint32_t count = read_uint32(pos, buffer);

	for (uint32_t j = 0; j < count; ++j)
	{
		// NAME
		name = read_string(pos, buffer);	

		for (int i = 0; i < param_count; ++i)
		{
			float v = read_float32(pos, buffer);
			m_bank[j].data[i] = v;
		}

		m_bank[j].name = name;
	}

	m_host->plugin_bank_changed(this);
	if (m_gui)
	{
		m_gui->on_data_changed();
	}

	set_selected_preset(0);

	return true;

}

plum::iblob *DSynth::get_bank_data()
{
	uint32_t count = sizeof(m_bank) / sizeof(preset_t);

	std::vector<uint8_t> buffer;
	append_string("plum", buffer);
	append_uint32(1, buffer);
	append_uint32(0, buffer);
	append_string("dsynth", buffer);
	append_uint32(1, buffer);
	append_uint32(0, buffer);
	append_string("bank", buffer);
	append_uint32(count, buffer);
	for (auto &preset : m_bank)
	{
		append_string(preset.name, buffer);
		for (auto &p : preset.data)
		{
			auto v = p.load();
			append_float32(v, buffer);			
		}
	}

	return new plum::blob(buffer.data(), buffer.size());
}



} // demo
