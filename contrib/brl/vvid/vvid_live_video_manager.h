//this-sets-emacs-to-*-c++-*-mode
#ifndef vvid_live_video_manager_h_
#define vvid_live_video_manager_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief Video player
//   the live_video_manager for playing video sequences
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy Apr 14, 2002    Initial version.
// \endverbatim
//--------------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vil/vil_image.h>
#include <vil/vil_memory_image_of.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_image_tableau_sptr.h>
#include <vgui/vgui_easy2D_tableau_sptr.h>
#include <vgui/vgui_viewer2D_tableau_sptr.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_window.h>
#include <vidl/vidl_movie.h>
#include <vvid/cmu_1394_camera.h>
#include <vvid/vvid_video_process_sptr.h>
#include <vvid/vvid_live_video_frame.h>
//: A manager for displaying live video frames and processing on the
//  frames.  The cameras are assumed to be wrapped in the live_video_frame 
//  class. At startup the number of cameras is determined. A reduced resolution
//  image of each camera is shown in an array at the right of the display. The
//  top is camera 0 the bottom is camera N-1. The left pane is used to display
//  results of processing on the set of cameras.
class vvid_live_video_manager : public vgui_grid_tableau
{
 public:
  vvid_live_video_manager();
  ~vvid_live_video_manager();
  static vvid_live_video_manager *instance();

  //: properties of the video frames
  unsigned get_height(){return height_;}
  unsigned get_width(){return width_;}

  //: properties of the camera setup
  int get_N_views(){return _N_views;}

  //: control live video actions
  void set_camera_params();
  void setup_views();
  void start_live_video();
  void stop_live_video();
  void quit();
  //: access to the current frames
  bool get_current_rgb_image(int view_no, int pix_sample_interval, 
                             vil_memory_image_of<vil_rgb<unsigned char> >& im);

  bool get_current_mono_image(int view_no, int pix_sample_interval,
                              vil_memory_image_of<unsigned char>& im);
  
  //: control of the process result window
  void set_process_rgb_image(vil_memory_image_of< vil_rgb<unsigned char> >& im);
  void set_process_mono_image(vil_memory_image_of<unsigned char>& im);

  //: access to the window
  vgui_window* get_window(){return _win;}
  void set_window(vgui_window* win){_win=win;}

  //: the virtual handle function
  virtual bool handle(const vgui_event&);
 protected:

 private:
  //utility functions
  void run_frames();
  //flags
  bool _init_successful;
  bool _live_capture;
  int _N_views;
  unsigned width_;
  unsigned height_;
  vgui_window* _win;
  
  vgui_image_tableau_sptr _it;
  vgui_viewer2D_tableau_sptr _v2D;
  vcl_vector<vvid_live_video_frame_sptr> _vframes;
  cmu_1394_camera_params _cp;
  vil_memory_image_of< vil_rgb<unsigned char> > _process_rgb;
  vil_memory_image_of<unsigned char> _process_mono;
  vvid_video_process_sptr _video_process;
  static vvid_live_video_manager *_instance;
};
#endif // vvid_live_video_manager_h_
