//this-sets-emacs-to-*-c++-*-mode
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
//   J.L. Mundy Octover 05, 2002 Ported from jvid
// \endverbatim
//----------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_wrapper_tableau.h>
#include <vgui/vgui_image_tableau_sptr.h>
#include <vgui/vgui_easy2D_tableau_sptr.h>
#include <vgui/vgui_viewer2D_tableau_sptr.h>
#include <vvid/vvid_video_process_sptr.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_window.h>
#include <vidl/vidl_movie.h>

//:A singleton manager class for playing videos.
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
// so that different (or the same) videos can be playing simulataneously
// in different zoom states as well as pause states. This extension will
// the state variables to be changed to vectors as well as having multiple
// caches.. etc.
//
// Known problems:
//  1) quiting while the video is paused can cause a seg fault since
//     the movie gets deleted before the loop quits
//  2) There is a continous gl error stream from vgui_adaptor. Something to
//     do with "setting draw buffer to back"
//.
class vvid_file_manager : public vgui_wrapper_tableau
{
 public:
  vvid_file_manager();
  ~vvid_file_manager();
  //: returns the unique instance of vvid_file_manger
  static vvid_file_manager *instance();

  //: height (in pixels) of the video frame
  unsigned get_height(){return height_;}

  //: width (in pixels) of the video frame
  unsigned get_width(){return width_;}

  //: load each frame of the video into a cached vector of overlays if chaching is enabled
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

  //: get the window of this player
  vgui_window* get_window(){return win_;}

  //: set the window
  void set_window(vgui_window* win){win_=win;}

  //: tableau handle function
  virtual bool handle(const vgui_event&);

 protected:
  void init();
  void cached_play();
  void un_cached_play();
  void display_image();
 private:
  //utility functions

  //flags
  bool cache_frames_;
  bool play_video_;
  bool pause_video_;
  bool next_frame_;
  bool prev_frame_;
  float time_interval_;
  unsigned width_;
  unsigned height_;
  vidl_movie_sptr my_movie_;
  vgui_window* win_;
  vcl_vector<vgui_easy2D_tableau_sptr> tabs_;
  vgui_viewer2D_tableau_sptr v2D0_;
  vgui_viewer2D_tableau_sptr v2D1_;
  vgui_easy2D_tableau_sptr easy0_;
  vgui_image_tableau_sptr itab0_;
  vgui_image_tableau_sptr itab1_;
  vvid_video_process_sptr video_process_;
  vgui_grid_tableau_sptr grid_;
  static vvid_file_manager *instance_;
};
#endif // vvid_file_manager_h_
