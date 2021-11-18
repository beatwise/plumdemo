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

#include <fstream>
#include <gtkmm.h>

#include "plumhelpers.h"
#include "plumhost.h"

#include "glade.cpp"



std::string choose_file(Gtk::Window &window, std::string title, std::string extension, bool save)
{
	auto k = save ? Gtk::FILE_CHOOSER_ACTION_SAVE : Gtk::FILE_CHOOSER_ACTION_OPEN;

	Gtk::FileChooserDialog dialog(title, k);
	dialog.set_transient_for(window);

	dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
	dialog.add_button("Select", Gtk::RESPONSE_OK);

	auto filter = Gtk::FileFilter::create();
	filter->set_name("plum");
	filter->add_pattern(extension);
	dialog.add_filter(filter);


	int result = dialog.run();

	if (result == Gtk::RESPONSE_OK)
	{
		return dialog.get_filename();
	}
	
	return "";
}

class tracklabel : public Gtk::Label
{
	bool m_is_synth;
	plum::iplugin *m_plugin {nullptr};

public:
	tracklabel(bool bsynth) : m_is_synth(bsynth)
	{
		set_label(m_is_synth ? "<no synth>" : "<no effect>");
	}

	bool is_synth() { return m_is_synth; }

	void set_plugin(plum::iplugin *plugin) 
	{ 
		if (m_plugin)
		{
			m_plugin->release();
		}

		m_plugin = plugin; 
		if (m_plugin)
		{
			auto name = m_plugin->get_name();
			set_label(name);
		}
		else
		{
			set_label(m_is_synth ? "<no synth>" : "<no effect>");
		}

	}
	plum::iplugin *get_plugin() { return m_plugin; }

};


plumhost::plumhost() 
{
	m_audio.start("plum.host", &m_engine);

	set_title(APP_TITLE);
	set_default_size(920, 500);
	signal_delete_event().connect(sigc::mem_fun(this, &plumhost::on_exit));

	auto ui = Gtk::Builder::create_from_string(glade);

	Gtk::Grid *grid = nullptr;
	ui->get_widget<Gtk::Grid>("grid", grid);

	m_tv = Glib::RefPtr<Gtk::TreeView>::cast_dynamic(ui->get_object("catalog"));

	Glib::RefPtr<Gtk::ToolButton> btn;

	btn = Glib::RefPtr<Gtk::ToolButton>::cast_dynamic(ui->get_object("tbLibrary"));
	btn->signal_clicked().connect(sigc::mem_fun(this, &plumhost::on_tbLibrary));

	btn = Glib::RefPtr<Gtk::ToolButton>::cast_dynamic(ui->get_object("tbPlug"));
	btn->signal_clicked().connect(sigc::mem_fun(this, &plumhost::on_tbPlug));

	btn = Glib::RefPtr<Gtk::ToolButton>::cast_dynamic(ui->get_object("tbUnplug"));
	btn->signal_clicked().connect(sigc::mem_fun(this, &plumhost::on_tbUnplug));

	m_track = Glib::RefPtr<Gtk::ListBox>::cast_dynamic(ui->get_object("track"));
	m_track->signal_row_selected().connect(sigc::mem_fun(this, &plumhost::on_plugin_selected));

	m_presets = Glib::RefPtr<Gtk::ComboBoxText>::cast_dynamic(ui->get_object("cboPresets"));
	m_presets_sig = m_presets->signal_changed().connect(sigc::mem_fun(this, &plumhost::on_preset_selected));

	m_scroller = Glib::RefPtr<Gtk::ScrolledWindow>::cast_dynamic(ui->get_object("scroller"));
	m_current_controller = controller_none;


	auto btnSavePreset = Glib::RefPtr<Gtk::Button>::cast_dynamic(ui->get_object("btnSavePreset"));
	btnSavePreset->signal_clicked().connect(sigc::mem_fun(this, &plumhost::on_save_preset));

	auto btnLoadPreset = Glib::RefPtr<Gtk::Button>::cast_dynamic(ui->get_object("btnLoadPreset"));
	btnLoadPreset->signal_clicked().connect(sigc::mem_fun(this, &plumhost::on_load_preset));

	auto btnSaveBank = Glib::RefPtr<Gtk::Button>::cast_dynamic(ui->get_object("btnSaveBank"));
	btnSaveBank->signal_clicked().connect(sigc::mem_fun(this, &plumhost::on_save_bank));

	auto btnLoadBank = Glib::RefPtr<Gtk::Button>::cast_dynamic(ui->get_object("btnLoadBank"));
	btnLoadBank->signal_clicked().connect(sigc::mem_fun(this, &plumhost::on_load_bank));

	add(*grid);

	init_treeview();
	init_track();

	show_all_children();
}

