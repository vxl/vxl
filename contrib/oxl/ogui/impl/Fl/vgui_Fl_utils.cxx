// This is oxl/vgui/impl/Fl/vgui_Fl_utils.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   28 Oct 99
//-----------------------------------------------------------------------------

#include "vgui_Fl_utils.h"

#include <vcl_iostream.h>

#include <FL/Fl_Menu_.H>
#include <FL/Fl_Menu_Item.H>

#include <vgui/vgui_command.h>
#include <vgui/vgui_menu.h>

static bool debug = false;

static void execute_command(Fl_Menu_*, void* data) {
  vgui_command *cmnd = static_cast<vgui_command*>(data);
  if (cmnd) {
    cmnd->execute();
  }
  else
    vcl_cerr << __FILE__ " : null command\n";
}


Fl_Menu_Item* vgui_Fl_utils::create_submenu(const vgui_menu& menu) {
  // create a new block of memory for the subitems
  // don't allocate memory for the separators as in FLTK
  // they are a flag on the previous menu item
  int menu_size_noseps = 0;
  for (unsigned i=0;i<menu.size();i++) {
    if (!menu[i].is_separator()) menu_size_noseps++;
  }

  Fl_Menu_Item *submenu = new Fl_Menu_Item[menu_size_noseps+1];

  int count = 0;

  for (unsigned i=0;i<menu.size();i++) {

    if (menu[i].is_separator()) {
      if (debug) vcl_cerr << " <separator>\n";
      if (count != 0) {
        Fl_Menu_Item *prev_item = submenu+count-1;
        prev_item->flags |= FL_MENU_DIVIDER;
      }
      continue;
    }


    Fl_Menu_Item *this_item = submenu+count;
    count++;
    if (debug) vcl_cerr << "adding " << this_item->text;
    vcl_string *namestr = new vcl_string;
    *namestr = menu[i].name;
    this_item->label(namestr->c_str());
    this_item->labeltype_ = this_item->labelfont_ = 0;
    this_item->labelsize_ = this_item->labelcolor_ = 0;
    this_item->flags = 0;
    this_item->shortcut_ = 0;


    if (menu[i].is_command()) {
      if (debug) vcl_cerr << " <command>\n";
      this_item->callback((Fl_Callback*)execute_command, (void*)menu[i].cmnd.as_pointer());
    }
    else if (menu[i].is_submenu()) {
      if (debug) vcl_cerr << " <submenu>\n";
      this_item->flags = FL_SUBMENU_POINTER;
      this_item->callback_ = 0;
      add_submenu(this_item, *menu[i].menu);
    }
  }

  Fl_Menu_Item *this_item = submenu+menu.size();
  this_item->text = 0;

  return submenu;
}


void vgui_Fl_utils::add_submenu(Fl_Menu_Item* menu_item, const vgui_menu& menu) {

  // create the submenu
  Fl_Menu_Item *submenu = create_submenu(menu);

  // attach the submenu to the menu_item
  menu_item->user_data((void*)submenu);
}


void vgui_Fl_utils::set_menu(Fl_Menu_* widget, const vgui_menu& menu) {
  if (debug)
    vcl_cerr << "vgui_Fl_utils::set_menu\n" << menu << vcl_endl;

  if (menu.size() == 0)
    widget->menu(0); // this is to disable the popup menu. FIXME

  // create the submenu
  Fl_Menu_Item *submenu = create_submenu(menu);

  // attach the submenu to the menu widget
  widget->menu(submenu);

  // print em
  const Fl_Menu_Item* m;
  for (m=widget->menu(); m->text; m = m->next())
    if (debug) vcl_cerr << m->text << vcl_endl;
}
