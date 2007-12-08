// This is brl/vvid/vvid_live_video_tableau.h
#ifndef vvid_live_video_tableau_h_
#define vvid_live_video_tableau_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief A special image tableau that has a live camera as an image generator.
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy January 09, 2002    Initial version.
// \endverbatim
//--------------------------------------------------------------------------------
#include <vgui/vgui_image_tableau_sptr.h>
#include <vgui/vgui_image_tableau.h>
#include <vvid/vvid_live_video_tableau.h>
#include <vvid/cmu_1394_camera.h>

#include "vvid_live_video_tableau_sptr.h"

class vvid_live_video_tableau : public vgui_image_tableau
{
 public:
  typedef vgui_image_tableau base;
  vvid_live_video_tableau();

  vvid_live_video_tableau(int node, int pixel_sample_interval,
                          const cmu_1394_camera_params& cp);
  ~vvid_live_video_tableau();
  vcl_string type_name() const;

  //:video camera properties
  void set_camera_params(const cmu_1394_camera_params& cp);
  cmu_1394_camera_params get_camera_params() { return (cmu_1394_camera_params)cam_; }
  bool video_capabilities(int format, int mode, int frame_rate) { return cam_.m_videoFlags[format][mode][frame_rate]; }

  int get_current() const { return cam_.get_current(); }
  void set_current(int current) { cam_.set_current(current); }
  vcl_string current_capability_desc() const { return cam_.current_capability_desc(); }
  vcl_vector<vcl_string> get_capability_descriptions() const { return cam_.get_capability_descriptions(); }

  //:live video processing
  bool attach_live_video();
  bool start_live_video();
  void update_frame();
  void stop_live_video();

  //:live capture methods
  void start_capture(vcl_string const & video_file_name) { cam_.start_capture(video_file_name); }
  bool stop_capture() { return cam_.stop_capture(); }
  //: collection state, i.e. is live and capturing frames
  bool get_video_live() const { return live_; }

  //: pixel sample interval for display
  void set_pixel_sample_interval(int pix_sample_itvl) { pixel_sample_interval_=pix_sample_itvl; }
  //: Access to next live camera frames. Causes the camera to take a new frame
  void get_camera_rgb_image(vil1_memory_image_of< vil1_rgb<unsigned char> >& im,
                            int pix_sample_interval = 1);
  void get_camera_mono_image(vil1_memory_image_of<unsigned char>& im,
                             int pix_sample_interval = 1);

  // return current cached images with no resampling
  vil1_memory_image_of< vil1_rgb<unsigned char> > get_rgb_frame(){ return rgb_frame_; }
  vil1_memory_image_of<unsigned char> get_mono_frame(){ return mono_frame_; }

  //: Access to current cached camera frames
  // Both styles of image access are supported

  //: New pointer to the image
  vil1_memory_image_of< vil1_rgb<unsigned char> > get_current_rgb_image(int pix_sample_interval);

  //: User supplies the pointer to the image
  bool get_current_rgb_image(int pix_sample_interval,
                             vil1_memory_image_of< vil1_rgb<unsigned char> >& im);

  vil1_memory_image_of<unsigned char> get_current_mono_image(int pix_sample_interval);

  bool get_current_mono_image(int pix_sample_interval,
                              vil1_memory_image_of<unsigned char>& im);

 protected:
  //: Handle all events for this tableau.
  bool handle(vgui_event const &e);

 private:
  //status flags
  bool live_;//video is live
  //the live camera
  int node_;//the camera id
  cmu_1394_camera cam_;
  int pixel_sample_interval_;//default pixel sample spacing
  vil1_memory_image_of< vil1_rgb<unsigned char> > rgb_frame_;
  vil1_memory_image_of< unsigned char > mono_frame_;
};

struct vvid_live_video_tableau_new : public vvid_live_video_tableau_sptr
{
 public:
  vvid_live_video_tableau_new() :
    vvid_live_video_tableau_sptr(new vvid_live_video_tableau()) {}

  vvid_live_video_tableau_new(int node, int pixel_sample_interval,
                              const cmu_1394_camera_params& cp) :
    vvid_live_video_tableau_sptr(new vvid_live_video_tableau(node,
                                                             pixel_sample_interval,
                                                             cp))
  {}

  operator vgui_image_tableau_sptr () const { vgui_image_tableau_sptr tt; tt.vertical_cast(*this); return tt; }
};

#endif // vvid_live_video_tableau_h_