plumhost::~plumhost() 
{
}

void plumhost::init_treeview() // catalog
{
	m_tv->append_column("Catalog", m_columns.text);

	m_ts = Gtk::TreeStore::create(m_columns);
	m_tv->set_model(m_ts);
}

void plumhost::init_track()
{
	auto n = m_engine.max_effects() + 1;
	for (uint32_t i = 0; i < n; ++i)
	{
		bool is_synth = i == 0;
		auto l = new tracklabel(is_synth);
		auto item = Gtk::manage(l);
		item->set_halign(Gtk::Align::ALIGN_START) ;
		m_track->append(*item);
		item->show();
	}
}


void plumhost::on_tbLibrary() 
{
	auto filename = choose_file(*this, "Open Library", "*.so", false);

	if (filename != "")
	{
		open_library(filename);
		m_tv->expand_all();
	}
}  

void plumhost::on_tbPlug() 
{
	auto sel = m_tv->get_selection()->get_selected();
	if (sel)
	{
		if (m_ts->iter_depth(sel) == 1)
		{
			std::vector<Gtk::TreePath> rows = m_tv->get_selection()->get_selected_rows();

			bool is_synth = rows[0][0] == 0;
			Glib::ustring plugin_name = (*sel)[m_columns.text]; 

			plug(plugin_name, is_synth);
		}
	}
}

void plumhost::on_tbUnplug() 
{
	auto sel = m_track->get_selected_row();
	if (sel)
	{
		unplug(sel);
	}
}

void plumhost::on_plugin_selected(Gtk::ListBoxRow* row)
{
	auto tl = (tracklabel *)row->get_child();
	if (tl->get_plugin())
	{
		openview(row);
	}
	else
	{
		openview(nullptr);
	}
}

void plumhost::on_preset_selected()
{
	plum::iplugin *plugin = nullptr;

	auto sel = m_track->get_selected_row();
	if (sel)
	{
		auto tl = (tracklabel *)sel->get_child();
		plugin = tl->get_plugin();
	}

	int n = m_presets->get_active_row_number();
	if (n != -1 && plugin)
	{
		plugin->set_selected_preset(n);
		display_parameters();
	}
}

bool plumhost::on_exit(GdkEventAny* event) 
{
	m_audio.stop();
	close_library();

	return false;
}


void plumhost::on_storage(bool save, bool bank)
{
	auto plugin = get_selected_plugin();

	if (plugin)
	{
		auto store = (plum::istorage *)plugin->as(IFID_PLUM_STORAGE);
		if (store)
		{
			std::string filename;

			if (save)
			{
				plum::iblob *blob = nullptr;

				if (bank)
				{
					filename = choose_file(*this, "Save", "*.plum.bank", true);

					if (filename.length() > 0)  blob = store->get_bank_data();
				}
				else
				{
					filename = choose_file(*this, "Save", "*.plum.preset", true);

					if (filename.length() > 0) blob = store->get_preset_data();
				}

				if (filename.length() > 0)
				{

					std::ofstream file(filename, std::ios::binary);
					if (file.is_open())
					{
						file.write((char *)blob->data(), blob->size());
					}
					else
					{
						printf("FILE ERROR\n");
					}

					blob->release();
				}
			}
			else
			{
				if (bank)
				{
					filename = choose_file(*this, "Load", "*.plum.bank", false);
				}
				else
				{
					filename = choose_file(*this, "Load", "*.plum.preset", false);
				}

				if (filename.length() > 0)
				{
					std::ifstream file(filename, std::ios::binary);

					if (file.is_open())
					{
						std::vector<uint8_t> buffer(std::istreambuf_iterator<char>(file), {});
						auto blob = new plum::blob(buffer.data(), buffer.size());

						if (bank)
						{
							store->set_bank_data(blob);
						}
						else
						{
							store->set_preset_data(blob);
						}
					}
					else
					{
						printf("FILE ERROR\n");
					}
				}
			}

			store->release();
		}
	}

}

