#ifndef vgui_mfc_utils_h_
#define vgui_mfc_utils_h_
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
//   14-AUG-2000 Marko Bacic, Oxford RRG -- Added support for popup menus
//-----------------------------------------------------------------------------

#include <vgui/vgui_event.h>
#include <vgui/vgui_menu.h>
#include "stdafx.h"
// -- Note that the range 0x8000-0x8400 is reserved
// for dialog box callbacks
#define ID_MENU_ITEMS 0x8400
#define MAX_ITEM_COUNT 0x400 // (dec 1024)
// -- Only one instance of the class is allowed
typedef void (*fp)();
class vgui_mfc_utils {
  int item_count;
  static vgui_mfc_utils *instance_;
  vcl_vector<vgui_command *> callbacks;
public:
  static vgui_mfc_utils *instance();
  vgui_mfc_utils();
  HMENU add_submenu(const vgui_menu& menu);
  void set_menu(const vgui_menu& menu);
  CMenu *set_popup_menu(const vgui_menu & menu);
  virtual void menu_dispatcher(UINT nID);

};

#endif // vgui_mfc_utils_h_
