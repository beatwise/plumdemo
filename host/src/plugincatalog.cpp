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

#include "plugincatalog.h"

bool plugincatalog::open(std::string path)
{
	close();

	try
	{
		m_loader.open(path);

		m_fn_plum_begin = m_loader.getFunction<void()>("plum_begin");
		m_fn_plum_end = m_loader.getFunction<void()>("plum_end");
		m_fn_plum_get_version = m_loader.getFunction<plum_version()>("plum_get_version");
		m_fn_plum_catalog = m_loader.getFunction<const char *(int, uint32_t)>("plum_catalog");
		m_fn_plum_plugin_info = m_loader.getFunction<const char *(const char *name)>("plum_plugin_info");
		m_fn_plum_create_plugin = m_loader.getFunction<plum::iplugin*(const char *name, plum::ihost *)>("plum_create_plugin");

		m_version = m_fn_plum_get_version();

		printf("\e[1;33mLIBRARY:\033[0m %s\n", path.c_str());
		printf("\e[1;33mPLUM VERSION:\033[0m  %u.%u\n\n", m_version.major, m_version.minor);


		if (m_fn_plum_begin) 
		{
			m_fn_plum_begin();
		}

		if (m_fn_plum_catalog) 
		{
			
			for (uint32_t i = 0;; ++i)
			{
				const char *name = m_fn_plum_catalog(PLUM_SYNTH, i);
				if (name == nullptr) break;
				m_synths.push_back(name);
				auto description = m_fn_plum_plugin_info(name);
				if (description)
				{
					printf("\e[1;33mSYNTH:\033[0m %s\n\e[1;33mDESCRIPTION:\033[0m %s\n\n", name, description);
				}
			}

			for (uint32_t i = 0;; ++i)
			{
				const char *name = m_fn_plum_catalog(PLUM_EFFECT, i);
				if (name == nullptr) break;
				m_effects.push_back(name);
				auto description = m_fn_plum_plugin_info(name);
				if (description)
				{
					printf("\e[1;33mEFFECT:\033[0m %s\n\e[1;33mDESCRIPTION:\033[0m %s\n\n", name, description);
				}

			}
		}
	}
	catch (const DyLib::exception &e) 
	{
		printf("\e[0;31mcatalog:\033[0m %s\n", e.what());
		close();	
		return false;
	}	

	return true;
}

void plugincatalog::close()
{
	if (m_fn_plum_end)
	{
		m_fn_plum_end();
	}

	m_synths.clear();
	m_effects.clear();

	m_fn_plum_begin = nullptr;
	m_fn_plum_end = nullptr;
	m_fn_plum_get_version = nullptr;
	m_fn_plum_catalog = nullptr;
	m_fn_plum_plugin_info = nullptr;
	m_fn_plum_create_plugin = nullptr;

	m_loader.close();
}

plum_version plugincatalog::version()
{
	return m_version;
}

std::vector<std::string> plugincatalog::synth_list()
{
	return m_synths;
}

std::vector<std::string> plugincatalog::effect_list()
{
	return m_effects;
}


plum::iplugin * plugincatalog::create_plugin(std::string name, plum::ihost *host)
{
	if (!m_fn_plum_create_plugin)
	{
		return nullptr;
	}

	return m_fn_plum_create_plugin(name.c_str(), host);
}


