#ifndef bmvv_recon_menus_h_
#define bmvv_recon_menus_h_
//-----------------------------------------------------------------------------
//:
// \file
// \brief the setup for menus for bmvv
//  This is essentially a namespace for static methods;
//  there is no public constructor
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy December 11, 2002    Initial version.
// \endverbatim
//-----------------------------------------------------------------------------
#include <bmvv/dll.h>

class bmvv_recon_menus
{
 public:
  BMVV_DLL_DATA  static void quit_callback();
  BMVV_DLL_DATA static void load_image_callback();
  BMVV_DLL_DATA static void clear_display_callback();
  BMVV_DLL_DATA static void clear_selected_callback();
  BMVV_DLL_DATA static void read_3d_points_callback();
  BMVV_DLL_DATA static void initial_model_projection_callback();
  BMVV_DLL_DATA static void model_projection_callback();
  BMVV_DLL_DATA static void print_selected_corr_callback();
  BMVV_DLL_DATA static void pick_corr_callback();
  BMVV_DLL_DATA static void write_corrs_callback();
  BMVV_DLL_DATA static void read_corrs_callback();
  BMVV_DLL_DATA static void compute_homographies_callback();
  BMVV_DLL_DATA static void write_homographies_callback();
  BMVV_DLL_DATA static void read_homographies_callback();
  BMVV_DLL_DATA static void project_image_callback();
  BMVV_DLL_DATA static void set_images_callback();
  BMVV_DLL_DATA static void compute_harris_corners_callback();
  BMVV_DLL_DATA static void overlapping_projections_callback();
  BMVV_DLL_DATA static void overlapping_projections_z_callback();
  BMVV_DLL_DATA static void overlapping_harris_proj_z_callback();
  BMVV_DLL_DATA static void cross_correlate_plane_callback();
  BMVV_DLL_DATA static void cross_correlate_z_callback();
  BMVV_DLL_DATA static void cross_correlate_harris_z_callback();
  BMVV_DLL_DATA static void depth_image_callback();
  BMVV_DLL_DATA static void harris_depth_match_callback();
  BMVV_DLL_DATA static void z_corr_image_callback();
  BMVV_DLL_DATA static void corr_plot_callback();
  BMVV_DLL_DATA static void map_point_callback();
  BMVV_DLL_DATA static void map_image_callback();
  BMVV_DLL_DATA static void map_harris_corners_callback();
  BMVV_DLL_DATA static void match_harris_corners_callback();
  BMVV_DLL_DATA static void harris_sweep_callback();
  BMVV_DLL_DATA static void display_matched_corners_callback();
  BMVV_DLL_DATA static void display_harris_3d_callback();
  BMVV_DLL_DATA static void write_points_vrml_callback();
  BMVV_DLL_DATA static void read_points_vrml_callback();
  BMVV_DLL_DATA static void write_volumes_vrml_callback();
  BMVV_DLL_DATA static void read_change_data_callback();
  BMVV_DLL_DATA static void write_change_volumes_vrml_callback();
  BMVV_DLL_DATA static void compute_change_callback();
#if 0
  BMVV_DLL_DATA static void display_dense_match_callback();
#endif
  BMVV_DLL_DATA static void read_f_matrix_callback();
  BMVV_DLL_DATA static void show_epipolar_line_callback();
  BMVV_DLL_DATA static void show_world_homography_callback();
  BMVV_DLL_DATA static vgui_menu get_menu();
 private:
  bmvv_recon_menus() {}
};

#endif // bmvv_recon_menus_h_
