// This is brl/vvid/cmu_1394_camera_params.h
#ifndef cmu_1394_camera_params_h_
#define cmu_1394_camera_params_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief cmu_1394_camera_params
//
//  A parameter block for CMU's 1394 camera class. This block supports
//  both menu operations and file-based configuration. The acquisition
//  mode is constrained to match the rgb/monochrome flag.
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy Aug 29, 2002    Initial version.
// \endverbatim
//--------------------------------------------------------------------------------
#include <vcl_iosfwd.h>
#include <vcl_string.h>
class cmu_1394_camera_params
{
 public:
  cmu_1394_camera_params(int video_format=1, int video_mode=3,
                         int frame_rate=3,
                         bool auto_exposure = false, bool auto_gain= false,
                         bool manual_shutter_control = true,
                         bool auto_exposure_control = true,
                         int shutter = 2750,
                         int min_shutter = 2750, int max_shutter = 2840,
                         int gain = 2048,
                         int min_gain = 2000, int max_gain = 2048,
                         int brightness=96,
                         int min_brightness=0, int max_brightness = 255,
                         int sharpness=3,
                         int min_sharpness=0, int max_sharpness = 128,
                         int exposure=128,
                         int min_exposure=0,int max_exposure = 255,
                         bool capture = true, bool rgb = true);
  cmu_1394_camera_params(const cmu_1394_camera_params& cp);
  ~cmu_1394_camera_params();
  void set_params(const cmu_1394_camera_params& cp);
  void constrain();//!< make sure the parameters are consistent
  vcl_string video_configuration(const int video_format, const int video_mode) const;
  vcl_string frame_rate(const int rate_code) const;
  friend
    vcl_ostream& operator<<(vcl_ostream& os, const cmu_1394_camera_params& cpp);

  //: video configuration (resolution, frame rate, color sampling)
  int video_format_;
  int video_mode_;
  int frame_rate_;
  //:image collection (exposure, gain and offset (brighness))

  bool auto_exposure_;
  bool auto_gain_;

  bool manual_shutter_control_;
  int shutter_;
  int min_shutter_;
  int max_shutter_;

  int gain_;
  int min_gain_;
  int max_gain_;

  int brightness_;
  int min_brightness_;
  int max_brightness_;

  bool auto_exposure_control_;
  int exposure_;
  int min_exposure_;
  int max_exposure_;

  int sharpness_;
  int min_sharpness_;
  int max_sharpness_;

  //:capture vs acquisition (multiple frame buffering vs single frame)
  bool capture_;
  //:color image vs monochrome as a display mode (not collection)
  bool rgb_;
};

#endif // cmu_1394_camera_params_h_
