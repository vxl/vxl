// This is core/vgui/impl/gtk/vgui_gtk_window.h
#ifndef vgui_gtk_window_h_
#define vgui_gtk_window_h_
//:
// \file
// \author  Robotics Research Group, University of Oxford
// \date    18 Dec 99
// \brief   The GTK implementation of vgui_window.
//
//  Contains classes: vgui_gtk_window
//
// \verbatim
//  Modifications
//   13-JUL-00   Marko Bacic, Oxford RRG -- Added support for menu shortcuts
//   21-SEP-00   capes@robots -- Changed destructor to actually destroy the widgets
//                              Facilitates the post_destroy() adaptor method.
//   13-SEP-2002 K.Y.McGaul - Changed to Doxygen style comments.
// \endverbatim


#include <vgui/vgui_window.h>
#include <vgui/vgui_menu.h>

#include <gtk/gtk.h>
#include "vgui_gtk_statusbar.h"

//: The GTK implementation of vgui_window.
//  Provides functions for manipulating a window.
class vgui_gtk_window : public vgui_window
{
 public:

  vgui_gtk_window(int w, int h, const vgui_menu& menu, const char* title);
  vgui_gtk_window(int w, int h, const char* title);
 ~vgui_gtk_window();

  bool use_menubar;
  bool use_statusbar;

  void init();

  void show();
  void hide();
  void set_title(std::string const &);

  void set_menubar(const vgui_menu &menu);
  //: Returns the current adaptor (OpenGL widget holder).
  vgui_adaptor* get_adaptor() { return adaptor; }
  void set_adaptor(vgui_adaptor* a) {adaptor=a;}
  vgui_statusbar* get_statusbar() { return statusbar; }
  void set_statusbar(bool) {}

  // gtk specific
  vgui_adaptor *adaptor;
  vgui_statusbar* statusbar;

  GtkWidget *window;
  GtkWidget *box;
  GtkWidget *menubar;

 private:
  // This is a place to store any menu passed in, so that it doesn't go out
  // of scope while the popup is on screen.
  vgui_menu* last_menubar; // <-- ask fsm about this.
};

#endif // vgui_gtk_window_h_
