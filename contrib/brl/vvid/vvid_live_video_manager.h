//this-sets-emacs-to-*-c++-*-mode
#ifndef vvid_live_video_manager_h_
#define vvid_live_video_manager_h_
//
//-----------------------------------------------------------------------------
//:
// \file
// \brief A manager for displaying a live video sequence and 
//        live segmentation and processing overlays
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy January 09, 2002    Initial version.
// \endverbatim
//-----------------------------------------------------------------------------

#include <vcl_vector.h>
#include <vil/vil_memory_image_of.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_image_tableau_sptr.h>
#include <vgui/vgui_easy2D_tableau_sptr.h>
#include <vgui/vgui_viewer2D_tableau_sptr.h>
#include <vgui/vgui_wrapper_tableau.h>
#include <bgui/bgui_vtol2D_tableau_sptr.h>
#include <vvid/cmu_1394_camera.h>
#include <vvid/vvid_video_process_sptr.h>
#include <vvid/vvid_live_video_tableau.h>

class vgui_window;

class vvid_live_video_manager : public vgui_wrapper_tableau
{
 public:
  vvid_live_video_manager();
  ~vvid_live_video_manager();
  static vvid_live_video_manager *instance();

  //:post construction actions
  void init();

  //: properties of the video frames
  unsigned get_height(){return height_;}
  unsigned get_width(){return width_;}

  //: control video parameters
  void set_camera_params();

  //: change edge detection parameters
  void set_detection_params();

  //: control video collection
  void start_live_video();
  void stop_live_video();

  //: quit the application
  void quit();

  //: access to the current frames
  bool get_current_rgb_image(int pix_sample_interval,
                             vil_memory_image_of< vil_rgb<unsigned char> >& im);

  bool get_current_mono_image(int pix_sample_interval,
                              vil_memory_image_of<unsigned char>& im);

  //: control of the process result window
  void set_process_rgb_image(vil_memory_image_of< vil_rgb<unsigned char> >& im);
  void set_process_mono_image(vil_memory_image_of<unsigned char>& im);

  //: access to the window
  vgui_window* get_window(){return win_;}
  void set_window(vgui_window* win){win_=win;}

  //: the virtual handle function
  virtual bool handle(const vgui_event&);
 protected:

 private:
  //utility functions
  void run_frames();
  //flags
  bool init_successful_;
  bool live_capture_;
  bool edges_;
  unsigned width_;
  unsigned height_;
  vgui_window* win_;
  bgui_vtol2D_tableau_sptr vt2D_;
  vvid_live_video_tableau_sptr vtab_;
  cmu_1394_camera_params cp_;
  vil_memory_image_of< vil_rgb<unsigned char> > process_rgb_;
  vil_memory_image_of<unsigned char> process_mono_;
  vvid_video_process_sptr video_process_;
  static vvid_live_video_manager *instance_;
};

#endif // vvid_live_video_manager_h_
