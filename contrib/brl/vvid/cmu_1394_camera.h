// This is brl/vvid/cmu_1394_camera.h
#ifndef cmu_1394_camera_h_
#define cmu_1394_camera_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief cmu_1394_camera
//
// \author
//   J.L. Mundy
//
//  A wrapper class for the cmu 1394 camera.  Mainly to set parameters. The
//  approach is inheritance so we don't have to duplicate the CMU camera
//  interface. The parameters can be passed as a block to duplicate camera
//  setup and to conveniently support file I/0.
// 
// 
// \verbatim
//  Modifications:
//   J.L. Mundy Aug 29, 2002    Initial version.
//   J.L. Mundy Jun 01, 2003    Added methods to determine the capabilities
//                              of cameras and constrain parameters to 
//                              match max-min ranges of a given camera
// \endverbatim 
//--------------------------------------------------------------------------------
#include <windows.h>
#include <winbase.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <1394Camera.h>
#include <vil1/vil1_rgb.h>
#include <vil1/vil1_memory_image_of.h>
#include <vvid/cmu_1394_camera_params.h>
#include <vpro/vpro_video_process_sptr.h>
#include <vcl_iosfwd.h>

class cmu_1394_camera : public cmu_1394_camera_params, public C1394Camera
{
 public:
  cmu_1394_camera();
  cmu_1394_camera(int node, const cmu_1394_camera_params& cp);

  ~cmu_1394_camera();

  //:camera status
  bool get_camera_present() const { return camera_present_; }
  bool get_running() const { return running_; }

  //: basic camera operations
  //: initialize the camera
  bool init(int node);

  //: start and stop the camera
  bool  start();
  void  stop();

  //: update the camera driver frame store from the camera
  bool get_frame();

  //: access the frame store and map the buffer to a sampled rgb image
  bool  get_rgb_image(vil1_memory_image_of<vil1_rgb<unsigned char> >& im,
                      int pixel_sample_interval=1, bool reread = true);

  //: access the frame store and map the buffer to a sampled monochrome image
  bool  get_monochrome_image(vil1_memory_image_of<unsigned char>& im,
                             int pixel_sample_interval =1, bool reread = true);

  //: caching and saving video data. The saved images are not sampled 
  //: initialize the file capture process, provide a filename for the video
  void start_capture(vcl_string const & video_file_name);

  //: stop the file capture process, write out the video file
  bool stop_capture();
  
  //: camera information 
  //: send the current parameter values to the camera
  void update_settings();
  //: get string values for valid camera video configurations
  vcl_vector<vcl_string> get_capability_descriptions(){return capability_desc_;}
  //: get the set of valid 1394 camera format values
  vcl_vector<int> get_valid_formats(){return format_;}

  //: get the set of valid 1394 camera mode values
  vcl_vector<int> get_valid_modes(){return mode_;}

  //: get the set of valid 1394 frame rate codes
  vcl_vector<int> get_valid_rates(){return rate_;}

  //: get the index of the current valid configuration
  int get_current(){return current_;}

  //: set the index of the current valid configuration
  void set_current(int current){current_ = current;}

  //: get the string description of the currently selected configuration
  vcl_string current_capability_desc(){return capability_desc_[current_];}
  
  //: code values for the current configuration
  int current_format(){return format_[current_];}
  int current_mode(){return mode_[current_];}
  int current_rate(){return rate_[current_];}

  //: stream output of the camera properties
  friend vcl_ostream& operator << (vcl_ostream& os, const cmu_1394_camera& c);

 private:
  //internal methods
  void print_control(C1394CameraControl const& c);
  void print_status(C1394CameraControl const& c);
  void init_capabilities();
  void init_control();
  void validate_default_configuration();
  void update_video_configuration();
  //camera status flags
  bool link_failed_;
  bool camera_present_;
  bool running_;
  bool image_valid_;
  bool file_capture_;
  bool capture_;
  //video process
  vpro_video_process_sptr vp_;
  //camera capability information
  int current_;
  vcl_vector<int> format_;
  vcl_vector<int> mode_;
  vcl_vector<int> rate_;
  vcl_vector<vcl_string> capability_desc_;
};

#endif // cmu_1394_camera_h_
