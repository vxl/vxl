// This is oxl/ogui/impl/Fl/vgui_Fl_window.h
#ifndef vgui_Fl_window_h_
#define vgui_Fl_window_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   05 Oct 99
// \brief  The FLTK (Fast Light Toolkit) implementation of vgui_window.
//
//  Contains classes:  vgui_Fl_window
//
// \verbatim
//  Modifications:
//    17-Sep-2002 K.Y.McGaul - Converted to doxygen style documenation.
// \endverbatim
//-----------------------------------------------------------------------------

#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Output.H>
#include <vgui/vgui_window.h>
#include <vgui/vgui_menu.h>
#include "vgui_Fl_statusbar.h"
class vgui_Fl_adaptor;

//: The FLTK (Fast Light Toolkit) implementation of vgui_window.
class vgui_Fl_window : public Fl_Window, public vgui_window
{
 public:

  vgui_Fl_window(int w, int h, const vgui_menu& menu, const char* title);
  vgui_Fl_window(int w, int h, const char* title);
  ~vgui_Fl_window();

  void set_menubar(const vgui_menu&);
  void set_statusbar(bool);
  void set_adaptor(vgui_adaptor* a);
  vgui_adaptor* get_adaptor();

  bool use_menubar;
  bool use_statusbar;

  void show();
  void hide();
  void resize(int x, int y, int w, int h);
  int handle(int);

  Fl_Menu_Bar menubar;
  vgui_Fl_adaptor* adaptor;
  vgui_Fl_statusbar statusbar;
  Fl_Box box;

 private:
  vgui_menu last_menubar; // <-- ask fsm about this.
};


#endif // vgui_Fl_window_h_
