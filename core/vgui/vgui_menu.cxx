// This is core/vgui/vgui_menu.cxx
#include "vgui_menu.h"
//:
// \file
// \author fsm
// \brief  See vgui_menu.h for a description of this file.

#include <vcl_iostream.h>
#include <vgui/vgui_command.h>

//--------------------------------------------------------------------------------

vgui_menu_item::vgui_menu_item()
  : name("[]")
  , menu(0)
{
  short_cut.mod = vgui_MODIFIER_NULL;
  short_cut.key = vgui_KEY_NULL;
}

vgui_menu_item::vgui_menu_item(vgui_menu_item const &that)
  : name(that.name)
  , cmnd(that.cmnd)
  , menu(that.menu)
  , short_cut(that.short_cut)
{
  if (menu)
    menu = new vgui_menu( *menu ); // make a copy.
}

vgui_menu_item::~vgui_menu_item()
{
  if (menu)
    delete menu;
  menu = 0;
}

bool vgui_menu_item::is_toggle_button() const
{
  // The use of dynamic_cast is forbidden by the VXL
  // guidelines. However, the alternative here is to implement our own
  // RTTI in vgui_command using a virtual function like
  // is_a_toggle_command. However, this is GUI code. It's not
  // performance critical, and mostly requires RTTI and threads and
  // all kinds of things. Let's just use the compiler generated
  // version until we run into a real problem.
  //
  return name!="" &&  (bool)cmnd && (menu == 0) &&  dynamic_cast<vgui_command_toggle*>(cmnd.ptr()) != 0;
}

//--------------------------------------------------------------------------------

vgui_menu::vgui_menu(vgui_menu const &that)
{
  operator=(that);
}

void vgui_menu::operator=(vgui_menu const &that)
{
  if (this != &that) {
    clear();
    this->include(that);
  }
}

//--------------------------------------------------------------------------------

#define im_here do { /* vcl_cerr << __FILE__ " : " << __LINE__ << vcl_endl; */ } while (false)

void vgui_menu::add(vcl_string const &n,
                    vgui_command_sptr c,
                    vgui_key key,
                    vgui_modifier modifiers)
{
  im_here;
  vgui_menu_item i;
  i.name = n;
  i.cmnd = c;
  i.short_cut.key = key;
  i.short_cut.mod = modifiers;
  items.push_back(i);
}

void vgui_menu::add(vcl_string const &n,
                    vgui_menu_callback f,
                    void const* client_data,
                    vgui_key key,
                    vgui_modifier modifiers)
{
  im_here;
  vgui_command* cfunc = new vgui_command_cfunc(f, client_data); //KYM fix for SunPro
  add(n, /* (vgui_command*) */cfunc, key, modifiers);
}

void vgui_menu::add(vcl_string const &n,
                    vgui_menu_callback_no_client_data f,
                    vgui_key key,
                    vgui_modifier modifiers)
{
  im_here;
  vgui_command* cfunc = new vgui_command_cfunc(f);  //KYM fix for SunPro
  add(n, /* (vgui_command*) */cfunc, key, modifiers);
}

void vgui_menu::add(vcl_string const &n,
                    vgui_menu const &m,
                    vgui_key key,
                    vgui_modifier modifiers)
{
  im_here;
  vgui_menu_item i;
  i.name = n;
  i.menu = new vgui_menu(m);
  i.short_cut.key = key;
  i.short_cut.mod = modifiers;
  items.push_back(i);
}

void vgui_menu::separator()
{
  vgui_menu_item i;
  i.name = "";
  items.push_back(i);
}

void vgui_menu::include(vgui_menu const &that)
{
  items.reserve(items.size() + that.items.size());

  for (unsigned i=0; i<that.size(); ++i)
    items.push_back( that.items[i] );
}

//--------------------------------------------------------------------------------

// Helper function for operator<<. Apart from the menu to print and the
// destination stream, the 'pre' argument is the prefix to print on every
// new line (usually spaces for indentation).
// This could be a member function, but that would just clutter
// the interface even more.
static void dump(vgui_menu const &This, vcl_ostream &os, vcl_string const &pre)
{
  for (unsigned i=0;i<This.size();i++) {
    // name
    os << pre << " \"" << This[i].name << "\" ";

    // shortcut
    if (This[i].short_cut.mod & vgui_CTRL)
      os << "CTRL+";
    if (This[i].short_cut.mod & vgui_SHIFT)
      os << "SHIFT+";
    if (This[i].short_cut.mod & vgui_META)
      os << "META+";
    if (This[i].short_cut.mod & vgui_ALT)
      os << "ALT+";
    if (This[i].short_cut.key)
      os << '\'' << char(This[i].short_cut.key) << '\'';
    else
      os << "\'\'"; //"(none)";
    os << ' ';

    // what it does
    if (This[i].is_command())
      os << "command(" << static_cast<void*>(This[i].cmnd.as_pointer()) << ")\n";
    else if (This[i].is_submenu()) {
      os << "submenu:\n";
      dump(* This[i].menu,os,pre+"  ");
    }
    else if (This[i].is_toggle_button()) {
      vgui_command_toggle *c = static_cast<vgui_command_toggle*>(This[i].cmnd.as_pointer());
      os << "toggle (" << (c->state ? "on" : "off") << ")\n";
    }
    else if (This[i].is_separator())
      os << "(----- separator -----)\n";
    else
      os << "[unknown menuitem]\n"; //
  }
}

vcl_ostream &operator<<(vcl_ostream &os, vgui_menu const &m)
{
  dump(m, os, "  ");
  return os;
}

//--------------------------------------------------------------------------------
