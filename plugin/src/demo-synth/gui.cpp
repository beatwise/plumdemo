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

#include "synth.h"

namespace demo {


#define CVTIN(v, d) ((v - def.min) / (def.max - def.min))
#define CVTOUT(v, d) (def.min + v * (def.max - def.min))


DSynthGui::DSynthGui(plum::ihostwindow *hostwindow, DSynth *plugin) 
	: abcdwindow(hostwindow)
	, m_plugin(plugin)
{
	printf("NEW demo::DSynthGui\n");

	m_size = {400, 340};
	on_data_changed();
}

void DSynthGui::on_data_changed()
{
	m_names.clear();
	int n = sizeof(m_plugin->m_bank) / sizeof(preset_t);
	for (int i = 0; i < n; ++i)
	{
		m_names.push_back(m_plugin->m_bank[i].name);
	}
}

DSynthGui::~DSynthGui() 
{
	printf("DEL demo::DSynthGui\n");
}

void DSynthGui::close()
{
	if (m_plugin)
	{
		m_plugin->on_gui_closed();
		m_plugin = nullptr;
	}

	abcdwindow::close();
}


class framerect
{
	int m_margin;	
	abcd::rect m_rect;

public:

	framerect(int margin) : m_margin(margin) {}

	void update(const abcd::rect &r, bool begin = false)
	{
		if (begin) 
		{
			m_rect = r;
		}
		else
		{
			m_rect.x1 = std::min(m_rect.x1, r.x1);
			m_rect.y1 = std::min(m_rect.y1, r.y1);
			m_rect.x2 = std::max(m_rect.x2, r.x2);
			m_rect.y2 = std::max(m_rect.y2, r.y2);
		}
	}

