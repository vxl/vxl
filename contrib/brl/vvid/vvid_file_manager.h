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
#include <vil/vil_image.h>
//#include <vil/vil_memory_image_of.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_image_tableau_sptr.h>
#include <vgui/vgui_easy2D_tableau_sptr.h>
#include <vgui/vgui_viewer2D_tableau_sptr.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_window.h>
#include <vidl/vidl_movie.h>

//:A singleton manager class for playing videos. A vector of images with
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
class vvid_file_manager : public vgui_grid_tableau
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

  //: load each frame of the video into a cached vector of overlays
  //  if chaching is enabled
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

  //: get the window of this player 
  vgui_window* get_window(){return _win;}

  //: set the window
  void set_window(vgui_window* win){_win=win;}

  //: tableau handle function
  virtual bool handle(const vgui_event&);
 protected:
  void cached_play();
  void un_cached_play();
 private:
  //utility functions

  //flags
  bool _cache_frames;
  bool _play_video;
  bool _pause_video;
  bool _next_frame;
  bool _prev_frame;
  float _time_interval;
  unsigned width_;
  unsigned height_;
  vidl_movie_sptr _my_movie;
  vgui_window* _win;
  vcl_vector<vgui_easy2D_tableau_sptr> _tabs;
  vgui_viewer2D_tableau_sptr _v2D;
  static vvid_file_manager *_instance;
};
#endif // vvid_file_manager_h_
