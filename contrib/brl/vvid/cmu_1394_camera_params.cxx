//this-sets-emacs-to-*-c++-*-mode
#include <vvid/cmu_1394_camera_params.h>

cmu_1394_camera_params::~cmu_1394_camera_params()
{
}

cmu_1394_camera_params::cmu_1394_camera_params(int video_format, int video_mode,
                                               int frame_rate, int brightness,
                                               int sharpness,
                                               int exposure, int gain,
                                               bool capture, bool rgb)
{
  video_format_=video_format;
  video_mode_=video_mode;
  frame_rate_=frame_rate;
  brightness_=brightness;
  sharpness_=sharpness;
  exposure_=exposure;
  gain_ = gain;
  capture_=capture;
  rgb_=rgb;
  this->constrain();
}

void cmu_1394_camera_params::set_params(const cmu_1394_camera_params& cp)
{
  video_format_=cp.video_format_;
  video_mode_=cp.video_mode_;
  frame_rate_=cp.frame_rate_;
  brightness_=cp.brightness_;
  sharpness_=cp.sharpness_;
  exposure_=cp.exposure_;
  gain_ = cp.gain_;
  capture_=cp.capture_;
  rgb_=cp.rgb_;
  this->constrain();
}

cmu_1394_camera_params::cmu_1394_camera_params(const cmu_1394_camera_params& cp)
{
  this->set_params(cp);
}

//ensure that the parameters are consistent
void cmu_1394_camera_params::constrain()
{
  //rgb vs monochrome - the boolean flag is considered dominant
  if (rgb_)
    video_mode_ = 4;
  else
    video_mode_ = 5;
}

vcl_ostream& operator << (vcl_ostream& os, const cmu_1394_camera_params& cp)
{
  os << "video_format: " << cp.video_format_ << vcl_endl;
  os << "video_mode: " << cp.video_mode_ << vcl_endl;
  os << "frame_rate: " << cp.frame_rate_ << vcl_endl;
  os << "brightness: " << cp.brightness_ << vcl_endl;
  os << "sharpness: " << cp.sharpness_ << vcl_endl;
  os << "exposure: " << cp.exposure_ << vcl_endl;
  os << "capture: " << cp.capture_ << vcl_endl;
  os << "gain: " << cp.gain_ << vcl_endl;
  os << "rgb: " << cp.rgb_ << vcl_endl;
  return os;
}
