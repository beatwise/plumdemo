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

const char *glade = 
R"..(
<?xml version="1.0" encoding="UTF-8"?>
<!-- Generated with glade 3.20.0 -->
<interface>
  <requires lib="gtk+" version="3.20"/>
  <object class="GtkGrid" id="grid">
    <property name="visible">True</property>
    <property name="can_focus">False</property>
    <property name="hexpand">True</property>
    <property name="vexpand">True</property>
    <child>
      <object class="GtkToolbar" id="toolbar">
        <property name="visible">True</property>
        <property name="can_focus">False</property>
        <child>
          <object class="GtkToolButton" id="tbLibrary">
            <property name="visible">True</property>
            <property name="can_focus">False</property>
            <property name="label" translatable="yes">Library</property>
            <property name="use_underline">True</property>
            <signal name="clicked" handler="on_tbLibrary_clicked" swapped="no"/>
          </object>
          <packing>
            <property name="expand">False</property>
            <property name="homogeneous">True</property>
          </packing>
        </child>
        <child>
          <object class="GtkToolButton" id="tbPlug">
            <property name="visible">True</property>
            <property name="can_focus">False</property>
            <property name="label" translatable="yes">Plug</property>
            <property name="use_underline">True</property>
          </object>
          <packing>
            <property name="expand">False</property>
            <property name="homogeneous">True</property>
          </packing>
        </child>
        <child>
          <object class="GtkToolButton" id="tbUnplug">
            <property name="visible">True</property>
            <property name="can_focus">False</property>
            <property name="label" translatable="yes">Unplug</property>
            <property name="use_underline">True</property>
          </object>
          <packing>
            <property name="expand">False</property>
            <property name="homogeneous">True</property>
          </packing>
        </child>
      </object>
      <packing>
        <property name="left_attach">0</property>
        <property name="top_attach">0</property>
        <property name="width">3</property>
      </packing>
    </child>
    <child>
      <object class="GtkTreeView" id="catalog">
        <property name="width_request">180</property>
        <property name="visible">True</property>
        <property name="can_focus">True</property>
        <property name="vexpand">True</property>
        <child internal-child="selection">
          <object class="GtkTreeSelection"/>
        </child>
      </object>
      <packing>
        <property name="left_attach">0</property>
        <property name="top_attach">1</property>
        <property name="height">3</property>
      </packing>
    </child>
    <child>
      <object class="GtkScrolledWindow" id="scroller">
        <property name="visible">True</property>
        <property name="can_focus">True</property>
        <property name="hexpand">True</property>
        <property name="vexpand">True</property>
        <property name="shadow_type">in</property>
        <child>
          <placeholder/>
        </child>
      </object>
      <packing>
        <property name="left_attach">1</property>
        <property name="top_attach">1</property>
        <property name="height">3</property>
      </packing>
    </child>
    <child>
      <object class="GtkBox">
        <property name="visible">True</property>
        <property name="can_focus">False</property>
        <property name="orientation">vertical</property>
        <child>
          <object class="GtkComboBoxText" id="cboPresets">
            <property name="visible">True</property>
            <property name="can_focus">False</property>
          </object>
          <packing>
            <property name="expand">False</property>
            <property name="fill">True</property>
            <property name="position">0</property>
          </packing>
        </child>
        <child>
          <object class="GtkListBox" id="track">
            <property name="width_request">180</property>
            <property name="visible">True</property>
            <property name="can_focus">False</property>
            <property name="vexpand">True</property>
          </object>
          <packing>
            <property name="expand">False</property>
            <property name="fill">True</property>
            <property name="position">1</property>
          </packing>
        </child>
        <child>
          <object class="GtkGrid">
            <property name="visible">True</property>
            <property name="can_focus">False</property>
            <property name="halign">center</property>
            <child>
              <object class="GtkButton" id="btnSavePreset">
                <property name="label" translatable="yes">SP</property>
                <property name="visible">True</property>
                <property name="can_focus">True</property>
                <property name="receives_default">True</property>
              </object>
              <packing>
                <property name="left_attach">0</property>
                <property name="top_attach">0</property>
              </packing>
            </child>
            <child>
              <object class="GtkButton" id="btnSaveBank">
                <property name="label" translatable="yes">SB</property>
                <property name="visible">True</property>
                <property name="can_focus">True</property>
                <property name="receives_default">True</property>
              </object>
              <packing>
                <property name="left_attach">1</property>
                <property name="top_attach">0</property>
              </packing>
            </child>
            <child>
              <object class="GtkButton" id="btnLoadPreset">
                <property name="label" translatable="yes">LP</property>
                <property name="visible">True</property>
                <property name="can_focus">True</property>
                <property name="receives_default">True</property>
              </object>
              <packing>
                <property name="left_attach">0</property>
                <property name="top_attach">1</property>
              </packing>
            </child>
            <child>
              <object class="GtkButton" id="btnLoadBank">
                <property name="label" translatable="yes">LB</property>
                <property name="visible">True</property>
                <property name="can_focus">True</property>
                <property name="receives_default">True</property>
              </object>
              <packing>
                <property name="left_attach">1</property>
                <property name="top_attach">1</property>
              </packing>
            </child>
          </object>
          <packing>
            <property name="expand">False</property>
            <property name="fill">True</property>
            <property name="position">2</property>
          </packing>
        </child>
      </object>
      <packing>
        <property name="left_attach">2</property>
        <property name="top_attach">1</property>
        <property name="height">3</property>
      </packing>
    </child>
  </object>
</interface>



)..";

