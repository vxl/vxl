#ifndef vidfpl_menus_h_
#define vidfpl_menus_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief the setup for vidfpl_menus for the video file player
//  This is essentially a namespace for static methods;
//  there is no public constructor
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy Oct 05, 2002    Initial version.
// \endverbatim
//--------------------------------------------------------------------------------

class vgui_menu;

//: Menu callbacks are represented as static methods
//  The key method is ::get_menu, which does the work of
//  assembling the menu bar for the main executable 
class vidfpl_menus
{
 public:
  static void load_video_callback();
  static void start_frame_callback();
  static void end_frame_callback();
  static void play_video_callback();
  static void pause_video_callback();
  static void next_frame_callback();
  static void prev_frame_callback();
  static void stop_video_callback();
  static void easy2D_tableau_demo_callback();
  static void no_op_callback();
  static void difference_frames_callback();
  static void compute_motion_callback();
  static void compute_lucas_kanade_callback();
  static void compute_harris_corners_callback();
  static void compute_vd_edges_callback();
  static void compute_line_fit_callback();
  static void compute_grid_match_callback();
  static void compute_corr_tracking_callback();
  static void compute_info_tracking_callback();
  static void display_poly_track_callback();
  static void start_save_display_callback();
  static void end_save_display_callback();
  static void generate_basis_sequence_callback();
  static void compute_fourier_transform_callback();
  static void spatial_filter_callback();
  static void create_box_callback();
  static void create_polygon_callback();
  static void create_long_arm_tip_callback();
  static void create_short_arm_tip_callback();
  static void create_stem_callback();
  static void create_background_model_callback();
  static void exercise_art_model_callback();
  static void track_art_model_callback();
  static void display_art_model_track_callback();
  static void display_ihs_callback();
  static void save_frame_callback();
  static void save_half_res_callback();
  static void create_c_and_g_tracking_face_callback();
  static void display_tracked_hist_data_callback();
  static void capture_feature_data_callback();
  static void quit_callback();
  static vgui_menu get_menu();
 private:
  vidfpl_menus() {}
};

#endif // vidfpl_menus_h_
