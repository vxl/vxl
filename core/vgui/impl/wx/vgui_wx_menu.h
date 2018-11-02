// This is core/vgui/impl/wx/vgui_wx_menu.h
#ifndef vgui_wx_menu_h_
#define vgui_wx_menu_h_
//=========================================================================
//:
// \file
// \brief  wxWidgets mapping of the vgui_menu.
// \author Miguel A. Figueroa-Villanueva (miguelfv)
//
// \verbatim
//  Modifications
//   04/18/2006 - File created. (miguelfv)
// \endverbatim
//=========================================================================

#include <map>
#include <vgui/vgui_menu.h>

#include <wx/event.h>
class wxMenu;
class wxMenuBar;

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//-------------------------------------------------------------------------
//: The wxWidgets mapping of the vgui_menu.
//
// Maps a vgui_menu object to the equivalent wxMenu and wxMenuBar.
//-------------------------------------------------------------------------
class vgui_wx_menu : public wxEvtHandler
{
  DECLARE_EVENT_TABLE()

public:
  vgui_wx_menu();
  virtual ~vgui_wx_menu();

  //: Create a wxMenuBar that maps to the vgui_menu.
  wxMenuBar* create_wx_menubar(const vgui_menu& menu);

  //: Create a wxMenu that maps to the vgui_menu.
  wxMenu* create_wx_menu(const vgui_menu& menu);

private:
  //: Helper class that actually builds the mapped menus.
  wxMenu* create_wx_submenu(const vgui_menu& menu);

  //: Create the accelerator substring to add to the menu item name.
  std::string create_accelerator_string(const vgui_menu_item& item) const;

  //: Disconnect the Event Handlers from the event table.
  void disconnect_handlers();

  //: The event handler that delegates the call to the correct command.
  void on_command(wxCommandEvent& event);

  static const int MENU_ID_OFFSET;
  int item_count_;

  std::map<int,vgui_command_sptr> handlers_;
};

#endif // vgui_wx_menu_h_
