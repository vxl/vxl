// vsrl_menus.h
// Menus for use with the vgui_dense_matcher_test program(s)
//
// G.W. Brooksby
// 2/13/03

#ifndef vsrl_menus_h_
#define vsrl_menus_h_

class vsrl_menus
{
 public:
  static void quit_callback();
  static void load_left_image_callback();
  static void load_right_image_callback();
  static void load_disparity_image_callback();
  static void save_disparity_image_callback();
  static void load_params_file_callback();
  static void point_pick_callback();
  static void clear_all_callback();
  static void do_dense_matching_callback();
  static void find_regions_callback();
  static void jseg_regions_callback();
  static void set_params_callback();
  static void draw_north_callback();
  static void test_left_func_callback();
  static void test_right_func_callback();
  static void make_3d_callback();
  static void raw_correlation_callback();
  static void corner_method_callback();
  static vgui_menu get_menus();
 private:
  vsrl_menus(){};
};

#endif // vsrl_menus_h_
