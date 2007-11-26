#include "bwm_menu.h"
#include "bwm_command_macros.h"

vgui_menu bwm_menu::add_to_menu (vgui_menu& top_menu)
{
  vgui_menu load_menu;
  load_menu.add("Create Site", create_site);
  load_menu.add("Edit Site", edit_site);
  load_menu.add("Load Site..." , load_site);
  load_menu.add("Save Site..." , save_site);

  MENU_LOAD_TABLEAU("Load Image Tableau...", "bwm_tableau_img", load_menu);
  MENU_LOAD_TABLEAU("Load Camera Tableau...", "bwm_tableau_cam", load_menu);
  MENU_LOAD_TABLEAU("Load 3D Tableau...", "bwm_tableau_coin3d", load_menu);
  MENU_LOAD_TABLEAU("Load Proj2D Tableau...", "bwm_tableau_proj2d", load_menu);
  MENU_LOAD_TABLEAU("Load LIDAR Tableau...", "bwm_tableau_lidar", load_menu);

  load_menu.add("Remove Selected Tableau..." , remove_tableau);
  load_menu.add("Exit..." , exit);
  top_menu.add("FILE   ", load_menu);

  vgui_menu process_menu;

  vgui_menu corr_menu;
  MENU_MENU_ADD_PROCESS("correspondence", corr_menu);
  process_menu.add("Correspondences",corr_menu);
  
  top_menu.add("PROCESSES ", process_menu);
  return top_menu;
}