	abcd::rect get_rect()
	{
		return {
			m_rect.x1 - m_margin, m_rect.y1 - m_margin, 
			m_rect.x2 + m_margin, m_rect.y2 + m_margin, 
		};
	}

};


void DSynthGui::do_gui(abcd::Draw &draw, abcd::rect frame)
{
	plum_param_def def;
	float v;
	char s[32];

	draw.set_solid_paint(m_win.m_theme.bg());
	draw.clear();

	// TITLE
	abcd::rect title {2, 2, m_size.width - 2, 30};
	draw.set_solid_paint(m_win.m_theme.fore());
	draw.fill_rounded_rectangle(title, 3, 3);

	draw.set_solid_paint(m_win.m_theme.text());
	draw.set_font(m_win.m_theme.font_family(), 22);
	draw.draw_textline("Demo-Synth", {title.x1 + 4, title.y1});
	

	framerect f(6);



	int w = 48;
	float mx = 48;

	abcd::rect r, rk, rl;
	abcd::guide gx1(mx);
	abcd::guide gx2(mx + (w + 6));
	abcd::guide gx3(mx + (w + 6) * 2);
	abcd::guide gx4(mx + (w + 6) * 3);


	abcd::guide gy9 (320);


	bool changed;

	// ADSR

	rl = {0, 0, w, 16};
	rk = {0, 0, w, w};

	int env_y = 220;
	abcd::guide gy_lenv(env_y);
	abcd::guide gy_kenv(env_y + rl.height() + 6);

	gy_lenv.top(rl);
	gy_kenv.top(rk);

	// PARAM: ATTACK

	m_plugin->get_parameter_def(dsynth_param_id::attack, &def);
	v = m_plugin->get_parameter(dsynth_param_id::attack);
	def.format(&def, s, 32, v);

	v = CVTIN(v, def);

	gx1.xcenter(rl);
	label(&m_win, &l_attack, rl, s, 0, 0);
	gx1.xcenter(rk);
	changed = knob(&m_win, &k_attack, rk, &v);
	if (changed)
	{
		v = CVTOUT(v, def);
		m_plugin->set_parameter(dsynth_param_id::attack, v);
	}

	f.update(rl, true);
	f.update(rk);

	// PARAM: DECAY

	m_plugin->get_parameter_def(dsynth_param_id::decay, &def);
	v = m_plugin->get_parameter(dsynth_param_id::decay);
	def.format(&def, s, 32, v);

	v = CVTIN(v, def);

	gx2.xcenter(rl);
	label(&m_win, &l_decay, rl, s, 0, 0);
	gx2.xcenter(rk);
	changed = knob(&m_win, &k_decay, rk, &v);
	if (changed)
	{
		v = CVTOUT(v, def);
		m_plugin->set_parameter(dsynth_param_id::decay, v);
	}

	f.update(rl);
	f.update(rk);

	// PARAM: SUSTAIN

	m_plugin->get_parameter_def(dsynth_param_id::sustain, &def);
	v = m_plugin->get_parameter(dsynth_param_id::sustain);
	def.format(&def, s, 32, v);

	v = CVTIN(v, def);

	gx3.xcenter(rl);
	label(&m_win, &l_sustain, rl, s, 0, 0);
	gx3.xcenter(rk);
	changed = knob(&m_win, &k_sustain, rk, &v);
	if (changed)
	{
		v = CVTOUT(v, def);
		m_plugin->set_parameter(dsynth_param_id::sustain, v);
	}

	f.update(rl);
	f.update(rk);

	// PARAM: RELEASE

	m_plugin->get_parameter_def(dsynth_param_id::release, &def);
	v = m_plugin->get_parameter(dsynth_param_id::release);
	def.format(&def, s, 32, v);

	v = CVTIN(v, def);

	gx4.xcenter(rl);
	label(&m_win, &l_release, rl, s, 0, 0);
	gx4.xcenter(rk);
	changed = knob(&m_win, &k_release, rk, &v);
	if (changed)
	{
		v = CVTOUT(v, def);
		m_plugin->set_parameter(dsynth_param_id::release, v);
	}


	f.update(rl);
	f.update(rk);

	// FRAME
	draw.set_stroke_width(1);

	r = f.get_rect();
	draw.set_solid_paint(m_win.m_theme.fore());
	draw.stroke_rounded_rectangle(r, 6, 6);

	draw.set_font(m_win.m_theme.font_family(), 22);
	draw.draw_textline("ADSR", {r.x1, int(r.y1 - draw.get_font_height())});

	// OSC
	abcd::rect rr = {0, 0, 18, 18};
	abcd::rect rrl = {0, 0, 32, 24};

	abcd::guide gx_osc;
	abcd::guide gy_squ(120);
	abcd::guide gy_saw(gy_squ.position() + 8 + rr.height());


	gx_osc.move(gx1.position() + rr.width() / 2 + 12);
	gx_osc.left(rrl);

	// PARAM: OSCTYPE

	int vi = int(round(m_plugin->get_parameter(dsynth_param_id::osctype)));

	gx1.xcenter(rr);

	gy_squ.ycenter(rr);
	changed = abcd::radiobutton(&m_win, &r_squ, rr, 0, &vi);
	gy_squ.ycenter(rrl);
	label(&m_win, &l_squ, rrl, "Squ", -1, 0);
	if (changed && vi == 0)
	{
		m_plugin->set_parameter(dsynth_param_id::osctype, 0);

	}

	f.update(rr, true);
	f.update(rrl);

	gy_saw.ycenter(rr);
	changed = abcd::radiobutton(&m_win, &r_saw, rr, 1, &vi);
	gy_saw.ycenter(rrl);
	label(&m_win, &l_saw, rrl, "Saw", -1, 0);
	if (changed && vi == 1)
	{
		m_plugin->set_parameter(dsynth_param_id::osctype, 1);
	}

	f.update(rr, true);
	f.update(rrl);

	// PARAM: PWM

	m_plugin->get_parameter_def(dsynth_param_id::pwm, &def);
	v = m_plugin->get_parameter(dsynth_param_id::pwm);
	def.format(&def, s, 32, v);

	v = CVTIN(v, def);

	abcd::guide gy_pwm((gy_squ.position() + gy_saw.position()) / 2);
	gy_pwm.ycenter(rk);
	gx3.xcenter(rk);
	changed = knob(&m_win, &k_pwm, rk, &v);
	if (changed)
	{
		v = CVTOUT(v, def);
		m_plugin->set_parameter(dsynth_param_id::pwm, v);
	}

	f.update(rk);



	gx4.xcenter(rrl);
	gy_squ.ycenter(rrl);
	label(&m_win, &l_pwm, rrl, "PWM", 0, 0);

	f.update(rrl);

	gy_saw.ycenter(rrl);
	label(&m_win, &l_vpwm, rrl, s, 0, 0);

	f.update(rrl);



	// FRAME
	draw.set_stroke_width(1);

	r = f.get_rect();
	draw.set_solid_paint(m_win.m_theme.fore());
	draw.stroke_rounded_rectangle(r, 6, 6);

	draw.set_font(m_win.m_theme.font_family(), 22);
	draw.draw_textline("OSC", {r.x1, int(r.y1 - draw.get_font_height())});

	// NAMES

	abcd::rect rli = {0, 0, 100, 100};
	move(rli, 0, r.y1);
	abcd::guide gx5(320);
	gx5.xcenter(rli);

	int n = m_plugin->get_selected_preset();
	changed = abcd::list(&m_win, &li_names, rli, m_names, n);
	if (changed)
	{
		m_plugin->set_selected_preset(n);
	}

	if (m_renaming)
	{
		abcd::rect rte = {0, 0, 100, 16};
		move(rte, rli.x1, rli.y2 + 6);
		gx5.xcenter(rte);

		changed = input(&m_win, &i_name, rte, m_name);
		if (changed)
		{
			m_names[n] = m_name;
			m_plugin->set_preset_name(n, new plum::string(m_name.c_str()));
			m_renaming = false;
		}

	}
	else
	{
		abcd::rect rte = {0, 0, 100, 32};
		move(rte, rli.x1, rli.y2 + 6);

		m_name = m_names[n];

		changed = button(&m_win, &b_rename, rte, "rename");
		if (changed)
		{
			m_renaming = true;	
		}
	}



}

void DSynthGui::refresh()
{
	m_hostwindow->on_plugin_repaint();
}






} // demo