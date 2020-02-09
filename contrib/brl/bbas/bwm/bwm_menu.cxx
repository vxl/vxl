#include "bwm_menu.h"
#include "bwm_command_macros.h"
#include "bwm_tableau_mgr.h"
vgui_menu bwm_menu::add_to_menu (vgui_menu& top_menu)
{
  vgui_menu load_menu;
  load_menu.add("Create Site", create_site);
  load_menu.add("Edit Site", edit_site);
  load_menu.add("Load Site..." , load_site);
  load_menu.add("Save Site..." , save_site);
  if(bwm_tableau_mgr::instance()->is_registered("bwm_tableau_video")){
    load_menu.add("Load VideoSite..." , load_video_site);
    load_menu.add("Save VideoSite..." , save_video_site);
  }
  // not typically needed - maybe move to another menu of seldom used utilities
  //load_menu.add("Load Depth Scene..." , load_depth_map_scene);
  //
  MENU_LOAD_TABLEAU("Load Image Tableau...", "bwm_tableau_img", load_menu);
  if(bwm_tableau_mgr::instance()->is_registered("bwm_tableau_video"))
    load_menu.add("Load Video Tableau...", load_video_tableau);
  MENU_LOAD_TABLEAU("Load Camera Tableau...", "bwm_tableau_rat_cam", load_menu);
  MENU_LOAD_TABLEAU("Load Fiducial Tableau...", "bwm_tableau_fiducial", load_menu);
  MENU_LOAD_TABLEAU("Load 3D Tableau...", "bwm_tableau_coin3d", load_menu);
  MENU_LOAD_TABLEAU("Load Proj2D Tableau...", "bwm_tableau_proj2d", load_menu);
  MENU_LOAD_TABLEAU("Load LIDAR Tableau...", "bwm_tableau_lidar", load_menu);

  load_menu.add("Remove Selected Tableau..." , remove_tableau);
  load_menu.add("Zoom to Fit..." , zoom_to_fit);
  load_menu.add("Scroll to Point..." , scroll_to_point);

  load_menu.add("Save adjusted cameras..." , save_cameras);
  load_menu.add("Exit..." , exit);
  top_menu.add("SITE   ", load_menu);

  vgui_menu file_menu;
  file_menu.add("Load shape file (.shp)", load_shape_file);
  file_menu.add("Save (ply)", save_ply);
  file_menu.add("Save (gml)", save_gml);
  file_menu.add("Save (kml)", save_kml);
  file_menu.add("Save (kml collada)", save_kml_collada);
  file_menu.add("Save (x3d)", save_x3d);
  file_menu.add("Save Video World Points (vrml)", save_world_points_vrml);
  file_menu.add("Save Video Cameras (vrml)", save_video_cameras_vrml);
  file_menu.add("Save Video Cams and Points (vrml)", save_video_cams_and_world_pts_vrml);
  file_menu.add("Save World Params", save_world_params);

  top_menu.add("FILE   ", file_menu);

  vgui_menu corr_menu;
  vgui_menu process_menu;
  MENU_MENU_ADD_PROCESS("correspondence", corr_menu);
  process_menu.add("Correspondences",corr_menu);
  top_menu.add("PROCESSES ", process_menu);

  return top_menu;
}
