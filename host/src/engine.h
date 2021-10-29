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
#include <array>
#include <vector>

#include "plum.h"
#include "audio.h"

class spinlock
{
	std::atomic_flag af = ATOMIC_FLAG_INIT;

public:
	void lock() 	{ while(af.test_and_set()); }
	void unlock() 	{ af.clear(); }
};

struct trackitem
{
	uint32_t buffersize {0};
	std::vector<float *> ins;
	std::vector<float *> outs;
	std::vector<float> buffers;
	plum::iplugin *plugin {nullptr};
	
	trackitem(plum::iplugin *p, uint32_t buffer_size);
	~trackitem();
	void allocate_io(uint32_t buffer_size);
	void process(uint32_t nframes);
	void copy(uint32_t nframes, uint32_t index, float *buffer);
};



class track_engine : public engine
{
public:
	track_engine();

	void reset(uint32_t buffersize, uint32_t samplerate) override;
	void midi(uint8_t *e) override;
	void process(uint32_t nframes, float **ins, float **outs) override;

	void set_synth(plum::iplugin *);
	void set_effect(plum::iplugin *, uint32_t index);

	uint32_t max_effects();

private:
	uint32_t m_buffersize;
	uint32_t m_samplerate;

	trackitem *m_synth {nullptr};
	std::vector<trackitem *> m_effects {nullptr, nullptr, nullptr, nullptr};
	spinlock m_sync;
};
