// This is oxl/vgui/impl/mfc/vgui_mfc_utils.h
#ifndef vgui_mfc_utils_h_
#define vgui_mfc_utils_h_
#ifdef __GNUC__
#pragma interface
#endif
//:
// \file
// \author  Marko Bacic, Oxford RRG
// \date    4 August 2000
// \brief   Provides support for menus 
//
// \verbatim
//  Modifications:
//   04-AUG-2000 Marko Bacic - Initial version.
//   14-AUG-2000 Marko Bacic - Added support for popup menus
//   16-MAR-2001 K.Y.McGaul - Added menu accelerators
//   22-AUG-2001 K.Y.McGaul - Added destructor to fix memory leak: all menus now deleted.
// \endverbatim

#include <vgui/vgui_menu.h>
#include "StdAfx.h"
//: Note that the range 0x8000-0x8400 is reserved for dialog box callbacks
#define ID_MENU_ITEMS 0x8400
#define MAX_ITEM_COUNT 0x400 // (dec 1024)

typedef void (*fp)();

//: MFC version of vgui_utils - provides support for menus.
//  Only one instance of the class is allowed
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
  //: Add keyboard shortcut for this menu item to our accelerator table (accels).
  void add_menu_accelerator(const vgui_menu_item item, const WORD function_id, vcl_string& the_menu_name);

public:
  //: (Create if necessary and) return singleton instance of this class.
  static vgui_mfc_utils *instance();
  //: Constructor.
  vgui_mfc_utils();
  //: Destructor.
  ~vgui_mfc_utils();
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

private:
  //: List of menus to be deleted when we are finished:
  vcl_vector<CMenu*> menus_to_be_deleted;
};

#endif // vgui_mfc_utils_h_
