#ifndef brct_menus_h_
#define brct_menus_h_
//-----------------------------------------------------------------------------
//:
// \file
// \brief This file is adopted from Joe's work.
//  This is essentially a namespace for static methods;
//  there is no public constructor
// \author
//  Kongbin Kang
//
//-----------------------------------------------------------------------------

#include <vgui/vgui_menu.h>
class brct_menus
{
 public:
  static void load_status();
  static void save_status();
  static void write_vrml_file();
  static void create_line();
  static void show_epipole();
  static void display_epipolar_lines_callback();
  static void load_image_callback();
  static void show_back_projection();
  static void remove_debug_info();
  static void show_next_observes();
  static void show_predicted_curves();
  static void init_kalman_callback();
  static void kalman_go_callback();
  static void write_vrml_callback();
  static void write_results_callback();
  static void print_motion_array_callback();
  static void quit_callback();
  static vgui_menu get_menu();
 private:
  brct_menus() {}
};

#endif // brct_menus_h_

