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

#include "gain.h"

namespace demo {

Gain::Gain(plum::ihost *)
{ 
	printf("NEW demo::Gain\n"); 
}

Gain::~Gain()
{ 
	printf("DEL demo::Gain\n"); 
}

const char *Gain::get_name()
{
	return "demoGain";
}

plum::iwindow *Gain::open_ui(plum::ihostwindow *hostwindow)
{
	if (m_gui)
	{
		return nullptr;
	}

	m_gui = new GainGui(hostwindow, this);

	return (plum::iwindow *)m_gui->as(IFID_PLUM_WINDOW);
}

void Gain::on_gui_closed()
{
	m_gui->release();
	m_gui = nullptr;
}

uint32_t Gain::count_inputs()
{
	return channel_names.size();
}

plum::istring *Gain::get_input_name(uint32_t index)
{
	return new plum::string(channel_names[index]);
}

uint32_t Gain::count_outputs()
{
	return channel_names.size();
}

plum::istring *Gain::get_output_name(uint32_t index)
{
	return new plum::string(channel_names[index]);
}

uint32_t Gain::count_parameters()
{
	return 1;
}

float Gain::get_parameter(uint32_t index)
{
	float v = 0;

	switch (index)
	{
		case 0: 
			v = 20 * log10(m_gain.load());
			break;
	}
	
	return v;
}

void Gain::set_parameter(uint32_t index, float value)
{
	switch (index)
	{
		case 0: 
			m_gain = pow(10, value / 20);
			break;
	}
}

void Gain::get_parameter_def(uint32_t index, plum_param_def *details)
{
	switch (index)
	{
		case 0: 
			details->type = PLUM_FLOAT;
			details->name = "gain";
			details->format = [](plum_param_def *, char *str, uint32_t size, float v) 
				{
					snprintf(str, size, "%3.0f DB", v);
				};
			break;
	}
}

void Gain::process(uint32_t nframes, float **ins, float **outs)
{
	float *inL, *inR;
	float *outL, *outR;

	inL = ins[0]; inR = ins[1];
	outL = outs[0]; outR = outs[1]; 

	float peakl = 0, peakr = 0;

	for (uint32_t i = 0; i < nframes; ++i)
	{
		*outL = *inL * m_gain;
		*outR = *inR * m_gain;

		float al = fabs(*outL);
		float ar = fabs(*outR);
		peakl = std::max(al, peakl);
		peakr = std::max(ar, peakr);

		inL ++;
		inR ++;
	
		outL ++;
		outR ++;
	}

	m_peakl = peakl;
	m_peakr = peakr;
}



} // demo
