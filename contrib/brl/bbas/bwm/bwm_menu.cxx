#include "bwm_menu.h"
#include "bwm_command.h"

vgui_menu bwm_menu::add_to_menu (vgui_menu& top_menu)
{
  vgui_menu load_menu;
  load_menu.add("Load From File..." , load_tableaus);

  MENU_LOAD_TABLEAU("Load Image Tableau...", "bwm_tableau_img", load_menu);
  MENU_LOAD_TABLEAU("Load Camera Tableau...", "bwm_tableau_cam", load_menu);
  MENU_LOAD_TABLEAU("Load 3D Tableau...", "bwm_tableau_coin3d", load_menu);
  MENU_LOAD_TABLEAU("Load Proj2D Tableau...", "bwm_tableau_proj2d", load_menu);
  MENU_LOAD_TABLEAU("Load LIDAR Tableau...", "bwm_tableau_lidar", load_menu);

  load_menu.add("Remove Selected Tableau..." , remove_tableau);
  load_menu.add("Exit..." , exit);
  top_menu.add("FILE   ", load_menu);

  vgui_menu corr_group_menu;
  MENU_ADD_PROCESS_NAME("Correspondence Mode", "corr_mode", corr_group_menu);
  MENU_ADD_PROCESS_NAME("Record Correspondences", "rec_corr", corr_group_menu);
  MENU_ADD_PROCESS_NAME("Save Correspondences", "save_corr", corr_group_menu);
  MENU_ADD_PROCESS_NAME("Delete Last Correspondence", "del_last_corr", corr_group_menu);
  MENU_ADD_PROCESS_NAME("Delete All Correspondences", "del_corr", corr_group_menu);
  top_menu.add("PROCESSES ", corr_group_menu);
  return top_menu;
}
