#include "bmvv_f_menus.h"
//:
// \file
#include <vgui/vgui.h>
#include <vgui/vgui_key.h>
#include <vgui/vgui_menu.h>
#include <bmvv/bmvv_f_manager.h>

//Static munu callback functions
//Static munu callback functions

void bmvv_f_menus::quit_callback()
{
  bmvv_f_manager::instance()->quit();
}


void bmvv_f_menus::load_image_callback()
{
  bmvv_f_manager::instance()->load_image();
}

void bmvv_f_menus::load_image_and_cam_callback()
{
  bmvv_f_manager::instance()->load_image_and_cam();
}

void bmvv_f_menus::save_image_callback()
{
  bmvv_f_manager::instance()->save_image();
}


void bmvv_f_menus::clear_display_callback()
{
  bmvv_f_manager::instance()->clear_display();
}

void bmvv_f_menus::read_corrs_callback()
{
  bmvv_f_manager::instance()->read_corrs();
}

void bmvv_f_menus::load_world_callback()
{
  bmvv_f_manager::instance()->load_world();
}

void bmvv_f_menus::save_world_callback()
{
  bmvv_f_manager::instance()->save_world();
}

void bmvv_f_menus::reconstruct_world_callback()
{
  bmvv_f_manager::instance()->reconstruct_world();
}

void bmvv_f_menus::save_corrs_callback()
{
  bmvv_f_manager::instance()->save_corrs();
}

void bmvv_f_menus::display_corrs_callback()
{
  bmvv_f_manager::instance()->display_corrs();
}

void bmvv_f_menus::project_world_callback()
{
  bmvv_f_manager::instance()->project_world();
}

void bmvv_f_menus::read_f_matrix_callback()
{
  bmvv_f_manager::instance()->read_f_matrix();
}

void bmvv_f_menus::save_f_matrix_callback()
{
  bmvv_f_manager::instance()->save_f_matrix();
}

void bmvv_f_menus::display_right_epi_lines_callback()
{
  bmvv_f_manager::instance()->display_right_epi_lines();
}

void bmvv_f_menus::display_picked_epi_line_callback()
{
  bmvv_f_manager::instance()->display_picked_epi_line();
}

void bmvv_f_menus::create_correspondence_callback()
{
  bmvv_f_manager::instance()->create_correspondence();
}

void bmvv_f_menus::pick_correspondence_callback()
{
  bmvv_f_manager::instance()->pick_correspondence();
}

void bmvv_f_menus::compute_f_matrix_callback()
{
  bmvv_f_manager::instance()->compute_f_matrix();
}

void bmvv_f_menus::set_range_params_callback()
{
  bmvv_f_manager::instance()->set_range_params();
}

void bmvv_f_menus::intensity_profile_callback()
{
  bmvv_f_manager::instance()->intensity_profile();
}

void bmvv_f_menus::intensity_histogram_callback()
{
  bmvv_f_manager::instance()->intensity_histogram();
}

//bmvv_f_menus definition
vgui_menu bmvv_f_menus::get_menu()
{
  vgui_menu menubar;
  vgui_menu menufile;
  vgui_menu menuview;
  vgui_menu menuedit;

  //file menu entries
  menufile.add( "Quit(CTL-q)", quit_callback,(vgui_key)'q', vgui_CTRL);
  menufile.add( "Load Image(CTL-l)", load_image_callback,(vgui_key)'l', vgui_CTRL);
  menufile.add( "Load Image and Camera", load_image_and_cam_callback);
  menufile.add( "Save Image", save_image_callback);
  menufile.add( "Read Correspondences", read_corrs_callback);
  menufile.add( "Save Correspondences", save_corrs_callback);
  menufile.add( "Load world", load_world_callback);
  menufile.add( "Save world", save_world_callback);
  menufile.add( "Read F Matrix", read_f_matrix_callback);
  menufile.add( "Save F Matrix", save_f_matrix_callback);

  //view menu entries
  menuview.add("Set Range Params", set_range_params_callback);
  menuview.add("Intensity Profile", intensity_profile_callback);
  menuview.add("Histogram Plot ", intensity_histogram_callback);
  menuview.add( "Clear Geometry(selected pane)", clear_display_callback);
  menuview.add( "Display Corrs", display_corrs_callback);
  menuview.add( "Display World", project_world_callback);
  menuview.add( "Display Right Epipolar Lines", display_right_epi_lines_callback);
  menuview.add( "Display Picked Epipolar Lines(d)", display_picked_epi_line_callback, (vgui_key)'d');


  //edit menu entries
  menuedit.add( "Create Correspondence(k)", create_correspondence_callback, (vgui_key)'k');
  menuedit.add( "Pick Correspondence(p)", pick_correspondence_callback,(vgui_key)'p');
  menuedit.add( "Compute F Matrix", compute_f_matrix_callback);
  menuedit.add( "Reconstruct World Points", reconstruct_world_callback);

  //operation menu entries

  //Top level menu layout
  menubar.add( "File", menufile);
  menubar.add( "View", menuview);
  menubar.add( "Edit", menuedit);
  return menubar;
}


