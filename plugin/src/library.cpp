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

#include "plum.h"
#include "plumhelpers.h"

#include "demo-gain/gain.h"
#include "demo-synth/synth.h"

static std::vector<const char *> g_synths = {"DSynth", "DSynthNoGui"};
static std::vector<const char *> g_effects = {"demoGain"};

void plum_begin()
{
	printf("demo plum plugin: %s\n", __func__);
}

void plum_end()
{
	printf("demo plum plugin: %s\n", __func__);
}

plum_version plum_get_version()
{
	return {1, 0};
}


const char *plum_catalog(int category, uint32_t index)
{
	if (category == PLUM_SYNTH)
	{
		if (index < g_synths.size()) return g_synths[index];
	}
	else if (category == PLUM_EFFECT)
	{
		if (index < g_effects.size()) return g_effects[index];
	}

	return nullptr;
}

const char* plum_plugin_info(const char *name)
{
	std::string s(name);

	if (s == "demoGain")
	{
		return "demoGain - demo effect";
	}
	else if (s == "DSynthNoGui")
	{
		return "DSynthNoGui - demo synth without gui";
	}
	else if (s == "DSynth")
	{
		return "DSynth - demo synth";
	}

	return nullptr;
}

plum::iplugin* plum_create_plugin(const char *name, plum::ihost *host)
{
	if (host == nullptr)
	{
		//TODO: print warning
	}

	std::string s(name);

	if (s == "demoGain")
	{
		return new demo::Gain(host);
	}
	else if (s == "DSynthNoGui")
	{
		return new demo::DSynth(host, true);
	}
	else if (s == "DSynth")
	{
		return new demo::DSynth(host);
	}

	return nullptr;
}
  

