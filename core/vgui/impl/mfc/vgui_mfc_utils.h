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
//   14-AUG-2000 Marko Bacic   Added support for popup menus
//   16-MAR-2001 K.Y.McGaul    Added menu accelerators.
//-----------------------------------------------------------------------------

#include <vgui/vgui_event.h>
#include <vgui/vgui_menu.h>
#include "stdafx.h"
//: Note that the range 0x8000-0x8400 is reserved for dialog box callbacks
#define ID_MENU_ITEMS 0x8400
#define MAX_ITEM_COUNT 0x400 // (dec 1024)

typedef void (*fp)();

//: Only one instance of the class is allowed
class vgui_mfc_utils 
{
  //: Number of menu items.
  int item_count;
  //: Singleton instance of this class.
  static vgui_mfc_utils *instance_;
  //: List of commands to be called by menus.
  vcl_vector<vgui_command *> callbacks;
  //: List of menu accelerators.
  vcl_vector<ACCEL> accels;

public:
  //: (Create if necessary and) return singleton instance of this class.
  static vgui_mfc_utils *instance();
  //: Constructor.
  vgui_mfc_utils();
  //: Create a MFC sub-menu from the given vgui_menu.
  HMENU add_submenu(const vgui_menu& menu);
  //: Sets the menu of the application window
  void set_menu(const vgui_menu& menu);
  //: Create popup menu. 
  CMenu *set_popup_menu(const vgui_menu & menu);
  //: Called within message service routine of vgui_mfc_mainfrm.
  virtual void menu_dispatcher(UINT nID);
  //: Menu accelerator table - this defines menu shortcuts
  HACCEL AccelTable;

};

#endif // vgui_mfc_utils_h_
