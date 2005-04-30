// This is brl/vvid/vvid_vil_file_manager.h
#ifndef vvid_vil_file_manager_h_
#define vvid_vil_file_manager_h_
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
#include <vgui/vgui_wrapper_tableau.h>
#include <vgui/vgui_image_tableau_sptr.h>
#include <vgui/vgui_rubberband_tableau_sptr.h>
#include <vgui/vgui_style_sptr.h>
#include <vgui/vgui_range_map_params_sptr.h>
#include <bgui/bgui_vtol2D_tableau_sptr.h>
#include <bgui/bgui_picker_tableau_sptr.h>
#include <bgui/bgui_bargraph_clipon_tableau_sptr.h>
#include <vgui/vgui_viewer2D_tableau_sptr.h>
#include <vgui/vgui_window.h>
#include <vidl/vidl_movie_sptr.h>

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

class vvid_vil_file_manager : public vgui_wrapper_tableau
{
 public:
  vvid_vil_file_manager();
  ~vvid_vil_file_manager();
  //: returns the unique instance of vvid_vil_file_manger
  static vvid_vil_file_manager *instance();

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

  //: pops up a dialog to indicate what frame to start play
  void start_frame();

  //: pops up a dialog to indicate what frame to start play
  void end_frame();

  //: index to the next frame (must be paused)
  void next_frame();

  //: index to the previous frame (must be paused)
  void prev_frame();

  //: set the frame rate
  void set_speed();

  //: set the range map rate
  void set_range_params();

  //: get the window of this player
  vgui_window* get_window() { return win_; }

  //: set the window
  void set_window(vgui_window* win){win_=win;}

  //: tableau handle function
  virtual bool handle(const vgui_event&);

 protected:
  //utility functions
  void init();
  void un_cached_play();
 private:
  //flags
  bool cache_frames_;
  bool play_video_;
  bool pause_video_;
  bool next_frame_;
  bool prev_frame_;
  vgui_range_map_params_sptr rmps_;
  int window_;//frame trail time window
  int start_frame_; //frame to start play
  int end_frame_; //frame to start play
  float time_interval_;
  int display_frame_repeat_;//insert duplicate frames to slow playback
  unsigned width_;
  unsigned height_;
  vidl_movie_sptr my_movie_;
  vgui_window* win_;
  vcl_vector<bgui_vtol2D_tableau_sptr> tabs_;
  vgui_viewer2D_tableau_sptr v2D0_;
  vgui_viewer2D_tableau_sptr v2D1_;
  bgui_vtol2D_tableau_sptr easy0_;
  bgui_vtol2D_tableau_sptr easy1_;
  vgui_image_tableau_sptr itab0_;
  vgui_rubberband_tableau_sptr rubber0_;
  bgui_picker_tableau_sptr picktab0_;
  vgui_image_tableau_sptr itab1_;
  
  vgui_grid_tableau_sptr grid_;
  
  static vvid_vil_file_manager *instance_;
};

#endif // vvid_vil_file_manager_h_
