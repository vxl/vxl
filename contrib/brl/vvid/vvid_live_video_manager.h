// This is brl/vvid/vvid_live_video_manager.h
#ifndef vvid_live_video_manager_h_
#define vvid_live_video_manager_h_
//
//-----------------------------------------------------------------------------
//:
// \file
// \brief A manager for displaying a live video sequence and live segmentation and processing overlays
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy January 09, 2002    Initial version.
// \endverbatim
//-----------------------------------------------------------------------------

#include <vil1/vil1_memory_image_of.h>
#include <vgui/vgui_viewer2D_tableau_sptr.h>
#include <vgui/vgui_wrapper_tableau.h>
#include <bgui/bgui_vtol2D_tableau_sptr.h>
#include <vvid/cmu_1394_camera.h>
#include <vpro/vpro_video_process_sptr.h>
#include <vvid/vvid_live_video_tableau.h>
#include <bgui/bgui_histogram_tableau_sptr.h>

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
  unsigned get_height() const { return height_; }
  unsigned get_width() const { return width_; }

  //: control video parameters
  void set_camera_params();

  //: change edge detection parameters
  void set_detection_params();

  //: no process
  void no_op();

  //: control video collection
  void start_live_video();
  void stop_live_video();

  //: show/hide histograms
  void toggle_histogram();

  //: capture and save a video sequence (old approach)
  void capture_sequence();

  //: new capture approach
  void init_capture();
  void stop_capture();

  //: quit the application
  void quit();

  //: access to the current frames
  bool get_current_rgb_image(unsigned camera_index,
                             int pix_sample_interval,
                             vil1_memory_image_of< vil1_rgb<unsigned char> >& im);

  bool get_current_mono_image(unsigned camera_index,
                              int pix_sample_interval,
                              vil1_memory_image_of<unsigned char>& im);

  //: control of the process result window
  void set_process_rgb_image(vil1_memory_image_of< vil1_rgb<unsigned char> >& im);
  void set_process_mono_image(vil1_memory_image_of<unsigned char>& im);

  //: access to the window
  vgui_window* get_window(){return win_;}
  void set_window(vgui_window* win){win_=win;}

  //: the virtual handle function
  virtual bool handle(const vgui_event&);
 protected:
  void display_topology();
  void display_image();
 private:
  //utility functions
  void run_frames();
  //flags
  bool init_successful_;
  bool edges_;
  bool histogram_;
  int sample_;
  unsigned width_;
  unsigned height_;
  unsigned num_cameras_;
  double min_msec_per_frame_;
  vgui_window* win_;
  vcl_vector <bgui_vtol2D_tableau_sptr> vt2Ds_;
  vcl_vector <vvid_live_video_tableau_sptr> vtabs_;
  vcl_vector <bgui_histogram_tableau_sptr> htabs_;
  cmu_1394_camera_params cp_;
  vil1_memory_image_of< vil1_rgb<unsigned char> > process_rgb_;
  vil1_memory_image_of<unsigned char> process_mono_;
  vpro_video_process_sptr video_process_;
  static vvid_live_video_manager *instance_;
};

#endif // vvid_live_video_manager_h_
