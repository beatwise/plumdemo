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

#include <algorithm>

#include "engine.h"

trackitem::trackitem(plum::iplugin *p, uint32_t buffer_size) : plugin(p)
{
	plugin->reference();
	allocate_io(buffer_size);
	plugin->activate();
}

trackitem::~trackitem()
{
	plugin->deactivate();
	plugin->release();
}

void trackitem::allocate_io(uint32_t buffer_size)
{
	buffersize = buffer_size;

	uint32_t ni = plugin->count_inputs();
	ins.resize(ni);

	uint32_t no = plugin->count_outputs();
	outs.resize(no);

	buffers.resize(buffer_size * (ni + no));

	uint32_t i, k = 0;

	for (i = 0; i < ni; ++i, k += buffer_size)
	{
		ins[i] = &buffers[k];
	}

	for (i = 0; i < no; ++i, k += buffer_size)
	{
		outs[i] = &buffers[k];
	}
}	

void trackitem::process(uint32_t nframes)
{
	plugin->process(nframes, ins.data(), outs.data());
}

void trackitem::copy(uint32_t nframes, uint32_t index, float *buffer)
{
	std::copy(outs[index], outs[index] + nframes, buffer);
}







uint32_t track_engine::max_effects()
{
	return m_effects.size();
}


track_engine::track_engine()
{
	m_effects.reserve(8);
}

void track_engine::set_synth(plum::iplugin *s)
{
	auto old = m_synth;

	trackitem *ti = nullptr;

	if (s)
	{
		ti = new trackitem(s, m_buffersize);
	}

	m_sync.lock();
	m_synth = ti;
	m_sync.unlock();

	if (old)
	{
		delete old;
	}
}



void track_engine::set_effect(plum::iplugin *e, uint32_t index)
{
	auto old = m_effects[index];

	trackitem *ti = nullptr;

	if (e)
	{
		ti = new trackitem(e, m_buffersize);
	}

	m_sync.lock();
	m_effects[index] = ti;
	m_sync.unlock();

	if (old)
	{
		delete old;
	}
}

void track_engine::reset(uint32_t buffersize, uint32_t samplerate)
{
	m_buffersize = buffersize;
	m_samplerate = samplerate;
}

void track_engine::midi(uint8_t *e)
{
	if (m_synth)
	{
		m_synth->plugin->midi_event(e);
	}
}

void track_engine::process(uint32_t nframes, float **ins, float **outs)
{
	int source = -2;
	float **target;
//	uint32_t count;

	m_sync.lock();

	if (m_synth != nullptr)
	{
		m_synth->process(nframes);
		source = -1;
	}
	
	for (size_t i = 0; i < m_effects.size(); ++i)	
	{
		if (m_effects[i] == nullptr) continue;

		target = m_effects[i]->ins.data();
//		count =  m_effects[i]->ins.size();

		if (source == -2)
		{
			std::fill(target[0], target[0] + nframes, 0);
			std::fill(target[1], target[1] + nframes, 0);
		}
		else if (source == -1)
		{
			m_synth->copy(nframes, 0, target[0]);
			m_synth->copy(nframes, 1, target[1]);
		}
		else 
		{
			m_effects[source]->copy(nframes, 0, target[0]);
			m_effects[source]->copy(nframes, 1, target[1]);
		}
		
		m_effects[i]->process(nframes);
		source = i;
	}

	if (source == -2)
	{
		std::fill(outs[0], outs[0] + nframes, 0);
		std::fill(outs[1], outs[1] + nframes, 0);
	}
	else if (source == -1)
	{
		m_synth->copy(nframes, 0, outs[0]);
		m_synth->copy(nframes, 1, outs[1]);
	}
	else 
	{
		m_effects[source]->copy(nframes, 0, outs[0]);
		m_effects[source]->copy(nframes, 1, outs[1]);
	}


	m_sync.unlock();
}
