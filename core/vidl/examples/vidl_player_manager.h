// This is core/vidl/examples/vidl_player_manager.h
#ifndef vidl_player_manager_h_
#define vidl_player_manager_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author Matt Leotta
// \brief A sample vidl video player
//
//----------------------------------------------------------------------------

#include <iostream>
#include <memory>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgui/vgui_wrapper_tableau.h>
#include <vgui/vgui_image_tableau_sptr.h>
#include <vgui/vgui_viewer2D_tableau_sptr.h>
#include <vgui/vgui_window.h>
#include <vidl/vidl_istream.h>
#include <vidl/vidl_ostream.h>

//: A singleton manager class for playing videos.
//  The purpose this code is demonstrate the use of vidl.  The vidl_player
//  loads, displays, and saves (saving not implemented yet) videos.
//
class vidl_player_manager : public vgui_wrapper_tableau
{
 public:
  vidl_player_manager();
  ~vidl_player_manager();
  //: returns the unique instance of vidl_player_manger
  static vidl_player_manager *instance();

  //: clean up before the program terminates
  void quit();

  //: height (in pixels) of the video frame
  unsigned get_height() const { return height_; }

  //: width (in pixels) of the video frame
  unsigned get_width() const { return width_; }

  //: open the input video stream
  void open_istream();

  //: open the output video stream
  void open_ostream();

  //: close the input video stream
  void close_istream();

  //: close the output video stream
  void close_ostream();

  //: Pipe the input stream into the output stream
  void pipe_streams();

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
  void set_window(vgui_window* win) { win_=win; }

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
  std::unique_ptr<vidl_istream> istream_;
  std::unique_ptr<vidl_ostream> ostream_;
  vgui_window* win_;
  vgui_viewer2D_tableau_sptr v2D_;
  vgui_image_tableau_sptr itab_;

  static vidl_player_manager *instance_;
};

#endif // vidl_player_manager_h_
