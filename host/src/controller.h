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

#include <vector>
#include <memory>

#include <gtkmm.h>

#include "plum.h"


class param_context
{
protected:
	uint32_t index;
	plum_param_def def;
	Gtk::Label *display;
	plum::iplugin *plugin;

	bool sig_on {true};

	param_context(uint32_t index, plum::iplugin *plugin, plum_param_def def, Gtk::Label *display)
	{
		this->index = index;
		this->plugin = plugin;
		this->def = def;
		this->display = display;
	}

public:
	void signal(bool on)
	{
		sig_on = on;
	}

	virtual void refresh() = 0;
};


class controller : public Gtk::Grid
{
	plum::iplugin *m_plugin {nullptr};
	std::vector<std::unique_ptr<param_context>> m_params;

public:

	void set_plugin(plum::iplugin *plugin);
	void create();
	void refresh();
};
