// This is oxl/vgui/impl/gtk/vgui_gtk_utils.h
#ifndef vgui_gtk_utils_h_
#define vgui_gtk_utils_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//
// .NAME vgui_gtk_utils - Undocumented class FIXME
// .LIBRARY vgui-gtk
// .HEADER vxl Package
// .INCLUDE vgui/impl/gtk/vgui_gtk_utils.h
// .FILE vgui_gtk_utils.cxx
//
// .SECTION Description
// vgui_gtk_utils is a class that Phil hasnt documented properly. FIXME
//
// .SECTION Author
//              Philip C. Pritchett, 19 Dec 99
//              Robotics Research Group, University of Oxford
//
// .SECTION Modifications
//   13-JUL-00  Marko Bacic, Oxford RRG -- Added support for menu accelerators
//
//-----------------------------------------------------------------------------

#include <vgui/vgui_event.h>
#include <vgui/vgui_menu.h>
#include <gtk/gtk.h>


class vgui_gtk_utils
{
 public:
  static vgui_button translate_button(int button);
  static vgui_key translate_key(GdkEventKey const *gev);
  static void set_coordinates(vgui_event &e, const gdouble x, const gdouble y);
  static void set_modifiers(vgui_event &e, const guint state);
  static bool is_modifier(GdkEvent const *gev);

  static void add_submenu(GtkWidget *widget, const vgui_menu& menu);
  static void set_menu(GtkWidget *widget, const vgui_menu& menu, bool is_menubar=false);
  static GtkAccelGroup *accel_group;
};

#endif // vgui_gtk_utils_h_
