#ifndef bwm_menu_h_
#define bwm_menu_h_

#include "bwm_tableau_mgr.h"
#include "bwm_site_mgr.h"
#include "bwm_world.h"

#include <vgui/vgui_menu.h>
#include <vgui/vgui_dialog.h>

class bwm_menu
{
 public:

  bwm_menu() {}

  virtual ~bwm_menu() { }

  vgui_menu add_to_menu (vgui_menu& menu);

  // FILE Menu Items
  static void create_site()  {bwm_site_mgr::instance()->create_site();}
  static void edit_site()  {bwm_site_mgr::instance()->edit_site();}
  static void load_site()  {bwm_site_mgr::instance()->load_site();}
  static void save_site() { bwm_site_mgr::instance()->save_site();}
  static void load_video_site()
    {bwm_site_mgr::instance()->load_video_site();}
  static void save_video_site() 
    { bwm_site_mgr::instance()->save_video_site();}
  static void remove_tableau() {bwm_tableau_mgr::instance()->remove_tableau();}
  static void save_ply() { bwm_world::instance()->save_ply(); }
  static void save_gml() { bwm_world::instance()->save_gml(); }
  static void save_kml() { bwm_world::instance()->save_kml(); }
  static void save_kml_collada() { bwm_world::instance()->save_kml_collada(); }
  static void save_x3d() { bwm_world::instance()->save_x3d(); }
  static void exit() {bwm_tableau_mgr::instance()->exit(); }
  static void load_video_tableau(){bwm_site_mgr::instance()->load_video_tableau(); }
};

#endif
