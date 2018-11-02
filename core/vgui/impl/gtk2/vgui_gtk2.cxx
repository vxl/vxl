// This is core/vgui/impl/gtk2/vgui_gtk2.cxx
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   16 Sep 99
// \brief  See vgui_gtk2.h for a description of this file.

#include "vgui_gtk2.h"
#include <vgui/vgui_gl.h> // glFlush()
#include <gtk/gtk.h>
#include "vgui_gtk2_window.h"
#include "vgui_gtk2_dialog_impl.h"
#ifdef DEBUG
# include <std::iostream.h>
#endif

vgui_gtk2* vgui_gtk2::instance()
{
  static vgui_gtk2* instance_ = new vgui_gtk2;
  return instance_;
}

//--------------------------------------------------------------------------------
//: Pure virtual function from vgui (this must be implemented).
//  Returns the name of the GUI toolkit.
std::string vgui_gtk2::name() const { return "gtk2"; }


//--------------------------------------------------------------------------------
//: Virtual function from vgui.  Initialise the implementation of vgui.
void vgui_gtk2::init(int &argc, char **argv)
{
#ifdef DEBUG
  std::cerr << "vgui_gtk2::init()\n";
#endif
  gtk_init(&argc, &argv);
}


//--------------------------------------------------------------------------------
//: Virtual function from vgui.   Runs the event loop.
void vgui_gtk2::run()
{
#ifdef DEBUG
  std::cerr << "vgui_gtk2::run()\n";
#endif
  gtk_main();
}

void vgui_gtk2::run_one_event()
{
  gtk_main_iteration();
}

void vgui_gtk2::run_till_idle()
{
  while (gtk_events_pending()) {
    gtk_main_iteration();
    glFlush();
  }
}

void vgui_gtk2::flush()
{
  glFlush();
  run_till_idle();
}

void vgui_gtk2::quit()
{
#ifdef DEBUG
  std::cerr << __FILE__ " : terminating GTK+ event loop\n";
#endif
  gtk_main_quit(); // capes@robots - causes the gtk event loop to return
}

//--------------------------------------------------------------------------------
//: Virtual function from vgui.  Creates a new window with a menubar.
vgui_window* vgui_gtk2::produce_window(int width, int height, const vgui_menu& menubar,
                                       const char* title)
{
  return new vgui_gtk2_window(width, height, menubar, title);
}


//--------------------------------------------------------------------------------
//: Virtual function from vgui.  Creates a new window.
vgui_window* vgui_gtk2::produce_window(int width, int height,
                                       const char* title)
{
  return new vgui_gtk2_window(width, height, title);
}


//--------------------------------------------------------------------------------
//: Virtual function from vgui.  Creates a new dialog box.
vgui_dialog_impl* vgui_gtk2::produce_dialog(const char* name)
{
  return new vgui_gtk2_dialog_impl(name);
}