void plumhost::plugin_load_preset(plum::iplugin *plugin)
{
	on_storage(false, false);
}

void plumhost::plugin_save_preset(plum::iplugin *plugin)
{
	on_storage(true, false);
}

void plumhost::plugin_load_bank(plum::iplugin *plugin)
{
	on_storage(false, true);
}

void plumhost::plugin_save_bank(plum::iplugin *plugin)
{
	on_storage(true, true);
}




void plumhost::on_save_preset()
{
	on_storage(true, false);
}

void plumhost::on_save_bank()
{
	on_storage(true, true);
}

void plumhost::on_load_preset()
{
	on_storage(false, false);
}

void plumhost::on_load_bank()
{
	on_storage(false, true);
}


// ------------------------------------------------------------------------------------
// IOBJECT
// ------------------------------------------------------------------------------------

void plumhost::reference() {}
void plumhost::release() {}

void *plumhost::as(const char *ifid)
{
	if (std::string(ifid) == IFID_PLUM_OBJECT)
	{
		reference(); return static_cast<Gtk::Window *>(this);
	}
	else if (std::string(ifid) == IFID_PLUM_HOST)
	{
		reference(); return static_cast<plum::ihost *>(this);
	}

	return nullptr;
}

// ------------------------------------------------------------------------------------
// IHOST
// ------------------------------------------------------------------------------------

void plumhost::plugin_preset_selected(plum::iplugin *plugin)
{
	display_selected_preset();
	display_parameters();
}

void plumhost::plugin_bank_changed(plum::iplugin *plugin)
{
	if (plugin == get_selected_plugin())
	{
		display_preset_list(plugin);
	}
}

void plumhost::plugin_preset_changed(plum::iplugin *plugin, uint32_t index)
{
	if (plugin == get_selected_plugin())
	{
		display_preset_list(plugin);
		display_selected_preset();
	}
}



// ------------------------------------------------------------------------------------
// 
// ------------------------------------------------------------------------------------

void plumhost::open_library(std::string filepath)
{
	close_library();

	if (m_catalog.open(filepath))
	{
		//auto v = m_catalog.version();
		

		auto row1 = *m_ts->append();
		row1[m_columns.text] = "Synths";
		
		for (auto &name : m_catalog.synth_list())
		{
			auto row2 = *m_ts->append(row1->children());
			row2[m_columns.text] = name;
		}


		row1 = *m_ts->append();
		row1[m_columns.text] = "Effects";

		for (auto &name : m_catalog.effect_list())
		{
			auto row2 = *m_ts->append(row1->children());
			row2[m_columns.text] = name;
		}
	}
}

void plumhost::clear_track()
{
	for (uint32_t index = 0; index < m_engine.max_effects() + 1; ++index)
	{
		auto row = m_track->get_row_at_index(index);
		auto tl = (tracklabel *)row->get_child();

		if (tl && tl->get_plugin())
		{		
			unplug(row);
		}
	};
}

void plumhost::close_library()
{
	closeview();
	clear_track();

	m_ts->clear();
	m_catalog.close();
}

