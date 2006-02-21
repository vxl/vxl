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

void bmvv_cal_menus::save_image_callback()
{
  bmvv_cal_manager::instance()->save_image();
}

void bmvv_cal_menus::adjust_contrast_callback()
{
  bmvv_cal_manager::instance()->adjust_contrast();
}

void bmvv_cal_menus::set_range_params_callback()
{
  bmvv_cal_manager::instance()->set_range_params();
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

void bmvv_cal_menus::save_constraints_callback()
{
  bmvv_cal_manager::instance()->save_constraints();
}


void bmvv_cal_menus::load_world_callback()
{
  bmvv_cal_manager::instance()->load_world();
}

void bmvv_cal_menus::save_world_callback()
{
  bmvv_cal_manager::instance()->save_world();
}

void bmvv_cal_menus::save_world_ply2_callback()
{
  bmvv_cal_manager::instance()->save_world_ply2();
}

void bmvv_cal_menus::read_world_ply2_callback()
{
  bmvv_cal_manager::instance()->read_world_ply2();
}

void bmvv_cal_menus::ply2_to_vrml_callback()
{
  bmvv_cal_manager::instance()->ply2_to_vrml();
}

void bmvv_cal_menus::pick_correspondence_callback()
{
  bmvv_cal_manager::instance()->pick_correspondence();
}

void bmvv_cal_menus::remove_correspondence_callback()
{
  bmvv_cal_manager::instance()->remove_correspondence();
}

void bmvv_cal_menus::pick_vertical_callback()
{
  bmvv_cal_manager::instance()->pick_vertical();
}


void bmvv_cal_menus::solve_camera_callback()
{
  bmvv_cal_manager::instance()->solve_camera();
}

void bmvv_cal_menus::set_identity_camera_callback()
{
  bmvv_cal_manager::instance()->set_identity_camera();
}

void bmvv_cal_menus::map_image_to_world_callback()
{
  bmvv_cal_manager::instance()->map_image_to_world();
}

void bmvv_cal_menus::clear_world_callback()
{
  bmvv_cal_manager::instance()->clear_world();
}

void bmvv_cal_menus::set_height_callback()
{
  bmvv_cal_manager::instance()->set_height();
}

void bmvv_cal_menus::pick_polygon_callback()
{
  bmvv_cal_manager::instance()->pick_polygon();
}

void bmvv_cal_menus::confirm_polygon_callback()
{
  bmvv_cal_manager::instance()->confirm_polygon();
}

void bmvv_cal_menus::create_box_callback()
{
  bmvv_cal_manager::instance()->create_box();
}

//bmvv_cal_menus definition
vgui_menu bmvv_cal_menus::get_menu()
{
  vgui_menu menubar;
  vgui_menu menufile;
  vgui_menu menuview;
  vgui_menu menuedit;
  vgui_menu menucal;
  vgui_menu menumodel;
  vgui_menu menudebug;
  //file menu entries
  menufile.add( "Load Image", load_image_callback,(vgui_key)'l', vgui_CTRL);
  menufile.add( "Save Image", save_image_callback);
  menufile.add( "Load Image and Cam", load_image_and_camera_callback);
  menufile.add("Save Camera", save_camera_callback);
  menufile.add( "Load Coors", load_corrs_callback);
  menufile.add( "Save Coors", save_corrs_callback);
  menufile.add( "Load World ", load_world_callback);
  menufile.add( "Save World ", save_world_callback);
  menufile.add( "Read World Ply2 ", read_world_ply2_callback);
  menufile.add( "Save World Ply2 ", save_world_ply2_callback);
  menufile.add( "Save Constraints", save_constraints_callback);
  menufile.add( "ply2->vrml", ply2_to_vrml_callback);
  menufile.add( "Quit", quit_callback,(vgui_key)'q', vgui_CTRL);

  //view menu entries
  menuview.add("Clear Display", clear_display_callback);
  menuview.add("Clear Selected", clear_selected_callback);
  menuview.add("Adjust Contrast", adjust_contrast_callback);
  menuview.add("Set Range Params", set_range_params_callback);
  menuview.add("Map Image to World", map_image_to_world_callback);

  //edit menu entries
  
//debug menus entries
  
//Calibration menu

  menucal.add("Set Identity Camera",set_identity_camera_callback);
  menucal.add("Compute Ransac Homography (RREL)",
               compute_ransac_homography_callback);
  menucal.add("Pick Correspondence", pick_correspondence_callback,(vgui_key)'p');

  menucal.add("Remove Correspondence", remove_correspondence_callback,(vgui_key)'r');
  menucal.add("Pick vertical", pick_vertical_callback, (vgui_key)'l');
  menucal.add("Solve Camera", solve_camera_callback);
  //modeling menu
  menumodel.add("Clear World", clear_world_callback, (vgui_key)'w');
  menumodel.add("Set Height", set_height_callback, (vgui_key)'z');
  menumodel.add("Draw Polygon", pick_polygon_callback, (vgui_key)'g');
  menumodel.add("Validate Last Polygon", confirm_polygon_callback, (vgui_key)'v');
  menumodel.add("Create Box", create_box_callback, (vgui_key)'b');

  //Top level menu layout
  menubar.add( "File", menufile);
  menubar.add( "View", menuview);
  menubar.add( "Edit", menuedit);
  menubar.add( "Calib", menucal);
  menubar.add( "Model", menumodel);
  return menubar;
}

