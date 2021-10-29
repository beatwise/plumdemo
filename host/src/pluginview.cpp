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

#include "pluginview.h"

#include "plumhelpers.h"


void pluginview::on_plugin_repaint()
{
	queue_draw();
}

void pluginview::on_plugin_preferred_size(plum_size size)
{
	set_size_request(size.width, size.height);	
}

plum_size pluginview::get_size()
{
	auto a = get_allocation();
	return {a.get_width(), a.get_height()};
}

void pluginview::on_size_allocate(Gtk::Allocation& a)
{
	Gtk::DrawingArea::on_size_allocate(a);
	m_surface = ::Cairo::ImageSurface::create(::Cairo::Format::FORMAT_ARGB32, a.get_width(), a.get_height());

	if (m_plugin_window)
	{
		m_plugin_window->on_size({a.get_width(), a.get_height()});
	}
}

bool pluginview::on_draw(const ::Cairo::RefPtr<::Cairo::Context>& cr)
{
	auto cr2 = ::Cairo::Context::create(m_surface);
	cr2->set_source_rgb(1, 1, 1);
	cr2->paint();


	if (m_plugin_window)
	{
		m_plugin_window->render(m_surface->get_data());
	}

	cr->set_source(m_surface, 0, 0);
	cr->paint();

	return true;
}


bool pluginview::on_leave_notify_event(GdkEventCrossing* crossing_event)
{
	if (m_mouse_in == true)
	{
		m_mouse_in = false;
		m_plugin_window->on_mouse_out();
	}

	return false;
}

bool pluginview::on_button_press_event(GdkEventButton *e)
{
	grab_focus();

	if (m_plugin_window)
	{
		m_mouse_grabbed = true;
		float x = e->x;
		float y = e->y;
		uint32_t b = map_mouse_button(e->button);
		m_plugin_window->on_mouse_down(x, y, b);
	}

	return false;
}
bool pluginview::on_button_release_event(GdkEventButton *e)
{
	if (m_plugin_window)
	{
		if (m_mouse_grabbed)
		{
			m_mouse_grabbed = false;
			float x = e->x;
			float y = e->y;
			uint32_t b = map_mouse_button(e->button);
			m_plugin_window->on_mouse_up(x, y, b);
		}
	}

	return false;
}
bool pluginview::on_motion_notify_event(GdkEventMotion *e)
{
	if (m_plugin_window)
	{
		if (m_mouse_grabbed)
		{
			if (m_mouse_in == false)
			{
				m_mouse_in = true;
				m_plugin_window->on_mouse_in();
			}

			float x = e->x;
			float y = e->y;
			m_plugin_window->on_mouse_move(x, y);
		}
		else
		{
			if (m_mouse_in == true)
			{
				m_mouse_in = false;
				m_plugin_window->on_mouse_out();
			}
		}
	}

	return false;
}


bool pluginview::on_key_press_event(GdkEventKey *e)
{
	// TODO: uint32_t m = map_key(e->state);

	uint32_t u32 = gdk_keyval_to_unicode(e->keyval);


	if (u32 < 127 || u32 > 159)
	{
		std::string s32(6, 0);
		gint count = g_unichar_to_utf8 (u32, (gchar *)s32.c_str());

		if (count)
		{
			if (m_plugin_window)
				m_plugin_window->on_utf8_char(s32.c_str(), 0);
		}
	}

	return false;
}

bool pluginview::on_key_release_event(GdkEventKey *e)
{
	return false;
}

pluginview::pluginview()
{
	set_can_focus(true);
	add_events(
		  Gdk::POINTER_MOTION_MASK 
		| Gdk::BUTTON_PRESS_MASK
		| Gdk::BUTTON_RELEASE_MASK
		| Gdk::KEY_PRESS_MASK 
		| Gdk::KEY_RELEASE_MASK
		| Gdk::ENTER_NOTIFY_MASK 
		| Gdk::LEAVE_NOTIFY_MASK);
}

plum::iwindow * pluginview::get_plugin_window()
{
	return m_plugin_window;
}

void pluginview::set_plugin_window(plum::iwindow *plugin_window)
{
	if (m_plugin_window)
	{
		m_plugin_window->close();
		m_plugin_window->release();
	}

	m_plugin_window = plugin_window;

	if (m_plugin_window)
	{
		auto ws = m_plugin_window->get_preferred_size();
		set_size_request(ws.width, ws.height);
	}

	m_mouse_grabbed = false;
	m_mouse_in = false;
}

void pluginview::get_clipboard_text()
{
	Gtk::Clipboard::get()->request_text(sigc::mem_fun(this, &pluginview::on_clipboard_text));
}

void pluginview::set_clipboard_text(plum::istring *str)
{		
	Gtk::Clipboard::get()->set_text(str->text());
	str->release();
}

void pluginview::on_clipboard_text(const std::string& s)
{
	if (m_plugin_window)
	{
		auto str = new plum::string(s.c_str());
		m_plugin_window->on_paste_text(str);
	}
}


void pluginview::add_timer(void *id, int period)
{
	remove_timer(id);
	sigc::slot<bool()> slot = sigc::bind(sigc::mem_fun(*this, &pluginview::on_timer), id);
	m_timers[id] = Glib::signal_timeout().connect(slot, period);
}

void pluginview::remove_timer(void *id)
{
	auto p = m_timers.find(id);
	if (p != m_timers.end())
	{
		p->second.disconnect();
		m_timers.erase(p);
	}
}

bool pluginview::on_timer(void *id)
{
	if (!m_plugin_window)
	{
		return false;
	}

	m_plugin_window->on_timer(id);
	return true;
}



uint32_t pluginview::map_mouse_button(int button)
{
	if (button == 1) return plum_button_left;
	if (button == 2) return plum_button_middle;
	if (button == 3) return plum_button_right;
	return 0;
}

uint32_t pluginview::map_modifier(int state)
{
	uint32_t mod = 0;
	if (GDK_SHIFT_MASK & state) mod |= plum_mod_shift;
	if (GDK_CONTROL_MASK & state) mod |= plum_mod_control;
	return mod;
}

uint32_t pluginview::map_key(int code)
{
	switch (code)
	{
		case GDK_KEY_Return: return plum_key_enter;
		case GDK_KEY_Home: return plum_key_home;
		case GDK_KEY_End: return plum_key_end;
		case GDK_KEY_BackSpace: return plum_key_backspace;
		case GDK_KEY_Delete: return plum_key_delete;
		case GDK_KEY_Left: return plum_key_left;
		case GDK_KEY_Right: return plum_key_right;
		case GDK_KEY_Up: return plum_key_up;
		case GDK_KEY_Down: return plum_key_down;
		case GDK_KEY_Tab: return plum_key_tab;
		default: return plum_key_none;
	}
}

