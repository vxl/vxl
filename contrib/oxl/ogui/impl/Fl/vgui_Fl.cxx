// This is oxl/vgui/impl/Fl/vgui_Fl.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   16 Sep 99
// \brief  See vgui_Fl.h for a description of this file.
//-----------------------------------------------------------------------------

#include "vgui_Fl.h"

#include <vcl_algorithm.h>

#include <FL/Fl.H>
#include "vgui_Fl_adaptor.h"
#include "vgui_Fl_window.h"
#include "vgui_Fl_dialog_impl.h"
#include <vul/vul_trace.h>

vgui_Fl* vgui_Fl::instance_ = 0;

vgui_Fl* vgui_Fl::instance()
{
  vul_trace;
  if (instance_ == 0)
    instance_ = new vgui_Fl;

  return instance_;
}

vgui_Fl::vgui_Fl()
{
}

vcl_string vgui_Fl::name() const { return "fltk"; }

void vgui_Fl::run()
{
  Fl::run();
}

void vgui_Fl::run_one_event()
{
  Fl::wait();
}

void vgui_Fl::flush()
{
  Fl::flush();
}


void vgui_Fl::add_event(const vgui_event& event)
{
  vcl_cerr << "broken\n";
}


vgui_window* vgui_Fl::produce_window(int width, int height, const vgui_menu& menubar,
                                          const char* title)
{
  vcl_cerr << "vgui_FL::produce_window_impl\n";
  return new vgui_Fl_window(width, height, menubar, title);
}

vgui_window* vgui_Fl::produce_window(int width, int height,
                                          const char* title)
{
  return new vgui_Fl_window(width, height, title);
}

vgui_dialog_impl* vgui_Fl::produce_dialog(const char* name)
{
  return new vgui_Fl_dialog_impl(name);
}
