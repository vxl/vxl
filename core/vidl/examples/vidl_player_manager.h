// This is core/vidl/example/vidl_player_manager.h
#ifndef vidl_player_manager_h_
#define vidl_player_manager_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author Matt Leotta
// \brief A sample vidl video player
//
//----------------------------------------------------------------------------

#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_wrapper_tableau.h>
#include <vgui/vgui_image_tableau_sptr.h>
#include <vgui/vgui_viewer2D_tableau_sptr.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_window.h>
#include <vidl/vidl_movie.h>

//: A singleton manager class for playing videos.
//  The purpose this code is demonstrate the use of vidl.  The vidl_player
//  loads, displays, and saves (saving not implemented yet) videos in the 
//  following formats:
//   - Directory of Images (any type supported by vil)
//   - MPEG (still in early stages of development)
//   - AVI (currently Windows only)
//

class vidl_player_manager : public vgui_wrapper_tableau
{
 public:
  vidl_player_manager();
  ~vidl_player_manager();
  //: returns the unique instance of vidl_player_manger
  static vidl_player_manager *instance();

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


  //: get the window of this player
  vgui_window* get_window() { return win_; }

  //: set the window
  void set_window(vgui_window* win){win_=win;}

  //: tableau handle function
  virtual bool handle(const vgui_event&);

 protected:
  //utility functions
  void init();
  void redraw();

 private:
  //flags
  bool preload_frames_;
  bool play_video_;

  float time_interval_;
  unsigned width_;
  unsigned height_;
  vidl_movie_sptr my_movie_;
  vidl_movie::frame_iterator pframe_;
  vgui_window* win_;
  vgui_viewer2D_tableau_sptr v2D_;
  vgui_image_tableau_sptr itab_;

  static vidl_player_manager *instance_;

};

#endif // vidl_player_manager_h_
