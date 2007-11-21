#ifndef bwm_menu_h_
#define bwm_menu_h_ 

#include "bwm_tableau_mgr.h"
#include <vgui/vgui_menu.h>
#include <vgui/vgui_dialog.h>

class bwm_menu
{
public:

  bwm_menu() {}

  virtual ~bwm_menu() { }

  vgui_menu add_to_menu (vgui_menu& menu);

  // FILE Menu Items
  static void create_site()  {bwm_tableau_mgr::instance()->create_site();}
  static void edit_site()  {bwm_tableau_mgr::instance()->edit_site();}
  static void load_tableaus()  {bwm_tableau_mgr::instance()->load_tableaus();}
  static void remove_tableau() {bwm_tableau_mgr::instance()->remove_tableau();}
  static void exit() {bwm_tableau_mgr::instance()->exit(); }

};

#endif
