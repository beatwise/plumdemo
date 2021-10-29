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

#include "controller.h"


class param_context_float : public param_context
{
	Gtk::Scale *slider;

	void value_changed()
	{
		if (sig_on)
		{
			float v = slider->get_value();
			plugin->set_parameter(index, v);
		}

		char str[32];
		def.format((plum_param_def *)&def, str, 32, plugin->get_parameter(index));
		display->set_label(str); 
	}

public:

	param_context_float(
			uint32_t index, 
			plum::iplugin *plugin, 
			plum_param_def def, 
			Gtk::Label *display, 
			Gtk::Scale *scale)
		: param_context(index, plugin, def, display), slider(scale)
	{
		slider->set_round_digits(4);
		slider->set_size_request(400, 20);
		slider->set_draw_value(false);
		slider->set_range(def.min, def.max);

		slider->signal_value_changed().connect(
			sigc::mem_fun(this, &param_context_float::value_changed));
	}

	void refresh() override
	{
		slider->set_value(plugin->get_parameter(index));
	}

};

class param_context_integer : public param_context
{
	Gtk::ComboBoxText *combo;


	void value_changed()
	{
		if (sig_on)
		{
			uint32_t n = combo->get_active_row_number();
			plugin->set_parameter(index,  n + def.min);
		}

		char str[32];
		def.format((plum_param_def *)&def, str, 32, plugin->get_parameter(index));
		display->set_label(str); 
	}


public:
	param_context_integer(
			uint32_t index, 
			plum::iplugin *plugin, 
			plum_param_def def, 
			Gtk::Label *display, 
			Gtk::ComboBoxText *cbo)
		: param_context(index, plugin, def, display), combo(cbo)
	{

		combo->set_size_request(300, 20);

		for (float i = def.min; i <= def.max; i++)
		{
			char str[32];
			def.format((plum_param_def *)&def, str, 32, i);
			combo->append(str);
		}

		combo->signal_changed().connect(
			sigc::mem_fun(this, &param_context_integer::value_changed));
	}

	void refresh() override
	{
		int n = int(plugin->get_parameter(index) - def.min);
		combo->set_active(n);
	}
	
};




void controller::set_plugin(plum::iplugin *plugin)
{
	if (m_plugin)
	{
		m_plugin->release();
	}

	m_plugin = plugin;
	if (m_plugin)
	{
		m_plugin->reference();
		create();
	}
}

void controller::create()
{
	set_margin_left(6);
	set_margin_top(6);


	m_params.clear();
	for (auto c : get_children()) remove(*c);

	uint32_t np = m_plugin->count_parameters();
	m_params.resize(np);
	

	int y = 0;

	auto wtitle = Gtk::manage(new Gtk::Label());
	wtitle->set_margin_bottom(24);
	wtitle->set_halign(Gtk::Align::ALIGN_START);
	attach(*wtitle, 1, y, 2, 1);
	wtitle->show();


	auto title = m_plugin->get_name();
	wtitle->set_label(title);


	++y;

	for (uint32_t i = 0; i < np; ++i)
	{
		plum_param_def def;
		m_plugin->get_parameter_def(i, &def);

		auto wname = Gtk::manage(new Gtk::Label());
		wname->set_label(def.name);
		wname->set_halign(Gtk::Align::ALIGN_START) ;
		attach(*wname, 1, y, 1, 1);
		wname->show();

		auto wvalue = Gtk::manage(new Gtk::Label());

		char str[32];
		def.format((plum_param_def *)&def, str, 32, m_plugin->get_parameter(i));
		wvalue->set_label(str);


		wvalue->set_size_request(100, 20);
		attach(*wvalue, 2, y, 1, 2);
		wvalue->show();

		if (def.type == PLUM_FLOAT)
		{
			auto wslider = Gtk::manage(new Gtk::Scale());
			attach(*wslider, 1, y + 1, 1, 1);
			wslider->show();

			auto ctx = std::unique_ptr<param_context_float> 
				(new param_context_float(i, m_plugin, def, wvalue, wslider));

			m_params[i] = std::move(ctx);
		}
		else
		{
			auto wcombo = Gtk::manage(new Gtk::ComboBoxText());
			attach(*wcombo, 1, y + 1, 1, 1);
			wcombo->show();

			auto ctx = std::unique_ptr<param_context_integer> 
				(new param_context_integer(i, m_plugin, def, wvalue, wcombo));

			m_params[i] = std::move(ctx);
		}

		y += 2;
	}

	refresh();
}

void controller::refresh()
{
	for (auto &x : m_params)
	{
		x->signal(false);
		x->refresh();
		x->signal(true);
	}
}

