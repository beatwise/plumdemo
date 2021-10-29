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

#include "audio.h"

int _process(jack_nframes_t nframes, void *arg)
{
	auto _this = static_cast<audio *>(arg);
	return _this->process(nframes);
}

audio::audio()
{
}

uint32_t audio::samplerate()
{
	return jack_get_sample_rate(m_jc);
}

uint32_t audio::buffersize()
{
	return jack_get_buffer_size(m_jc);
}


void audio::connect_ports()
{
	const char **ports = nullptr;

	ports = jack_get_ports(m_jc, nullptr, 
		JACK_DEFAULT_AUDIO_TYPE, 
		JackPortIsPhysical | JackPortIsInput);


	if (ports)
	{
		int i = 0;
		if (ports[i])
		{
			jack_connect(m_jc, jack_port_name(m_audio_out_port[0]), ports[i]);
		}

		++i;

		if (ports[i])
		{
			jack_connect(m_jc, jack_port_name(m_audio_out_port[1]), ports[i]);
		}

		jack_free(ports);
	}
}

bool audio::start(const char *id, engine *e)
{
	jack_options_t options = JackNullOption;
	jack_status_t status;

	//
	m_jc = jack_client_open(id, options, &status, nullptr);

	if (m_jc == nullptr)
	{
		return false;
	}

	//
	jack_set_process_callback(m_jc, _process, this);	

	//
	m_midi_in_port = jack_port_register(m_jc, "midi-in",
		 JACK_DEFAULT_MIDI_TYPE,
		 JackPortIsInput, 0);

	if (m_midi_in_port == nullptr)
	{
		printf("AUDIO ERROR: can't register midi-in\n");
	}

	//
	m_audio_out_port[0] = jack_port_register(m_jc, "audio-out-left",
		  JACK_DEFAULT_AUDIO_TYPE,
		  JackPortIsOutput, 0);

	if (m_audio_out_port[0] == nullptr)
	{
		printf("AUDIO ERROR: can't register audio-out-left\n");
	}

	//
	m_audio_out_port[1] = jack_port_register(m_jc, "audio-out-right",
		  JACK_DEFAULT_AUDIO_TYPE,
		  JackPortIsOutput, 0);

	if (m_audio_out_port[1] == nullptr)
	{
		printf("AUDIO ERROR: can't register audio-out-right\n");
	}


	m_engine = e;
	if (m_engine)
	{
		auto bs = jack_get_buffer_size(m_jc);
		auto sr = jack_get_sample_rate(m_jc);
		m_engine->reset(bs, sr);
	}

	//
	if (jack_activate(m_jc)) 
	{
		printf("AUDIO ERROR: cannot activate\n");

		jack_client_close(m_jc);
		m_jc = nullptr;
		return false;
	}


	connect_ports();

	return true;
}

void audio::stop()
{
	if (m_jc)
	{
		jack_client_close(m_jc);
		m_jc = nullptr;
		m_engine = nullptr;
	}
}

int audio::process(jack_nframes_t nframes)
{
	auto *out1 = (jack_default_audio_sample_t *)jack_port_get_buffer(m_audio_out_port[0], nframes);
	auto *out2 = (jack_default_audio_sample_t *)jack_port_get_buffer(m_audio_out_port[1], nframes);
	jack_default_audio_sample_t *outs[] = {out1, out2};

	void* midi_buf = jack_port_get_buffer(m_midi_in_port, nframes);

	jack_nframes_t count = jack_midi_get_event_count(midi_buf);
	jack_nframes_t index = 0;
	jack_nframes_t frame = 0;
	jack_nframes_t delta = 0;


	jack_midi_event_t e;

	while (nframes)
	{
		delta = nframes;

		while (index < count)
		{
			jack_midi_event_get(&e, midi_buf, index);

			if (e.time == frame)
			{
				if (m_engine)
				{
					m_engine->midi(e.buffer);
				}

				++index;
			}
			else
			{
				delta = e.time - frame;
				frame = e.time;

				break;
			}
		}

		if (m_engine)
		{

			m_engine->process(delta, nullptr, outs);
		}

		nframes -= delta;
		outs[0] += delta;
		outs[1] += delta;
	}

	return 0;
}

