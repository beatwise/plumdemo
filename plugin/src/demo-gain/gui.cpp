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



GainGui::GainGui(plum::ihostwindow *hostwindow, Gain *plugin) 
	: abcdwindow(hostwindow)
	, m_plugin(plugin)
{
	printf("NEW demo::GainGui\n");

	gain = m_plugin->get_parameter(0);

	m_hostwindow->add_timer(&m_timer, 50);
}

GainGui::~GainGui() 
{
	printf("DEL demo::GainGui\n");
}

void GainGui::close()
{
	m_hostwindow->remove_timer(&m_timer);

	if (m_plugin)
	{
		m_plugin->on_gui_closed();
		m_plugin = nullptr;
	}

	abcdwindow::close();
}

void GainGui::on_timer(void *id)
{
	m_hostwindow->on_plugin_repaint();
}

void GainGui::do_gui(abcd::Draw &draw, abcd::rect frame)
{
	draw.set_solid_paint(m_win.m_theme.bg());
	draw.clear();

	// TITLE
	abcd::rect title {2, 2, m_size.width - 2, 30};
	draw.set_solid_paint(m_win.m_theme.fore());
	draw.fill_rounded_rectangle(title, 3, 3);

	draw.set_solid_paint(m_win.m_theme.text());
	draw.set_font(m_win.m_theme.font_family(), 22);
	draw.draw_textline("Demo-Gain", {title.x1 + 4, title.y1});


	char s[32];

	abcd::rect r;
	abcd::guide gx(m_size.width / 2);
	abcd::guide gy(48);

	plum_param_def def;
	m_plugin->get_parameter_def(0, &def);
	float v = m_plugin->get_parameter(0);
	def.format((plum_param_def*)&def, s, 32, v);

	r = {0, 0, 48, 16};
	gx.xcenter(r);
	gy.top(r);
	label(&m_win, &l_gain, r, s, 0, 0);

	r = {0, 0, 32, 96};
	gx.xcenter(r);
	gy.shift(32);
	gy.top(r);

	gain = m_plugin->get_parameter(0) / -60.f;
	if (slider(&m_win, &sl_gain, r, 16, &gain, false))
	{
		m_plugin->set_parameter(0, gain * -60.f);
	}


	// OUTPUT LEVELS
	float dbl = 20 * log10(m_plugin->m_peakl.load());
	float dbr = 20 * log10(m_plugin->m_peakr.load());

	abcd::rect rlev = {0, 0, 120, 16};
	snprintf(s, 32, "Out levels: %3.0f %3.0f", dbl, dbr);
	move(rlev, 0, r.y2 + 4);

	gx.xcenter(rlev);
	label(&m_win, &l_outs, rlev, s, 0, 0);
	

}


} // demo

