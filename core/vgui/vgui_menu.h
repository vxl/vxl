// This is core/vgui/vgui_menu.h
#ifndef vgui_menu_h_
#define vgui_menu_h_
//:
// \file
// \brief  Representation of menus and menu items for pop-up and menubar menus.
// \author fsm
//
//  Contains classes  vgui_menu_item  vgui_menu
//
//  These menu description structures are designed to work for both
//  pop-up (modal) menus and stay-up (non-modal) menus. They can be
//  passed by value.
//
// \verbatim
//  Modifications
//   07-Aug-1999 K.Y.McGaul - Added Doxygen style comments.
// \endverbatim

#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_iosfwd.h>
#include <vbl/vbl_smart_ptr.h>
#include <vgui/vgui_key.h>
#include <vgui/vgui_modifier.h>
#include <vgui/vgui_command_sptr.h>

class vgui_menu;

typedef void (*vgui_menu_callback)(void const* client_data);
typedef void (*vgui_menu_callback_no_client_data)();

//: Representation of an item on a menu.
//
// Definition of vgui_menu::item = vgui_menu_item.
// \verbatim
// Each item is either a command :
// . name
// . action (command)
// . invocation (eg. shortcuts)
// or a submenu :
// . name
// . another menu
// . invocation
// or a toggle button :
// . name
// . toggle_command
// . invocation
// or a separator :
// . -----------
// \endverbatim
struct vgui_menu_item
{
  //: Constructor - create a default menu item.
  vgui_menu_item();

  //: Constructor - create menu item same as given item.
  vgui_menu_item(vgui_menu_item const &);

  //: Destructor
  ~vgui_menu_item();

  //: Name of item, "" for separators.
  vcl_string name;

  //: Pointer to the command to be executed.
  //  Non-zero for command items only.
  vgui_command_sptr cmnd;

  //: Pointer to a submenu.
  //  The object pointed to is owned (ie managed) by
  //  the menu. non-zero for submenu items only.
  vgui_menu *menu;

  //: Description of keyboard shortcut (makes no sense for separators).
  struct invocation {
    vgui_modifier mod;
    vgui_key key;
  } short_cut;

  //: Return true if the item is a command (item which performs an action).
  bool is_command()       const { return name!="" &&  (bool)cmnd && (menu == 0) && !is_toggle_button(); }

  //: Returns true if the item is a sub-menu.
  bool is_submenu()       const { return name!="" && !(bool)cmnd && (menu != 0) && !is_toggle_button(); }

  //: Returns true if the item is a toggle button.
  bool is_toggle_button() const;

  //: Returns true if the item is a separator.
  bool is_separator()     const { return name=="" && !(bool)cmnd && (menu == 0) && !is_toggle_button(); }
};

//: Representation of a menu for both pop-up and menubar menus.
//
// Building the menu:
//
// NB. empty strings are not acceptable names.
class vgui_menu
{
  vcl_vector<vgui_menu_item> items;

 public:
  //: Constructor - creates an empty menu.
  vgui_menu() {}

  //: Constructor - creates a menu same as the given menu.
  vgui_menu(vgui_menu const &);

  //: Make this menu equal to the given menu.
  void operator=(vgui_menu const &);

  //: Destructor - clears the menu.
  ~vgui_menu() { clear(); }

  //: Add given command to this menu.
  void add(vcl_string const &,
           vgui_command_sptr c,
           vgui_key key =vgui_KEY_NULL,
           vgui_modifier modifiers =vgui_MODIFIER_NULL);

  //: Add given callback function to this menu.
  void add(vcl_string const &,
           vgui_menu_callback_no_client_data f,
           vgui_key key =vgui_KEY_NULL,
           vgui_modifier modifiers =vgui_MODIFIER_NULL);

  //: Add callback function with client data to this menu.
  void add(vcl_string const &,
           vgui_menu_callback f,
           void const *client_data,
           vgui_key key =vgui_KEY_NULL,
           vgui_modifier modifiers =vgui_MODIFIER_NULL);

  //: Add given submenu to this menu.
  void add(vcl_string const &,
           vgui_menu const &,
           vgui_key key =vgui_KEY_NULL,
           vgui_modifier modifiers =vgui_MODIFIER_NULL);

  //: Add separator to this menu.
  void separator();        // a separator

  //: Add the given menu to the end of this menu.
  void include(vgui_menu const & that); // add 'that' to end of *this.

  //: Empty this menu.
  void clear() { items.clear(); }

  //: Return the number of items in this menu
  unsigned size() const { return items.size(); }

  //: Get the menu item using the [] index operator.
  vgui_menu_item const & operator[](unsigned i) const { return items[i]; }
};

vcl_ostream & operator<<(vcl_ostream &,vgui_menu const &);

//--------------------------------------------------------------------------------

#endif // vgui_menu_h_
