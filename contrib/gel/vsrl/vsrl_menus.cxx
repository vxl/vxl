// vsrl_menus.cxx
// Menus for use with the vgui_dense_matcher_test program(s)
//
// G.W. Brooksby
// 2/13/03

#include <vgui/vgui.h>
#include <vgui/vgui_key.h>
#include <vgui/vgui_modifier.h>
#include <vgui/vgui_menu.h>
#include "vsrl_manager.h"
#include "vsrl_menus.h"

// Static menu callback functions

void vsrl_menus::quit_callback()
{
  vsrl_manager::instance()->quit();
}

void vsrl_menus::load_left_image_callback()
{
  vsrl_manager::instance()->load_left_image();
}

void vsrl_menus::load_right_image_callback()
{
  vsrl_manager::instance()->load_right_image();
}

void vsrl_menus::load_disparity_image_callback()
{
  vsrl_manager::instance()->load_disparity_image();
}

void vsrl_menus::save_disparity_image_callback()
{
  vsrl_manager::instance()->save_disparity_image();
}

void vsrl_menus::load_params_file_callback()
{
  vsrl_manager::instance()->load_params_file();
}

void vsrl_menus::point_pick_callback()
{
  vsrl_manager::instance()->point_pick();
}

void vsrl_menus::clear_all_callback()
{
  vsrl_manager::instance()->clear_all();
}

void vsrl_menus::do_dense_matching_callback()
{
  vsrl_manager::instance()->do_dense_matching();
}

void vsrl_menus::find_regions_callback()
{
  vsrl_manager::instance()->find_regions();
}

void vsrl_menus::set_params_callback()
{
  vsrl_manager::instance()->set_params();
}

void vsrl_menus::draw_north_callback()
{
  vsrl_manager::instance()->draw_north_arrow();
}

void vsrl_menus::test_left_func_callback()
{
  vsrl_manager::instance()->test_left_func();
}

void vsrl_menus::test_right_func_callback()
{
  vsrl_manager::instance()->test_right_func();
}

void vsrl_menus::make_3d_callback()
{
  vsrl_manager::instance()->make_3d();
  return;
}

void vsrl_menus::raw_correlation_callback()
{
  vsrl_manager::instance()->raw_correlation();
  return;
}

void vsrl_menus::corner_method_callback()
{
  vsrl_manager::instance()->corner_method();
  return;
}

// vsrl_menus definitions

vgui_menu vsrl_menus::get_menus()
{
  vgui_menu menubar;
  vgui_menu menufile;
  vgui_menu menuops;

  // File menu entries
  menufile.add("Load Left Image...", load_left_image_callback, (vgui_key)'l', vgui_CTRL);
  menufile.add("Load Right Image...", load_right_image_callback, (vgui_key)'r', vgui_CTRL);
  menufile.add("Load Disparity Image...", load_disparity_image_callback, (vgui_key)'d', vgui_CTRL);
  menufile.add("Load Parameters File...", load_params_file_callback, (vgui_key)'p', vgui_CTRL);
  menufile.add("Save Disparity Image...", save_disparity_image_callback, (vgui_key)'s', vgui_CTRL);
  menufile.add("Quit", quit_callback, (vgui_key)'q', vgui_CTRL);

  // Operations menu entries
  menuops.add("Clear all 2D objects", clear_all_callback, (vgui_key)'x', vgui_CTRL);
  menuops.add("Set Dense Matching Parameters...", set_params_callback);
  menuops.add("Perform Dense Matching", do_dense_matching_callback);
  menuops.add("Make 3D from disparity map", make_3d_callback);
  menuops.add("Draw North Arrow", draw_north_callback);
  menuops.add("Region Segmentation...", find_regions_callback);
  menuops.add("JSEG Segmentation (Left)...", jseg_regions_callback);
  menuops.add("Raw Correlation", raw_correlation_callback);
  menuops.add("Corner Method", corner_method_callback);
  menuops.add("Test Function (Left)", test_left_func_callback);
  menuops.add("Test Function (Right)", test_right_func_callback);

  // Main menu bar
  menubar.add("File", menufile);
  menubar.add("Ops", menuops);
  return menubar;
}
