#include "bwm_menu.h"

vgui_menu bwm_menu::add_to_menu (vgui_menu& top_menu)
{
  vgui_menu menu;
  menu.add("Load From File..." , load_tableaus);
  menu.add("Load Image Tableau...", load_img_tableau);
  menu.add("Load Camera Tableau..." , load_cam_tableau);
  menu.add("Load Coin3D Tableau..." , load_coin3d_tableau);
  menu.add("Load Proj2D Tableau..." , load_proj2d_tableau);
  menu.add("Load LIDAR Tableau..." , load_lidar_tableau);
  menu.add("Remove Selected Tableau..." , remove_tableau);
  menu.add("Exit..." , exit);
  top_menu.add("FILE   ", menu);


  vgui_menu edit;
  edit.add("Correspondence Mode", mode_corr);
  edit.add("Record Correspondences", rec_corr, vgui_key('r'));
  edit.add("Save Correspondences", save_corr);
  edit.add("Delete Last Correspondence", delete_last_corr);
  edit.add("Delete All", delete_corr);
  top_menu.add("CORR    ", edit);
  return top_menu;
}
