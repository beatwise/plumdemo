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

#include "plum.h"
#include "abcdgui.h"

class abcdwindow : public plum::iwindow
{
public:
	PLUM_IOBJECT_RC_IMPL(m_rc, abcdwindow)

	void *as(const char *ifid)
	{
		if (std::string(ifid) == IFID_PLUM_OBJECT)
		{
			reference(); return static_cast<plum::iwindow *>(this);
		}
		else if (std::string(ifid) == IFID_PLUM_WINDOW)
		{
			reference(); return static_cast<plum::iwindow *>(this);
		}

		return nullptr;
	}

	virtual void close() override;
	void on_size(plum_size s) override;
	plum_size get_preferred_size() override;
	void render(uint8_t *pixel32) override;
	abcdwindow(plum::ihostwindow *hostwindow);
	virtual ~abcdwindow();
	void on_mouse_in() override;
	void on_mouse_out() override;
	void on_mouse_move(float x, float y) override;
	void on_mouse_down(float x, float y, uint32_t btn) override;
	void on_mouse_up(float x, float y, uint32_t btn) override;
	void on_utf8_char(const char *ch, uint32_t mod) override;
	void on_key_down(uint32_t code, uint32_t mod) override;
	void on_key_up(uint32_t code, uint32_t mod) override;


protected:

	virtual void do_gui(abcd::Draw &draw, abcd::rect frame) = 0;

	plum::ihostwindow *m_hostwindow {nullptr};
	plum_size m_size {240, 200};

	abcd::window m_win;
	abcd::panel_widget panel1;
};

