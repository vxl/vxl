// This is oxl/vgui/impl/Fl/vgui_Fl_utils.h
#ifndef vgui_Fl_utils_h_
#define vgui_Fl_utils_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   28 Oct 99
// \brief  FLTK support for menus.
//
//  Contains classes: vgui_Fl_utils
//
// \verbatim
//  Modifications:
//    17-Sep-2002 K.Y.McGaul - Converted to doxygen style documentation.
// \endverbatim

struct Fl_Menu_Item;
class Fl_Menu_;
class vgui_menu;

//: FLTK support for menus.
class vgui_Fl_utils
{
 public:
  static Fl_Menu_Item* create_submenu(const vgui_menu& menu);
  static void add_submenu(Fl_Menu_Item* menu_item, const vgui_menu& menu);
  static void set_menu(Fl_Menu_* widget, const vgui_menu& menu);
};

#endif // vgui_Fl_utils_h_
