#ifndef vgui_mfc_util_h_
#define vgui_mfc_util_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME vgui_mfc_utils - Provides support for menus
// .LIBRARY vgui-mfc
// .SECTION Author
//    Marko Bacic, Oxford RRG
// Created: 4 August 2000
// .SECTION Modifications:
//
//-----------------------------------------------------------------------------

#include <vgui/vgui_event.h>
#include <vgui/vgui_menu.h>
#include "stdafx.h"


class vgui_mfc_utils {
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

#endif // vgui_mfc_utils_h_
