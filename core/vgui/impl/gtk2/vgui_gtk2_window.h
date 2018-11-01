// This is core/vgui/impl/gtk2/vgui_gtk2_window.h
#ifndef vgui_gtk2_window_h_
#define vgui_gtk2_window_h_
//:
// \file
// \author  Robotics Research Group, University of Oxford
// \date    18 Dec 99
// \brief   The GTK implementation of vgui_window.
//
//  Contains classes: vgui_gtk2_window
//
// \verbatim
//  Modifications
//   13-Jul-2000 Marko Bacic, Oxford RRG - Added support for menu shortcuts
//   21-Sep-2000 capes@robots- Changed destructor to actually destroy the widgets
//                             Facilitates the post_destroy() adaptor method.
//   13-Sep-2002 K.Y.McGaul  - Changed to Doxygen style comments.
//   07-Jan-2011 Lianqing Yu - Move member function init() from show() to the
//                             constructors so that client may call show()
//                             multiple times without triggering GTK+ warning
//                             that a child widget (indeed its member box) is
//                             inserted into a GtkBin widget (actually its
//                             member window) that already has a child.
// \endverbatim

#include <vgui/vgui_window.h>
#include <vgui/vgui_menu.h>

#include <gtk/gtk.h>

//: The GTK implementation of vgui_window.
//  Provides functions for manipulating a window.
class vgui_gtk2_window : public vgui_window
{
 public:

  vgui_gtk2_window(int w, int h, const vgui_menu& menu, const char* title);
  vgui_gtk2_window(int w, int h, const char* title);
 ~vgui_gtk2_window();

  bool use_menubar;
  bool use_statusbar;

  void init();

  void show();
  void hide();
  void reshape(unsigned, unsigned);
  void set_title(std::string const &);

  void set_menubar(const vgui_menu &menu);

  //: Returns the current adaptor (OpenGL widget holder).
  vgui_adaptor* get_adaptor() { return adaptor; }
  void set_adaptor(vgui_adaptor* a) { adaptor=a; }

  vgui_statusbar* get_statusbar() { return statusbar; }
  void set_statusbar(bool) {}

  // gtk specific
  vgui_adaptor *adaptor;
  vgui_statusbar *statusbar;

  GtkWidget *window;
  GtkWidget *box;
  GtkWidget *menubar;

 private:
  // This is a place to store any menu passed in, so that it doesn't go out
  // of scope while the popup is on screen.
  vgui_menu* last_menubar; // <-- ask fsm about this.
};

#endif // vgui_gtk2_window_h_
