// This is core/vgui/impl/mfc/vgui_mfc_utils.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
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
// \endverbatim

#include "vgui_mfc_utils.h"
#include <vgui/vgui_command.h>
#include <vcl_iostream.h>
static bool debug = false;
vgui_mfc_utils *vgui_mfc_utils::instance_ = NULL;


//: Called within message service routine of vgui_mfc_mainfrm.
void vgui_mfc_utils::menu_dispatcher(UINT nID)
{
  // Make sure nID is in the relevant range
  ASSERT(nID>=ID_MENU_ITEMS && int(nID)<ID_MENU_ITEMS+item_count);
  // Call the callback function associated with the menu item
  callbacks[nID-ID_MENU_ITEMS]->execute();
}

//: (Create if necessary and) return singleton instance of this class.
vgui_mfc_utils *vgui_mfc_utils::instance()
{
  if (!instance_)
    instance_= new vgui_mfc_utils;
  return instance_;
}

//: Constructor.
vgui_mfc_utils::vgui_mfc_utils()
{
  item_count = 0;
}

//: Destructor.
vgui_mfc_utils::~vgui_mfc_utils()
{
  for (unsigned i=0; i<menus_to_be_deleted.size(); i++)
    delete menus_to_be_deleted[i];
}

//: Add keyboard shortcut for this menu item to our accelerator table (accels).
//  Also add text to the_menu_name for the shortcut.
void vgui_mfc_utils::add_menu_accelerator(const vgui_menu_item menu_item, const WORD function_id, vcl_string& the_menu_name)
{
  ACCEL acc;
  acc.cmd = function_id;
  the_menu_name += vcl_string("\t");  // tab to right hand side of label
  if (menu_item.short_cut.mod == vgui_SHIFT)
  {
    acc.fVirt = FSHIFT|FVIRTKEY;  // you can't use modifiers unless you use virtual key codes
                                  // hence the need for FVIRTKEY here.
    the_menu_name += vcl_string("Shift+");
  }
  else if (menu_item.short_cut.mod == vgui_CTRL)
  {
    acc.fVirt = FCONTROL|FVIRTKEY;
    the_menu_name += vcl_string("Ctrl+");
  }
  else if (menu_item.short_cut.mod == vgui_ALT)
  {
    acc.fVirt = FALT|FVIRTKEY;
    the_menu_name += vcl_string("Alt+");
  }
  else // we are going to give a virtual key code, even if there is no modifier:
    acc.fVirt = FVIRTKEY;
  // VkKeyScan converts the key code from ASCII to a virtual key code:
  acc.key = VkKeyScan(menu_item.short_cut.key);
  the_menu_name += menu_item.short_cut.key;
  accels.push_back(acc);
}

//: Create a MFC sub-menu from the given vgui_menu.
HMENU vgui_mfc_utils::add_submenu(const vgui_menu& menu)
{
  CMenu *popdown_menu;

  // Create a new menu
  popdown_menu = new CMenu();
  menus_to_be_deleted.push_back(popdown_menu);
  popdown_menu->CreatePopupMenu();

  for (unsigned i=0;i<menu.size();i++)
  {
    if (menu[i].is_separator())
    {
      if (debug) vcl_cerr << " <separator>\n";
        popdown_menu->AppendMenu(MF_SEPARATOR);
    }
    else if (menu[i].is_command())
    {
      if (debug) vcl_cerr << " <command>\n";
      int the_menu_id = ID_MENU_ITEMS+item_count++;
      vcl_string the_menu_name = menu[i].name;

      // Add menu accelerators:
      if (menu[i].short_cut.mod!=vgui_MODIFIER_NULL ||
          menu[i].short_cut.key!=vgui_KEY_NULL)
         add_menu_accelerator(menu[i], the_menu_id, the_menu_name);

      popdown_menu->AppendMenu(MF_STRING,the_menu_id,the_menu_name.c_str());
      // Add to our callback list, the associated callback function pointer
      vgui_command *cmnd = static_cast<vgui_command *>(menu[i].cmnd.as_pointer());
      callbacks.push_back(cmnd);
    }
    else if (menu[i].is_submenu()) {
      if (debug) vcl_cerr << " <submenu>\n";
      popdown_menu->AppendMenu(MF_POPUP,(UINT)add_submenu(*menu[i].menu),menu[i].name.c_str());
    }
  }
  return popdown_menu->GetSafeHmenu();
}

//: Sets the menu of the application window
void vgui_mfc_utils::set_menu(const vgui_menu& menu)
{
  CMenu *menu_bar;

  // Get the application thread (this is the single CWinApp object associated with the application):
  CWinApp *theapp = AfxGetApp();
  // Obtain the main window associated with it
  CWnd *window = theapp->GetMainWnd();
  // See whether the application window already has a menu associated with it
  if (window->GetMenu() == NULL)
  {
    menu_bar = new CMenu();
    menus_to_be_deleted.push_back(menu_bar);
    menu_bar->CreateMenu();
    window->SetMenu(menu_bar);
  }
  for (unsigned i=0;i<menu.size();i++)
  {
    if (menu[i].is_separator())
    {
      if (debug) vcl_cerr << " <separator>\n";
      menu_bar->AppendMenu(MF_SEPARATOR);
    }
    else if (menu[i].is_command())
    {
      int the_menu_id = ID_MENU_ITEMS+item_count++;
      vcl_string the_menu_name = menu[i].name;

      // Add menu accelerators
      if (menu[i].short_cut.mod!=vgui_MODIFIER_NULL ||
          menu[i].short_cut.key!=vgui_KEY_NULL)
          add_menu_accelerator(menu[i], the_menu_id, the_menu_name);

      if (debug) vcl_cerr << " <command> " << menu[i].name << vcl_endl;
      menu_bar->AppendMenu(MF_STRING|MF_ENABLED,the_menu_id,
                           the_menu_name.c_str());
    }
    else if (menu[i].is_submenu())
    {
      if (debug) vcl_cerr << " <submenu> " << menu[i].name << vcl_endl;
      menu_bar->AppendMenu(MF_POPUP,(UINT)add_submenu(*menu[i].menu),menu[i].name.c_str());
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
CMenu *vgui_mfc_utils::set_popup_menu(const vgui_menu &menu)
{
  CMenu *pop_up;

  // Create a new menu
  pop_up = new CMenu();
  pop_up->CreatePopupMenu();

  for (unsigned i=0;i<menu.size();i++)
  {
    if (menu[i].is_separator())
    {
      if (debug) vcl_cerr << " <separator>\n";
          pop_up->AppendMenu(MF_SEPARATOR);
    }
    else if (menu[i].is_command())
    {
      if (debug) vcl_cerr << " <command>\n";
      int the_menu_id = ID_MENU_ITEMS+item_count++;
      vcl_string the_menu_name = menu[i].name;

      // Add menu accelerators:
      if (menu[i].short_cut.mod!=vgui_MODIFIER_NULL ||
          menu[i].short_cut.key!=vgui_KEY_NULL)
          add_menu_accelerator(menu[i], the_menu_id, the_menu_name);

      pop_up->AppendMenu(MF_STRING,the_menu_id,menu[i].name.c_str());
      // Add to our callback list, the associated callback function pointer
      vgui_command *cmnd = static_cast<vgui_command *>(menu[i].cmnd.as_pointer());
      callbacks.push_back(cmnd);
    }
    else if (menu[i].is_submenu())
    {
      if (debug) vcl_cerr << " <submenu>\n";
        pop_up->AppendMenu(MF_POPUP,(UINT)add_submenu(*menu[i].menu),menu[i].name.c_str());
    }
  }
  return pop_up;
}
