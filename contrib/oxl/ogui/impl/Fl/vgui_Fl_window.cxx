// This is oxl/ogui/impl/Fl/vgui_Fl_window.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   05 Oct 99
// \brief  See vgui_Fl_window.h for a description of this file.
//-----------------------------------------------------------------------------

#include "vgui_Fl_window.h"

#include <vcl_compiler.h>

#include <vgui/vgui.h>

#include <FL/Fl.H>
#include <FL/Fl_Box.H>

#include "vgui_Fl.h"
#include "vgui_Fl_adaptor.h"
#include "vgui_Fl_utils.h"


static int init_w;
static int init_h;

vgui_Fl_window::vgui_Fl_window(int w, int h, const vgui_menu& menu, const char* l)
  : Fl_Window(w,h,l)
  , use_menubar(true)
  , use_statusbar(true)
  , menubar(0,0,0,0)
  , adaptor(0)
  , statusbar(0,0,0,0)
  , box(0,0,0,0)
{
  init_w = w;
  init_h = h;

  //vgui_Fl_utils::set_menu(&menubar, menu);
  set_menubar(menu); // call this instead to avoid segfault -- fsm.

  adaptor = new vgui_Fl_adaptor(0, 0, w, h, l);
  Fl_Group::current()->resizable(&box);
  end();
}


vgui_Fl_window::vgui_Fl_window(int w, int h, const char* l)
  : Fl_Window(w,h,l)
  , use_menubar(false)
  , use_statusbar(true)
  , menubar(0,0,0,0)
  , adaptor(0)
  , statusbar(0,0,0,0)
  , box(0,0,0,0)
{
  init_w = w;
  init_h = h;

  adaptor = new vgui_Fl_adaptor(0, 0, w, h, l);

  Fl_Group::current()->resizable(&box);
  end();
}

vgui_Fl_window::~vgui_Fl_window() {
  delete adaptor;
}


void vgui_Fl_window::show() {
  statusbar.color(49);
  statusbar.textsize(12);

  int adaptor_y = 0;
  int adaptor_h = init_h;

  if (use_menubar) {
    menubar.resize(0,0,init_w,25);
    adaptor_y += 25;
    adaptor_h -= 25;
  }

  if (use_statusbar) {
    statusbar.resize(0,init_h-25,init_w,25);
    adaptor_h -= 25;
  }

  adaptor->resize(0,adaptor_y,init_w,adaptor_h);
  box.resize(0,adaptor_y,init_w,adaptor_h);


  Fl_Window::show();
  adaptor->show();
}

void vgui_Fl_window::hide() {
  Fl_Window::hide();
  adaptor->hide();
}


void vgui_Fl_window::resize(int x, int y, int w, int h) {
  Fl_Window::resize(x,y,w,h);
}


int vgui_Fl_window::handle(int event) {

  if (event == FL_ENTER) {
#ifndef VCL_WIN32
    if (use_statusbar)
      vgui::out.rdbuf(statusbar.statusbuf);
    else
      vgui::out.rdbuf(vcl_cout.rdbuf());
#endif
    return 1;
  }
  else
    return Fl_Window::handle(event);

  return 0;
}


void vgui_Fl_window::set_statusbar(bool use) {
  use_statusbar = use;
}

void vgui_Fl_window::set_menubar(const vgui_menu& menu) {
  last_menubar = menu;
  vgui_Fl_utils::set_menu(&menubar,last_menubar);
  use_menubar = true;
}

void vgui_Fl_window::set_adaptor(vgui_adaptor* a) {
  adaptor = static_cast<vgui_Fl_adaptor*>(a);
}

vgui_adaptor* vgui_Fl_window::get_adaptor() {
  return adaptor;
}

