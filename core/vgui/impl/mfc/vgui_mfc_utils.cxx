// This is core/vgui/impl/mfc/vgui_mfc_utils.cxx
//:
// \file
// \brief See vgui_mfc_utils.h for a description of this file.
// \author  Marko Bacic, Oxford RRG
// \date    4 August 2000
//
// \verbatim
//  Modifications
//   20-JUL-2001  K.Y.McGaul  Added menu accelerators.
//   22-AUG-2001  K.Y.McGaul  Added destructor to fix memory leak: all menus now deleted.
//   25-JUL-2006  A. Tamrakar Fixed the "Popup Menu timeout" bug.
//   11-FEB-2007  A. Khropov Fixed the "Main menu callbacks not initialized" bug.
// \endverbatim

#include <iostream>
#include "vgui_mfc_utils.h"
#include "vgui/vgui_command.h"
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
static bool debug = false;


//: Called within message service routine of vgui_mfc_mainfrm.
void
vgui_mfc_utils::menu_dispatcher(UINT nID)
{
  // Make sure nID is in the relevant range
  ASSERT(nID >= ID_MENU_ITEMS && int(nID) < ID_MENU_ITEMS + item_count);
  // Call the callback function associated with the menu item
  callbacks[nID - ID_MENU_ITEMS]->execute();
}

//: (Create if necessary and) return singleton instance of this class.
vgui_mfc_utils *
vgui_mfc_utils::instance()
{
  static vgui_mfc_utils instance_;
  return &instance_;
}

//: Destructor.
vgui_mfc_utils::~vgui_mfc_utils()
{
  for (unsigned i = 0; i < menus_to_be_deleted.size(); i++)
    delete menus_to_be_deleted[i];
}

//: Add keyboard shortcut for this menu item to our accelerator table (accels).
//  Also add text to the_menu_name for the shortcut.
void
vgui_mfc_utils::add_menu_accelerator(const vgui_menu_item menu_item,
                                     const WORD function_id,
                                     std::string & the_menu_name)
{
  ACCEL acc;
  acc.cmd = function_id;
  the_menu_name += std::string("\t"); // tab to right hand side of label
  if (menu_item.short_cut.mod == vgui_SHIFT)
  {
    acc.fVirt = FSHIFT | FVIRTKEY; // you can't use modifiers unless you use virtual key codes
                                   // hence the need for FVIRTKEY here.
    the_menu_name += std::string("Shift+");
  }
  else if (menu_item.short_cut.mod == vgui_CTRL)
  {
    acc.fVirt = FCONTROL | FVIRTKEY;
    the_menu_name += std::string("Ctrl+");
  }
  else if (menu_item.short_cut.mod == vgui_ALT)
  {
    acc.fVirt = FALT | FVIRTKEY;
    the_menu_name += std::string("Alt+");
  }
  else // we are going to give a virtual key code, even if there is no modifier:
    acc.fVirt = FVIRTKEY;
  // VkKeyScan converts the key code from ASCII to a virtual key code:
  acc.key = VkKeyScan(menu_item.short_cut.key);
  the_menu_name += menu_item.short_cut.key;
  accels.push_back(acc);
}

//: Create a MFC sub-menu from the given vgui_menu.
HMENU
vgui_mfc_utils::add_submenu(const vgui_menu & menu)
{
  CMenu * popdown_menu;

  // Create a new menu
  popdown_menu = new CMenu();
  menus_to_be_deleted.push_back(popdown_menu);
  popdown_menu->CreatePopupMenu();

  for (unsigned i = 0; i < menu.size(); i++)
  {
    if (menu[i].is_separator())
    {
      if (debug)
        std::cerr << " <separator>\n";
      popdown_menu->AppendMenu(MF_SEPARATOR);
    }
    else if (menu[i].is_command())
    {
      if (debug)
        std::cerr << " <command>\n";
      int the_menu_id = ID_MENU_ITEMS + item_count++;
      std::string the_menu_name = menu[i].name;

      // Add menu accelerators:
      if (menu[i].short_cut.mod != vgui_MODIFIER_NULL || menu[i].short_cut.key != vgui_KEY_NULL)
        add_menu_accelerator(menu[i], the_menu_id, the_menu_name);

      popdown_menu->AppendMenu(MF_STRING, the_menu_id, the_menu_name.c_str());
      // Add to our callback list, the associated callback function pointer
      vgui_command * cmnd = static_cast<vgui_command *>(menu[i].cmnd.as_pointer());
      callbacks.push_back(cmnd);
    }
    else if (menu[i].is_submenu())
    {
      if (debug)
        std::cerr << " <submenu>\n";
      popdown_menu->AppendMenu(MF_POPUP, (UINT_PTR)add_submenu(*menu[i].menu), menu[i].name.c_str());
    }
  }
  return popdown_menu->GetSafeHmenu();
}