void plumhost::plug(std::string name, bool is_synth)
{
	auto row = m_track->get_selected_row();
	if (row)
	{
		
		int index = row->get_index();

		if (is_synth && index != 0) return;
		if (!is_synth && index == 0) return;

		auto host =  (plum::ihost *)as(IFID_PLUM_HOST);
		auto plugin = m_catalog.create_plugin(name.c_str(), host);

		if (plugin == nullptr)
		{
			printf("\e[1;31mCannot create %s\033[0m\n", name.c_str());
			return;
		}

		uint32_t n;

		n = plugin->count_inputs();
		if (n == 1)
		{
			plugin->release();
			printf("\e[1;31m%s: plugin audio input must be stereo\033[0m\n", name.c_str());
			return;
		}

		n = plugin->count_outputs();
		if (n == 1)
		{
			plugin->release();
			printf("\e[1;31m%s: plugin audio output must be stereo\033[0m\n", name.c_str());
			return;
		}
		

		plugin->configure(m_audio.samplerate(), m_audio.buffersize());

		auto item = (tracklabel *)row->get_child();
		item->set_plugin(plugin);

		if (is_synth)
		{
			m_engine.set_synth(plugin); 
		}
		else
		{
			m_engine.set_effect(plugin, index - 1); 
		}

		openview(row);
	}
}

void plumhost::unplug(Gtk::ListBoxRow *row)
{
	auto sel = m_track->get_selected_row();
	if (row == sel)
	{
		closeview();
	}

	auto item = (tracklabel *)row->get_child();

	if (item->is_synth())
	{
		m_engine.set_synth(nullptr);
	}
	else
	{
		m_engine.set_effect(nullptr, row->get_index() - 1);
	}

	item->set_plugin(nullptr);
}

void plumhost::openview(Gtk::ListBoxRow *row)
{
	closeview();

	if (row)
	{
		auto sl = (tracklabel *)row->get_child();
printf("OPEN VIEW: %s\n", sl->get_label().c_str());

		plum::iplugin *plugin = sl->get_plugin();

		display_preset_list(plugin);
		display_selected_preset();

		auto *hw = (plum::ihostwindow *)m_pluginview.as(IFID_PLUM_HOSTWINDOW);
		plum::iwindow *gui = plugin->open_ui(hw);

		if (gui != nullptr)
		{
			m_scroller->add(m_pluginview);
			m_pluginview.show_all();
			m_pluginview.set_plugin_window(gui);
			m_current_controller = controller_custom;
		}
		else
		{
			m_scroller->add(m_controller);
			m_controller.show_all();
			m_controller.set_plugin(plugin);
			m_current_controller = controller_basic;
		}
	}
}

void plumhost::closeview()
{
	display_preset_list(nullptr);

	switch (m_current_controller)
	{
		case controller_basic:
			m_controller.set_plugin(nullptr);
			break;

		case controller_custom:
			m_pluginview.set_plugin_window(nullptr);
			break;

		case controller_none:
			break;
	}

	m_scroller->remove_with_viewport();
	m_current_controller = controller_none;
}

void plumhost::display_preset_list(plum::iplugin *plugin)
{
	m_presets->remove_all();
	if (plugin)
	{
		uint32_t n = plugin->count_presets();
		for (uint32_t i = 0; i < n; ++i)
		{
			auto name = plugin->get_preset_name(i);
			m_presets->append(name->text());
			name->release();
		}
	}
}

plum::iplugin *plumhost::get_selected_plugin()
{
	auto row = m_track->get_selected_row();
	if (row)
	{
		auto child = (tracklabel *)row->get_child();
		return child->get_plugin();
	}
	
	return nullptr;
}

void plumhost::display_selected_preset()
{
	auto plugin = get_selected_plugin();
	if (plugin)
	{	
		uint32_t n = plugin->get_selected_preset();
		m_presets_sig.block();
		m_presets->set_active(n);
		m_presets_sig.unblock();
	}
}

void plumhost::display_parameters()
{
	if (m_current_controller == controller_basic)
	{
		auto plugin = get_selected_plugin();
		if (plugin)
		{
			m_controller.refresh();
		}

	}
}




// ------------------------------------------------------------------------------------
// MAIN
// ------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
	auto app = Gtk::Application::create(argc, argv, "Plum.Host");
	plumhost host;
	auto code = app->run(host);
	return code;
}
