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

#include "abcdwindow.h"


void abcdwindow::close()
{
	if (m_hostwindow)
	{
		m_hostwindow->release();
		m_hostwindow = nullptr;
	}
}

void abcdwindow::on_size(plum_size s)
{
	m_hostwindow->on_plugin_repaint();
}

plum_size abcdwindow::get_preferred_size()
{
	return m_size;
}

void abcdwindow::render(uint8_t *pixel32)
{
	auto winsize = m_hostwindow->get_size();
	abcd::Draw draw(pixel32, winsize.width, winsize.height);

	for (int i = 0; i < 2; ++i)
	{
		m_win.begin(&draw);

		draw.set_solid_paint({255, 255, 255, 255});
		draw.clear();

		draw.set_font(m_win.m_theme.font_family(), m_win.m_theme.font_size());



		int ox = 0, oy = 0;

		if (winsize.width > m_size.width)
		{
			ox = winsize.width / 2 - m_size.width / 2;
		}

		if (winsize.height > m_size.height)
		{
			oy = winsize.height / 2 - m_size.height / 2;
		}

		abcd::rect frame {ox, oy, ox + m_size.width, oy + m_size.height};

		frame = begin_panel(&m_win, &panel1, frame);

		do_gui(draw, frame);

		end_panel(&m_win, &panel1);

		m_win.end();

	}

}


abcdwindow::abcdwindow(plum::ihostwindow *hostwindow) 
	: m_hostwindow(hostwindow)
{
	m_win.m_theme.set_font("Roboto", 12);
}

abcdwindow::~abcdwindow() 
{
}

void abcdwindow::on_mouse_in()
{
	m_hostwindow->on_plugin_repaint();
}
void abcdwindow::on_mouse_out()
{
	m_hostwindow->on_plugin_repaint();
}


void abcdwindow::on_mouse_move(float x, float y)
{
	m_win.mouse_x = x;
	m_win.mouse_y = y;
	m_hostwindow->on_plugin_repaint();
}

void abcdwindow::on_mouse_down(float x, float y, uint32_t btn)
{
	m_win.mouse_x = x;
	m_win.mouse_y = y;
	m_win.mouse_button = btn;
	m_win.mouse_down = true;
	m_hostwindow->on_plugin_repaint();
}

void abcdwindow::on_mouse_up(float x, float y, uint32_t btn)
{
	m_win.mouse_x = x;
	m_win.mouse_y = y;
	m_win.mouse_button = btn;
	m_win.mouse_down = false;
	m_hostwindow->on_plugin_repaint();
}

void abcdwindow::on_utf8_char(const char *ch, uint32_t mod)
{
	m_win.key_down = true;
	m_win.key_utf8 = std::string(ch);
	m_hostwindow->on_plugin_repaint();
}
void abcdwindow::on_key_down(uint32_t code, uint32_t mod)
{
	m_hostwindow->on_plugin_repaint();
}
void abcdwindow::on_key_up(uint32_t code, uint32_t mod)
{
	m_hostwindow->on_plugin_repaint();
}




