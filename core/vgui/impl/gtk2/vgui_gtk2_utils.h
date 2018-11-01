// This is core/vgui/impl/gtk2/vgui_gtk2_utils.h
#ifndef vgui_gtk2_utils_h_
#define vgui_gtk2_utils_h_
//:
// \file
// \author Robotics Research Group, University of Oxford
// \date   19-DEC-1999
// \brief  GTK support for menus.
//
//  Contains classes: vgui_gtk2_utils
//
// \verbatim
//  Modifications
//   13-JUL-2000 Marko Bacic, Oxford RRG -- Added support for menu accelerators
//   13-SEP-2002
//   05-JAN-2011 Lianqing Yu -- Add member function translate_key_reverse to
//                              fix the bug that menu accelerator text
//                              including non-alphabet keys such as F5, Home
//                              are shown incorrectly.
//   05-JAN-2011 Lianqing Yu -- Change GDK_MOD2_MASK to GDK_MOD1_MASK for
//                              modifier Alt.
// \endverbatim


#include <vgui/vgui_event.h>
#include <vgui/vgui_menu.h>
#include <gtk/gtk.h>


//: GTK support for menus.
class vgui_gtk2_utils
{
 public:
  static vgui_button translate_button(int button);
  static vgui_key translate_key(GdkEventKey const *gev);
  static guint translate_key_reverse(vgui_key key);
  static void set_coordinates(vgui_event &e, const gdouble x, const gdouble y);
  static void set_modifiers(vgui_event &e, const guint state);
  static bool is_modifier(GdkEvent const *gev);

  static void add_submenu(GtkWidget *widget, const vgui_menu& menu);
  static void set_menu(GtkWidget *widget, const vgui_menu& menu, bool is_menubar=false);
  static GtkAccelGroup *accel_group;
};

#endif // vgui_gtk2_utils_h_
