// This is brl/vvid/vvid_live_stereo_manager.h
#ifndef vvid_live_stereo_manager_h_
#define vvid_live_stereo_manager_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief the live_video_manager for playing video sequences
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy Apr 14, 2002    Initial version.
// \endverbatim
//--------------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vil1/vil1_memory_image_of.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_image_tableau_sptr.h>
#include <vgui/vgui_easy2D_tableau_sptr.h>
#include <vgui/vgui_viewer2D_tableau_sptr.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_window.h>
#include <vidl_vil1/vidl_vil1_movie.h>
#include <vvid/cmu_1394_camera.h>
#include <vpro/vpro_video_process_sptr.h>
#include <vvid/vvid_live_video_tableau.h>

//: A manager for displaying live video frames and processing on the frames.
//  The cameras are assumed to be wrapped in the live_video_frame
//  class. At startup the number of cameras is determined. A reduced resolution
//  image of each camera is shown in an array at the right of the display. The
//  top is camera 0 the bottom is camera N-1. The left pane is used to display
//  results of processing on the set of cameras.
class vvid_live_stereo_manager : public vgui_grid_tableau
{
 public:
  vvid_live_stereo_manager();
  ~vvid_live_stereo_manager();
  static vvid_live_stereo_manager *instance();

  //: properties of the video frames
  unsigned get_height() const { return height_; }
  unsigned get_width() const { return width_; }

  //: properties of the camera setup
  int get_N_views() const { return N_views_; }

  //: control live video actions
  void set_camera_params();
  void setup_views();
  void start_live_video();
  void stop_live_video();
  void quit();
  //: access to the current frames
  bool get_current_rgb_image(int view_no, int pix_sample_interval,
                             vil1_memory_image_of<vil1_rgb<unsigned char> >& im);

  bool get_current_mono_image(int view_no, int pix_sample_interval,
                              vil1_memory_image_of<unsigned char>& im);

  //: control of the process result window
  void set_process_rgb_image(vil1_memory_image_of< vil1_rgb<unsigned char> >& im);
  void set_process_mono_image(vil1_memory_image_of<unsigned char>& im);

  //: access to the window
  vgui_window* get_window(){return win_;}
  void set_window(vgui_window* win){win_=win;}

  //: the virtual handle function
  virtual bool handle(const vgui_event&);

 private:
  //utility functions
  void run_frames();
  //flags
  bool init_successful_;
  bool live_capture_;
  int N_views_;
  unsigned width_;
  unsigned height_;
  vgui_window* win_;

  vgui_image_tableau_sptr it_;
  vgui_viewer2D_tableau_sptr v2D_;
  vcl_vector<vvid_live_video_tableau_sptr> vframes_;
  cmu_1394_camera_params cp_;
  vil1_memory_image_of< vil1_rgb<unsigned char> > process_rgb_;
  vil1_memory_image_of<unsigned char> process_mono_;
  vpro_video_process_sptr video_process_;
  static vvid_live_stereo_manager *instance_;
};

#endif // vvid_live_stereo_manager_h_
