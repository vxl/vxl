// This is brl/vvid/vvid_file_manager.h
#ifndef vvid_file_manager_h_
#define vvid_file_manager_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author J.L. Mundy
// \brief A sample video file player
//
// \verbatim
//  Modifications:
//   J.L. Mundy October 05, 2002 Ported from jvid
// \endverbatim
//----------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vcl_list.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_wrapper_tableau.h>
#include <vgui/vgui_image_tableau_sptr.h>
#include <vgui/vgui_rubberband_tableau_sptr.h>
#include <bgui/bgui_vtol2D_tableau_sptr.h>
#include <vgui/vgui_viewer2D_tableau_sptr.h>
#include <strk/strk_art_info_model_sptr.h>
#include <vtol/vtol_face_2d_sptr.h>
#include <vpro/vpro_video_process_sptr.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_window.h>
#include <vidl_vil1/vidl_vil1_movie.h>
#include <vvid/vvid_frame_trail.h>

//: A singleton manager class for playing videos.
// A vector of images with
// enclosing image and easy2D tableaux is cached so that computed overlays
// such as Harris corners can be played back quickly. The cache option can be
// turned off if one is going to just play the video and not apply image
// segmentation processing to the frames.
//
// A demo of overlaying points on the video is provide by ::easy2D_demo
// to get rid of the points, reload the video
//
// It is planned to extend the class to handle multiple panes (grid locations)
// so that different (or the same) videos can be playing simultaneously
// in different zoom states as well as pause states. This extension will
// the state variables to be changed to vectors as well as having multiple
// caches.. etc.
//
// Known problems:
//  - quiting while the video is paused can cause a seg fault since
//    the movie gets deleted before the loop quits
//  - There is a continuous gl error stream from vgui_adaptor. Something to
//    do with "setting draw buffer to back"
//

class vvid_file_manager : public vgui_wrapper_tableau
{
 public:
  vvid_file_manager();
  ~vvid_file_manager();
  //: returns the unique instance of vvid_file_manger
  static vvid_file_manager *instance();

  //: height (in pixels) of the video frame
  unsigned get_height() const { return height_; }

  //: width (in pixels) of the video frame
  unsigned get_width() const { return width_; }

  //: load each frame of the video into a cached vector of overlays if caching is enabled
  void load_video_file();

  //: loop through the frames and display
  void play_video();

  //: stop at the current frame
  void pause_video();

  //: stop playing and return to the first frame
  void stop_video();

  //: pops up a dialog to indicate what frame to go to
  void go_to_frame();

  //: index to the next frame (must be paused)
  void next_frame();

  //: index to the previous frame (must be paused)
  void prev_frame();

  //: set the frame rate
  void set_speed();

  //: a demo of spatial overlays on the video (scrolling points)
  void easy2D_tableau_demo();

  //: No operation
  void no_op();

  //: show the difference of sequential frames
  void difference_frames();

  //: show the motion condition
  void compute_motion();

  //: show the Lucas-Kanade vectors as color
  void compute_lucas_kanade();

  //: show the Harris corners
  void compute_harris_corners();

  //: show VanDuc edges
  void compute_vd_edges();

  //: show curve tracking
  void compute_curve_tracking();

  //: show fit lines
  void compute_line_fit();

  //: show matches of calibration grid
  void compute_grid_match();

  //: show correlation tracking
  void compute_corr_tracking();

  //: show mutual information tracking
  void compute_info_tracking();

  //: display a tracked polygon
  void display_poly_track();

  //: display a tracked art model
  void display_art_model_track();

  //: enable display movie generation
  void start_save_display();

  //: enable display movie generation
  void end_save_display();

  //: generate a basis sequence
  void generate_basis_sequence();

  //: compute fourier transform
  void compute_fourier_transform();

  //: compute fourier transform
  void spatial_filter();

  //: create a box by rubberbanding
  void create_box();

  //: create a polygon by rubberbanding
  void create_polygon();

  //: create the elements of the art model
  void create_stem();
  void create_long_arm_tip();
  void create_short_arm_tip();

  //: test model
  void exercise_art_model();

  //: process_art_model
  void track_art_model();

  //: get the window of this player
  vgui_window* get_window() { return win_; }

  //: set the window
  void set_window(vgui_window* win){win_=win;}

  //: tableau handle function
  virtual bool handle(const vgui_event&);

 protected:
  //utility functions
  void init();
  void cached_play();
  void un_cached_play();
  void display_image();
  void display_spatial_objects();
  void display_topology();
  void set_changing_colors(int num, float *r, float *g, float *b);
  void save_display(int frame);
 private:
  //flags
  bool cache_frames_;
  bool play_video_;
  bool pause_video_;
  bool next_frame_;
  bool prev_frame_;
  bool save_display_;
  bool overlay_pane_;
  bool track_;//keep trail of display items
  int window_;//frame trail time window
  bool color_label_;//display with a different color for different labels
  float time_interval_;
  unsigned width_;
  unsigned height_;
  vidl_vil1_movie_sptr my_movie_;
  vgui_window* win_;
  vcl_vector<bgui_vtol2D_tableau_sptr> tabs_;
  vgui_viewer2D_tableau_sptr v2D0_;
  vgui_viewer2D_tableau_sptr v2D1_;
  bgui_vtol2D_tableau_sptr easy0_;
  bgui_vtol2D_tableau_sptr easy1_;
  vgui_image_tableau_sptr itab0_;
  vgui_rubberband_tableau_sptr rubber0_;
  vgui_image_tableau_sptr itab1_;
  vpro_video_process_sptr video_process_;
  vgui_grid_tableau_sptr grid_;
  vvid_frame_trail frame_trail_;
  static vvid_file_manager *instance_;
  vcl_vector<vcl_vector<vsol_spatial_object_2d_sptr> > cached_spat_objs_;
  vcl_list<vil1_image> display_output_frames_;
  vcl_string display_output_file_;
  vtol_face_2d_sptr stem_;
  vtol_face_2d_sptr long_tip_;
  vtol_face_2d_sptr short_tip_;
  strk_art_info_model_sptr art_model_;
};

#endif // vvid_file_manager_h_