//: Sets the menu of the application window
void
vgui_mfc_utils::set_menu(const vgui_menu & menu)
{
  // Get the application thread (this is the single CWinApp object associated with the application):
  CWinApp * theapp = AfxGetApp();
  // Obtain the main window associated with it
  CWnd * window = theapp->GetMainWnd();

  CMenu * menu_bar = new CMenu();
  menus_to_be_deleted.push_back(menu_bar);
  menu_bar->CreateMenu();
  window->SetMenu(menu_bar);

  for (unsigned i = 0; i < menu.size(); i++)
  {
    if (menu[i].is_separator())
    {
      if (debug)
        std::cerr << " <separator>\n";
      menu_bar->AppendMenu(MF_SEPARATOR);
    }
    else if (menu[i].is_command())
    {
      int the_menu_id = ID_MENU_ITEMS + item_count++;
      std::string the_menu_name = menu[i].name;

      // Add menu accelerators
      if (menu[i].short_cut.mod != vgui_MODIFIER_NULL || menu[i].short_cut.key != vgui_KEY_NULL)
        add_menu_accelerator(menu[i], the_menu_id, the_menu_name);

      if (debug)
        std::cerr << " <command> " << menu[i].name << std::endl;
      menu_bar->AppendMenu(MF_STRING | MF_ENABLED, the_menu_id, the_menu_name.c_str());
      // Add to our callback list, the associated callback function pointer
      vgui_command * cmnd = static_cast<vgui_command *>(menu[i].cmnd.as_pointer());
      callbacks.push_back(cmnd);
    }
    else if (menu[i].is_submenu())
    {
      if (debug)
        std::cerr << " <submenu> " << menu[i].name << std::endl;
      menu_bar->AppendMenu(MF_POPUP, (UINT_PTR)add_submenu(*menu[i].menu), menu[i].name.c_str());
    }
  }
  if (accels.size() > 0)
    AccelTable = CreateAcceleratorTable(&accels[0], accels.size());
  window->DrawMenuBar();
  window->UpdateWindow();
  window->ShowWindow(SW_SHOW);
}

//: Create popup menu.
//  Make sure to call delete after its use otherwise MLK!!!
CMenu *
vgui_mfc_utils::set_popup_menu(const vgui_menu & menu)
{
  // if there was a popup menu created before,
  // delete the callback functions from that menu
  if (!first_popup)
    delete_last_popup_menu_callbacks();
  else
    first_popup = false;

  // store the current item count
  last_item_count = item_count;

  // Create a new menu
  CMenu * pop_up = new CMenu();
  pop_up->CreatePopupMenu();

  for (unsigned i = 0; i < menu.size(); i++)
  {
    if (menu[i].is_separator())
    {
      if (debug)
        std::cerr << " <separator>\n";
      pop_up->AppendMenu(MF_SEPARATOR);
    }
    else if (menu[i].is_command())
    {
      if (debug)
        std::cerr << " <command>\n";
      int the_menu_id = ID_MENU_ITEMS + item_count++;
      std::string the_menu_name = menu[i].name;

      // Add menu accelerators:
      if (menu[i].short_cut.mod != vgui_MODIFIER_NULL || menu[i].short_cut.key != vgui_KEY_NULL)
        add_menu_accelerator(menu[i], the_menu_id, the_menu_name);

      pop_up->AppendMenu(MF_STRING, the_menu_id, menu[i].name.c_str());
      // Add to our callback list, the associated callback function pointer
      vgui_command * cmnd = static_cast<vgui_command *>(menu[i].cmnd.as_pointer());
      callbacks.push_back(cmnd);
    }
    else if (menu[i].is_submenu())
    {
      if (debug)
        std::cerr << " <submenu>\n";
      pop_up->AppendMenu(MF_POPUP, (UINT_PTR)add_submenu(*menu[i].menu), menu[i].name.c_str());
    }
  }
  return pop_up;
}

//: Delete the callback functions from the last popup menu.
void
vgui_mfc_utils::delete_last_popup_menu_callbacks()
{
  // Amir: Delete the callbacks that were created for the last popup menu
  //      Without this step, the menu ids keep increasing everytime
  //      a popup menu is created. However, the menu servicing table has a
  //      predefined range of ids it can handle (see line 32 of vgui_mfc_mainfrm.cxx)
  //      which means that after a while the popup menus just stop functioning.
  //      The local lab jargon for this event is "Menu timeout" :)

  // delete all the callbacks up to the last item count
  for (int i = 0; i < item_count - last_item_count; i++)
    callbacks.pop_back();

  // reset item_count
  item_count = last_item_count;
}
