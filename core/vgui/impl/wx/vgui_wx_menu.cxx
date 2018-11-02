// This is core/vgui/impl/wx/vgui_wx_menu.cxx
#include <cctype>
#include "vgui_wx_menu.h"
//=========================================================================
//:
// \file
// \brief  wxWidgets implementation of vgui_menu.
//
// See vgui_wx_menu.h for details.
//=========================================================================

#include <vgui/vgui_command.h>

#include <wx/menu.h>

#ifndef wxCommandEventHandler        // wxWidgets-2.5.3 doesn't define this
#define wxCommandEventHandler(func) \
    (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxCommandEventFunction, &func)
#endif

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

//-------------------------------------------------------------------------
// Private helpers - declarations.
//-------------------------------------------------------------------------
namespace
{
  wxMenuBar* strip_extra_separators(wxMenuBar* menubar);
  wxMenu*    strip_extra_separators(wxMenu* menu);
}

//-------------------------------------------------------------------------
// vgui_wx_menu implementation - construction & destruction.
//-------------------------------------------------------------------------
constexpr int vgui_wx_menu::MENU_ID_OFFSET = 1000;

vgui_wx_menu::vgui_wx_menu(void)
  : item_count_(-1)
{
}

vgui_wx_menu::~vgui_wx_menu(void)
{
  disconnect_handlers();

  // ***** doesn't this get destroyed by the parent window?
  //delete menu_;
}

//-------------------------------------------------------------------------
// vgui_wx_menu implementation.
//-------------------------------------------------------------------------
BEGIN_EVENT_TABLE(vgui_wx_menu, wxEvtHandler)
END_EVENT_TABLE()

//: Create a wxMenuBar that maps to the vgui_menu.
wxMenuBar* vgui_wx_menu::create_wx_menubar(const vgui_menu& menu)
{
  disconnect_handlers();

  wxMenuBar* menubar = new wxMenuBar;

  // ***** do i need to reset the item_count_?
  for (unsigned int i = 0; i < menu.size(); i++)
  {
    if (menu[i].is_separator())
    {
      assert(false);
    }
    else if (menu[i].is_command())
    {
#ifdef DEBUG
      std::cerr << __FILE__ ":command as top level menu item!\n";
#endif
      // create a submenu and add this top-level command to it
      vgui_menu submenu;
      submenu.add(menu[i].name,
                  menu[i].cmnd,
                  menu[i].short_cut.key,
                  menu[i].short_cut.mod);

      menubar->Append(create_wx_submenu(submenu), wxString(menu[i].name.c_str(),wxConvUTF8));
    }
    else if (menu[i].is_submenu())
    {
      menubar->Append(
        create_wx_submenu(*menu[i].menu), wxString(menu[i].name.c_str(),wxConvUTF8));
    }
  }

  return strip_extra_separators(menubar);
}

//: Create a wxMenu that maps to the vgui_menu.
wxMenu* vgui_wx_menu::create_wx_menu(const vgui_menu& menu)
{
  disconnect_handlers();
  return strip_extra_separators(create_wx_submenu(menu));
}

//: Helper class that actually builds the mapped menus.
wxMenu* vgui_wx_menu::create_wx_submenu(const vgui_menu& menu)
{
  wxMenu* popup = new wxMenu;

  for (unsigned int i = 0; i < menu.size(); i++)
  {
    const int menu_id = MENU_ID_OFFSET + ++item_count_;

    if (menu[i].is_separator())
    {
      popup->AppendSeparator();
    }
    else if (menu[i].is_command())
    {
      // add menu accelerators
      std::string menu_item = menu[i].name
                           + create_accelerator_string(menu[i]);

      popup->Append(menu_id, wxString(menu_item.c_str(),wxConvUTF8));

      // save the handle
      handlers_[menu_id] = static_cast<vgui_command_sptr>(menu[i].cmnd);

      // connect the event handler
      Connect(menu_id, wxEVT_COMMAND_MENU_SELECTED,
              wxCommandEventHandler(vgui_wx_menu::on_command));
    }
    else if (menu[i].is_submenu())
    {
      popup->Append(
        menu_id,
        wxString(menu[i].name.c_str(),wxConvUTF8),
        strip_extra_separators(create_wx_submenu(*menu[i].menu)));
    }
  }

  return popup;
}

//: Create the accelerator substring to add to the menu item name.
std::string
vgui_wx_menu::create_accelerator_string(const vgui_menu_item& item) const
{
  if ( item.short_cut.key == vgui_KEY_NULL )
  {
    return std::string("");
  }

  std::string accelerator("\t");

  // ***** taken from mfc impl, but what about combinations??
  if (item.short_cut.mod == vgui_CTRL)
  {
    accelerator += std::string("Ctrl+");
  }
  else if (item.short_cut.mod == vgui_SHIFT)
  {
    accelerator += std::string("Shift+");
  }
  else if (item.short_cut.mod == vgui_ALT)
  {
    accelerator += std::string("Alt+");
  }

  accelerator += std::toupper(item.short_cut.key);

  return accelerator;
}

//: Disconnect the event handlers from the event table.
void vgui_wx_menu::disconnect_handlers()
{
  std::map<int,vgui_command_sptr>::const_iterator iter = handlers_.begin();
  for (; iter != handlers_.end(); iter++)
  {
    Disconnect(iter->first, wxEVT_COMMAND_MENU_SELECTED);
  }
  handlers_.clear();
}

//: The event handler that delegates the call to the correct command.
void vgui_wx_menu::on_command(wxCommandEvent& event)
{
  handlers_[event.GetId()]->execute();
}

//-------------------------------------------------------------------------
// Private helpers - declarations.
//-------------------------------------------------------------------------
namespace
{
  wxMenuBar* strip_extra_separators(wxMenuBar* menubar)
  {
    assert(menubar);

    for (unsigned int i = 0; i < menubar->GetMenuCount(); i++)
    {
      strip_extra_separators(menubar->GetMenu(i));
    }

    return menubar;
  }

  wxMenu* strip_extra_separators(wxMenu* menu)
  {
    assert(menu);

    wxMenuItemList::Node* node = menu->GetMenuItems().GetLast();
    while (node)
    {
      if (node->GetData()->IsSeparator())
      {
        if ( !node->GetNext()      ||                // it's the last item
             !node->GetPrevious()  ||                // it's the first item
              node->GetPrevious()->GetData()->IsSeparator()) // it's double
        {
          menu->Delete(node->GetData());
          node = menu->GetMenuItems().GetLast();
        }
      }
      node = node->GetPrevious();
    }

    return menu;
  }
} // unnamed namespace
