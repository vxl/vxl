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
  static void load_tableaus()  {bwm_tableau_mgr::instance()->load_tableaus();}
  /*static void load_img_tableau() {bwm_tableau_mgr::instance()->load_img_tableau();}
  static void load_cam_tableau() {bwm_tableau_mgr::instance()->load_cam_tableau();}
  static void load_coin3d_tableau() {bwm_tableau_mgr::instance()->load_coin3d_tableau();}
  static void load_proj2d_tableau() {bwm_tableau_mgr::instance()->load_proj2d_tableau();}
  static void load_lidar_tableau() {bwm_tableau_mgr::instance()->load_lidar_tableau();}*/
  static void remove_tableau() {bwm_tableau_mgr::instance()->remove_tableau();}
  static void exit() {bwm_tableau_mgr::instance()->exit(); }

  // Correspondence Menu Items
  static void mode_corr() { bwm_tableau_mgr::instance()->mode_corr(); }
  static void rec_corr() { bwm_tableau_mgr::instance()->rec_corr(); }
  static void save_corr() { bwm_tableau_mgr::instance()->save_corr(); }
  static void delete_last_corr() { bwm_tableau_mgr::instance()->delete_last_corr(); }
  static void delete_corr() { bwm_tableau_mgr::instance()->delete_corr(); }
};

#endif
