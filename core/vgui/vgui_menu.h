#ifndef vgui_menu_h_
#define vgui_menu_h_
// .NAME vgui_menu
// .INCLUDE vgui/vgui_menu.h
// .FILE vgui_menu.cxx
// @author fsm@robots.ox.ac.uk

#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_iosfwd.h>
#include <vbl/vbl_smart_ptr.h>
#include <vgui/vgui_key.h>
#include <vgui/vgui_modifier.h>
class vgui_menu;
struct vgui_command;

// These menu description structures are designed to work for both
// pop-up (modal) menus and stay-up (non-modal) menus. They can be
// passed by value.

typedef void (*vgui_menu_callback)(void const* client_data);
typedef void (*vgui_menu_callback_no_client_data)();

// Definition of vgui_menu::item = vgui_menu_item.
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
struct vgui_menu_item
{
  vgui_menu_item();
  vgui_menu_item(vgui_menu_item const &);
  ~vgui_menu_item();

  // name of item. "" for separators.
  vcl_string name;

  // pointer to the command to be executed. non-zero for command items only.
  vbl_smart_ptr<vgui_command> cmnd;

  // pointer to a submenu. the object pointed to is owned (ie managed) by
  // the menu. non-zero for submenu items only.
  vgui_menu *menu;

  // bool flag set for toggle buttons.
  bool is_toggle;

  // description of keyboard shortcut (makes no sense for separators).
  struct invocation {
    vgui_modifier mod;
    vgui_key key;
  } short_cut;

  // use these methods to discover what kind of item you have :
  bool is_command()       const { return name!="" &&  (bool)cmnd && (menu == 0) && !(bool)is_toggle; }
  bool is_submenu()       const { return name!="" && !(bool)cmnd && (menu != 0) && !(bool)is_toggle; }
  bool is_toggle_button() const { return name!="" &&  (bool)cmnd && (menu == 0) &&  (bool)is_toggle; }
  bool is_separator()     const { return name=="" && !(bool)cmnd && (menu == 0) && !(bool)is_toggle; }
};

class vgui_menu {
public:
  vgui_menu();
  vgui_menu(vgui_menu const &);
  void operator=(vgui_menu const &);
  ~vgui_menu();

  // Building the menu.
  // NB 1. empty strings are not acceptable names.
  // NB 2. make sure the signature of your callback
  //       function is correct, or the function pointer
  //       will get cast to bool and the menu item
  //       treated as a toggle button.
  //     ESPECIALLY: REMOVE DUMMY "const void *" ARGUMENT !  (PVr, Apr.2002)
  //      --> look for VC++ warning C4305:
  //      'argument' : truncation from 'void (__cdecl *)(const void *)' to 'bool'

  // command
  void add(vcl_string const &,
           vgui_command *c,
           vgui_key key =vgui_KEY_NULL,
           vgui_modifier modifiers =vgui_MODIFIER_NULL);
  // callback function.
  void add(vcl_string const &,
           vgui_menu_callback_no_client_data f,
           vgui_key key =vgui_KEY_NULL,
           vgui_modifier modifiers =vgui_MODIFIER_NULL);
  // callback function with client data.
  void add(vcl_string const &,
           vgui_menu_callback f,
           void const *client_data,
           vgui_key key =vgui_KEY_NULL,
           vgui_modifier modifiers =vgui_MODIFIER_NULL);
  // a submenu
  void add(vcl_string const &,
           vgui_menu const &,
           vgui_key key =vgui_KEY_NULL,
           vgui_modifier modifiers =vgui_MODIFIER_NULL);
  // a toggle button
#if 1 // see above comment.
  void add(vcl_string const &,
           bool initial,
           vgui_key key =vgui_KEY_NULL,
           vgui_modifier modifiers =vgui_MODIFIER_NULL);
#endif
  void separator();        // a separator
  void include(vgui_menu const & that); // add 'that' to end of *this.
  void clear();

  unsigned size() const; // number of items in menu

  vgui_menu_item const & operator[](unsigned i) const;

private:
  vcl_vector<vgui_menu_item> items;
};

vcl_ostream & operator<<(vcl_ostream &,vgui_menu const &);

//--------------------------------------------------------------------------------

#endif // vgui_menu_h_
