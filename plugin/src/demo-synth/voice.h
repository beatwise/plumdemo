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

#include <atomic>
#include "plum.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wsign-compare"
#include "Tonic.h"
#pragma GCC diagnostic pop

namespace demo {



enum dsynth_param_id
{
	osctype = 0,
	pwm,
	attack,
	decay,
	sustain,
	release,
	param_count
};


struct preset_t
{
	plum_param_def *defs;
	std::atomic<float> data[param_count];
	std::string name;

	void define(plum_param_def *x, const float (&y)[param_count], std::string z)
	{
		defs = x;
		name = z;
		for (int i = 0; i < param_count; ++i) data[i] = y[i];
	}

	float get(uint32_t index)
	{
		return data[index];
	}

	void set(uint32_t index, float v)
	{
		plum_param_def *p = &defs[index];
		v = std::min(p->max, v);
		v = std::max(p->min, v);				
		data[index] = v;
	}

	void clone(preset_t *np)
	{
		np->defs = defs;
		np->name = name;
		for (int i = 0; i < param_count; ++i) np->data[i] = data[i].load();
	}
};



class voice : public Tonic::Synth
{
public:
	
	voice()
	{
		Tonic::ADSR adsr;
		Tonic::SawtoothWaveBL osc_saw;
		Tonic::RectWaveBL osc_squ;

		adsr.exponential(true);
		adsr.legato(false);


		auto gate = addParameter("gate", 0);
		auto freq = addParameter("freq", 1);
		auto gain = addParameter("gain", 0.5);

		auto en_squ = addParameter("en_squ", 0);
		auto en_saw = addParameter("en_saw", 1);

		auto squ_pwm = addParameter("squ_pwm", 0.5).smoothed();

		auto attack = addParameter("attack", 0.25);
		auto decay = addParameter("decay", 0.25);
		auto sustain = addParameter("sustain", 0.5);
		auto release = addParameter("release", 0.25);

		osc_squ.freq(freq);
		osc_saw.freq(freq);

		osc_squ.pwm(squ_pwm);


		adsr.attack(attack);
		adsr.decay(decay);
		adsr.sustain(sustain);
		adsr.release(release);

		adsr.trigger(gate);

		auto osc = osc_squ * en_squ + osc_saw * en_saw;

		auto x = (osc * gain) * adsr;

		setOutputGen(x);
	}

	void update_params(bool init = false)
	{
		float osctype = round(m_preset->load()->get(dsynth_param_id::osctype));

		if (osctype == 0)
		{
			setParameter("en_squ", 1);
			setParameter("en_saw", 0);
		}
		else
		{
			setParameter("en_squ", 0);
			setParameter("en_saw", 1);
		}

		setParameter("squ_pwm", m_preset->load()->get(pwm));

		if (init)
		{
			setParameter("attack", m_preset->load()->get(attack));
			setParameter("decay", m_preset->load()->get(decay));
			setParameter("sustain", m_preset->load()->get(sustain));
			setParameter("release", m_preset->load()->get(dsynth_param_id::release));
		}
	}


	void start(int note, int, float gain, std::atomic<preset_t *> *p) 
	{
		m_preset = p;
		update_params(true);

		m_note = note;
		setParameter("freq", 440.0 * std::pow(2.0, (note - 69.0) / 12.0));
		setParameter("gate", 1);
		setParameter("gain", gain);
		m_held = true;
	}

	void release(int velocity) 
	{		
		setParameter("gate", 0);
		m_held = false;
	}

	bool is_held()
	{
		return m_held;
	}

	bool is_free()
	{
		return m_level < 0.000001 && !m_held;
	}

	int midi_note() 
	{
		return m_note;
	}

	void process(float **outs, int nframes)
	{
		update_params();

		fillBufferOfFloats(outs[0], nframes, 1);
		std::copy(outs[0], outs[0] + nframes, outs[1]);

		m_level = 0;
		for (int i = 0; i < nframes; ++i)	
		{
			m_level += fabs(outs[0][i]);
		}

		m_level = m_level / nframes;		
	}

private:
	int m_note;
	bool m_held {false};
	float m_level {0};

	std::atomic<preset_t *> *m_preset {nullptr};

};



} // demo
