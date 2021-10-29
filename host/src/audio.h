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

#include <stdio.h>
#include <jack/jack.h>
#include <jack/midiport.h>

class engine
{
public:
	virtual void reset(uint32_t buffersize, uint32_t samplerate) = 0;
	virtual void midi(uint8_t *e) = 0;
	virtual void process(uint32_t nframes, float **ins, float **outs) = 0;
};


class audio
{
	friend int _process(jack_nframes_t nframes, void *arg);
	int process(jack_nframes_t nframes);
	void connect_ports();

public:
	audio();
	bool start(const char *id, engine *e);
	void stop();
	uint32_t samplerate();
	uint32_t buffersize();

private:
	jack_client_t *m_jc;
	jack_port_t *m_midi_in_port;
	jack_port_t *m_audio_out_port[2];

	engine *m_engine {nullptr};	
};
