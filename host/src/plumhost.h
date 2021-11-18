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

#include "plugincatalog.h"
#include "pluginview.h"
#include "controller.h"
#include "audio.h"
#include "engine.h"

#define APP_TITLE "Plum host 1.0"

class ModelColumns : public Gtk::TreeModel::ColumnRecord
{
public:
    ModelColumns()
    { 
		add(text); 
	}

    Gtk::TreeModelColumn<Glib::ustring> text;
};


class plumhost : public Gtk::Window, public plum::ihost
{
	enum controller_type {controller_none, controller_basic, controller_custom};
	controller_type m_current_controller {controller_none};

	audio m_audio;
	track_engine m_engine;

	plugincatalog m_catalog;
	pluginview m_pluginview;
	controller m_controller;

	Glib::RefPtr<Gtk::TreeView> m_tv;
	Glib::RefPtr<Gtk::TreeStore> m_ts;
	ModelColumns m_columns;

	Glib::RefPtr<Gtk::ListBox> m_track;
	Glib::RefPtr<Gtk::ScrolledWindow> m_scroller;
	Glib::RefPtr<Gtk::ComboBoxText> m_presets;	
	sigc::connection m_presets_sig;

	void init_treeview();
	void init_track();
	bool on_exit(GdkEventAny* event);

	void on_tbLibrary();
	void on_tbPlug();
	void on_tbUnplug();
	void on_plugin_selected(Gtk::ListBoxRow *);
	void on_preset_selected();

	void on_storage(bool save, bool bank);
	void on_save_preset();
	void on_save_bank();
	void on_load_preset();
	void on_load_bank();

	void plugin_preset_selected(plum::iplugin *) override;
	void plugin_bank_changed(plum::iplugin *) override;
	void plugin_preset_changed(plum::iplugin *, uint32_t index) override;

	void plugin_load_preset(plum::iplugin *) override;
	void plugin_save_preset(plum::iplugin *) override;
	void plugin_load_bank(plum::iplugin *) override;
	void plugin_save_bank(plum::iplugin *) override;


	void open_library(std::string);
	void close_library();
	void plug(std::string name, bool is_synth);
	void unplug(Gtk::ListBoxRow *);
	void clear_track();

	plum::iplugin *get_selected_plugin();

	void openview(Gtk::ListBoxRow *);
	void closeview();

	void display_selected_preset();
	void display_preset_list(plum::iplugin *);
	void display_parameters();



	// IHOST
	void reference() override;
	void release() override;
	void *as(const char *ifid) override;
	
public:

	plumhost();
	~plumhost();


};
