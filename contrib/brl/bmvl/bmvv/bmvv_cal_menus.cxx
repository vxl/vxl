#include <vgui/vgui.h>
#include <vgui/vgui_key.h>
#include <vgui/vgui_menu.h>
#include <bmvv/bmvv_cal_manager.h>
#include <bmvv/bmvv_cal_menus.h>

//Static munu callback functions

void bmvv_cal_menus::quit_callback()
{
  bmvv_cal_manager::instance()->quit();
}

void bmvv_cal_menus::load_image_callback()
{
  bmvv_cal_manager::instance()->load_image();
}

void bmvv_cal_menus::adjust_contrast_callback()
{
  bmvv_cal_manager::instance()->adjust_contrast();
}


void bmvv_cal_menus::clear_display_callback()
{
  bmvv_cal_manager::instance()->clear_display();
}

void bmvv_cal_menus::clear_selected_callback()
{
  bmvv_cal_manager::instance()->clear_selected();
}

void bmvv_cal_menus::compute_ransac_homography_callback()
{
  bmvv_cal_manager::instance()->compute_ransac_homography();
}

void bmvv_cal_menus::load_image_and_camera_callback()
{
  bmvv_cal_manager::instance()->load_image_and_camera();
}

void bmvv_cal_menus::save_camera_callback()
{
  bmvv_cal_manager::instance()->save_camera();
}

void bmvv_cal_menus::load_corrs_callback()
{
  bmvv_cal_manager::instance()->load_corrs();
}

void bmvv_cal_menus::save_corrs_callback()
{
  bmvv_cal_manager::instance()->save_corrs();
}

void bmvv_cal_menus::load_world_callback()
{
  bmvv_cal_manager::instance()->load_world();
}

void bmvv_cal_menus::pick_correspondence_callback()
{
  bmvv_cal_manager::instance()->pick_correspondence();
}

void bmvv_cal_menus::remove_correspondence_callback()
{
  bmvv_cal_manager::instance()->remove_correspondence();
}

void bmvv_cal_menus::solve_camera_callback()
{
  bmvv_cal_manager::instance()->solve_camera();
}

//bmvv_cal_menus definition
vgui_menu bmvv_cal_menus::get_menu()
{
  vgui_menu menubar;
  vgui_menu menufile;
  vgui_menu menuview;
  vgui_menu menuedit;
  vgui_menu menucal;
  vgui_menu menudebug;
  //file menu entries
  menufile.add( "Load Image", load_image_callback,(vgui_key)'l', vgui_CTRL);
  menufile.add( "Load Image and Cam", load_image_and_camera_callback);
  menufile.add("Save Camera", save_camera_callback);
  menufile.add( "Load Coors", load_corrs_callback);
  menufile.add( "Save Coors", save_corrs_callback);
  menufile.add( "Load World Points", load_world_callback);
  menufile.add( "Quit", quit_callback,(vgui_key)'q', vgui_CTRL);

  //view menu entries
  menuview.add("Clear Display", clear_display_callback);
  menuview.add("Clear Selected", clear_selected_callback);
  menuview.add("Adjust Contrast", adjust_contrast_callback);

  //edit menu entries
  
//debug menus entries
  
//Calibration menu
  menucal.add("Compute Ransac Homography (RREL)",
               compute_ransac_homography_callback);
  menucal.add("Pick Correspondence", pick_correspondence_callback,(vgui_key)'p');
  menucal.add("Remove Correspondence", remove_correspondence_callback,(vgui_key)'r');
  menucal.add("Solve Camera", solve_camera_callback);
  //Top level menu layout
  menubar.add( "File", menufile);
  menubar.add( "View", menuview);
  menubar.add( "Edit", menuedit);
  menubar.add( "Calib", menucal);
  return menubar;
}

