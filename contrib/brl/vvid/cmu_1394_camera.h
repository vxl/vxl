// This is brl/vvid/cmu_1394_camera.h
#ifndef cmu_1394_camera_h_
#define cmu_1394_camera_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief cmu_1394_camera
//
//  A wrapper class for the cmu 1394 camera.  Mainly to set parameters. The
//  approach is inheritance so we don't have to duplicate the CMU camera
//  interface. The parameters can be passed as a block to duplicate camera
//  setup and to conveniently support file I/0.
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy Aug 29, 2002    Initial version.
// \endverbatim
//--------------------------------------------------------------------------------
#include <windows.h>
#include <winbase.h>
#include <1394Camera.h>
#include <vil1/vil1_rgb.h>
#include <vil1/vil1_memory_image_of.h>
#include <vvid/cmu_1394_camera_params.h>
#include <vcl_iosfwd.h>

class cmu_1394_camera : public cmu_1394_camera_params, public C1394Camera
{
 public:
  cmu_1394_camera();
  cmu_1394_camera(int node, const cmu_1394_camera_params& cp);

  ~cmu_1394_camera();

  //camera status
  bool get_camera_present() const { return camera_present_; }
  bool get_running() const { return running_; }
  //basic camera operations
  void update_settings();
  bool init(int node);
  void start();
  void stop();
  bool get_frame();
  bool get_rgb_image(vil1_memory_image_of<vil1_rgb<unsigned char> >& im,
                     int pixel_sample_interval=1, bool reread = true);
  bool get_monochrome_image(vil1_memory_image_of<unsigned char>& im,
                            int pixel_sample_interval =1, bool reread = true);
  friend vcl_ostream& operator << (vcl_ostream& os, const cmu_1394_camera& c);

 private:
  //camera status flags
  bool link_failed_;
  bool camera_present_;
  bool running_;
  bool image_valid_;
};

#endif // cmu_1394_camera_h_
