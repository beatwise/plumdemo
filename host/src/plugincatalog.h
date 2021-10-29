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

#include <string>
#include <vector>

#include "plum.h"

#include "DyLib.hpp"

class plugincatalog
{
	DyLib m_loader;

	std::function<void()> m_fn_plum_begin;
	std::function<void()> m_fn_plum_end;

	std::function<plum_version()> m_fn_plum_get_version;
	std::function<const char *(int, uint32_t)> m_fn_plum_catalog;
	std::function<const char *(const char *name)> m_fn_plum_plugin_info;
	std::function<plum::iplugin*(const char *name, plum::ihost *)> m_fn_plum_create_plugin;

	plum_version m_version;
	std::vector<std::string> m_synths;
	std::vector<std::string> m_effects;

public:
	bool open(std::string path);
	void close();

	plum_version version();

	std::vector<std::string> synth_list();
	std::vector<std::string> effect_list();
	plum::iplugin *create_plugin(std::string name, plum::ihost *host);

};