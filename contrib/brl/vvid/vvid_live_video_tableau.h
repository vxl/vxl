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
#include <vvid/vvid_live_video_tableau_sptr.h>
#include <vvid/cmu_1394_camera.h>

class vvid_live_video_tableau : public vgui_image_tableau
{
 public:
  typedef vgui_image_tableau base;
  vvid_live_video_tableau();

  vvid_live_video_tableau(int node, int pixel_sample_interval,
                          const cmu_1394_camera_params& cp);
  ~vvid_live_video_tableau();
  vcl_string type_name() const;
  //camera manipulation

  void set_camera_params(const cmu_1394_camera_params& cp);
  cmu_1394_camera_params get_camera_params(){return (cmu_1394_camera_params)cam_;}
  bool attach_live_video();
  void start_live_video();
  void update_frame();
  void stop_live_video();

  //: collection state, i.e. is live and capturing frames
  bool get_video_live(){return live_;}

  //: Access to next live camera frames. Causes the camera to take a new frame
  void get_camera_rgb_image(vil_memory_image_of< vil_rgb<unsigned char> >& im,
                            int pix_sample_interval = 1);
  void get_camera_mono_image(vil_memory_image_of<unsigned char>& im,
                             int pix_sample_interval = 1);

  //: Access to current cached camera frames
  // Both styles of image access are supported

  //: New pointer to the image
  vil_memory_image_of< vil_rgb<unsigned char> > get_current_rgb_image(int pix_sample_interval);

  //: User supplies the pointer to the image
  bool get_current_rgb_image(int pix_sample_interval,
                             vil_memory_image_of< vil_rgb<unsigned char> >& im);

  vil_memory_image_of<unsigned char> get_current_mono_image(int pix_sample_interval);

  bool get_current_mono_image(int pix_sample_interval,
                              vil_memory_image_of<unsigned char>& im);


 protected:
  //: Handle all events for this tableau.
  bool handle(vgui_event const &e);
 private:
  //utility functions
  //status flags
  bool live_;//video is live
  //the live camera
  int node_;//the camera id
  cmu_1394_camera cam_;
  int pixel_sample_interval_;//default pixel sample spacing
  vil_memory_image_of< vil_rgb<unsigned char> > rgb_frame_;
  vil_memory_image_of< unsigned char > mono_frame_;
};

struct vvid_live_video_tableau_new : public vvid_live_video_tableau_sptr
{
 public:

  vvid_live_video_tableau_new() :
    vvid_live_video_tableau_sptr(new vvid_live_video_tableau()){}

  vvid_live_video_tableau_new(int node, int pixel_sample_interval,
                              const cmu_1394_camera_params& cp) :
    vvid_live_video_tableau_sptr(new vvid_live_video_tableau(node,
                                                             pixel_sample_interval,
                                                             cp))
  {}

  operator vgui_image_tableau_sptr () const
  { vgui_image_tableau_sptr tt; tt.vertical_cast(*this); return tt; }
};

#endif // vvid_live_video_tableau_h_
