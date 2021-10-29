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


#include <unordered_map>

#include <gtkmm.h>

#include "plum.h"

class pluginview : public Gtk::DrawingArea, public plum::ihostwindow 
{
public:

	// IOBJECT
	void reference() override {}
	void release() override {}
	void *as(const char *ifid) override
	{
		if (std::string(ifid) == IFID_PLUM_OBJECT)
		{
			reference(); return static_cast<Gtk::DrawingArea *>(this);
		}
		else if (std::string(ifid) == IFID_PLUM_HOSTWINDOW)
		{
			reference(); return static_cast<plum::ihostwindow *>(this);
		}

		return nullptr;
	}



	void on_plugin_repaint() override;
	void on_plugin_preferred_size(plum_size size) override;
	plum_size get_size() override;
	void on_size_allocate(Gtk::Allocation& a) override;
	bool on_draw(const ::Cairo::RefPtr<::Cairo::Context>& cr) override;
	bool on_leave_notify_event(GdkEventCrossing* crossing_event);
	bool on_button_press_event(GdkEventButton *e) override;
	bool on_button_release_event(GdkEventButton *e) override;
	bool on_motion_notify_event(GdkEventMotion *e) override;
	bool on_key_press_event(GdkEventKey *e) override;
	bool on_key_release_event(GdkEventKey *e) override;
public:

	pluginview();
	plum::iwindow * get_plugin_window();
	void set_plugin_window(plum::iwindow *plugin_window);
	void get_clipboard_text() override;
	void set_clipboard_text(plum::istring *str) override;
	void on_clipboard_text(const std::string& s);
	void add_timer(void *id, int period) override;
	void remove_timer(void *id) override;
	bool on_timer(void *id);

private:

	uint32_t map_mouse_button(int button);
	uint32_t map_modifier(int state);
	uint32_t map_key(int code);

	plum::iwindow *m_plugin_window {nullptr};
	bool m_mouse_grabbed {false};
	bool m_mouse_in {false};

	::Cairo::RefPtr<::Cairo::ImageSurface> m_surface;

	std::unordered_map<void *, sigc::connection> m_timers;

};

