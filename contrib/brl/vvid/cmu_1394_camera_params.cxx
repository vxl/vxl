//this-sets-emacs-to-*-c++-*-mode
#include <vvid/cmu_1394_camera_params.h>

cmu_1394_camera_params::~cmu_1394_camera_params()
{
}

cmu_1394_camera_params::cmu_1394_camera_params(int video_format,int video_mode,
                                               int frame_rate, int shutter,
                                               int brightness, int sharpness,
                                               int exposure, int gain,
                                               bool capture, bool rgb)
{
  video_format_=video_format;
  video_mode_=video_mode;
  frame_rate_=frame_rate;
  shutter_ = shutter;
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
  shutter_=cp.shutter_;
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
  return; //JLM
  //rgb vs monochrome - the boolean flag is considered dominant
  if (rgb_)
    video_mode_ = 4;
  else
    video_mode_ = 5;
}
vcl_string cmu_1394_camera_params::
video_configuration(const int video_format, const int video_mode) const
{
  if(video_format<0||video_mode<0)
    return "";
  if(video_format==0)
    switch(video_mode)
      {
      case 0:
      return "160x120 YUV(4:4:4)";              
      case 1:
      return "320x240 YUV(4:2:2)";
      case 2:
        return "640x480 YUV(4:1:1)";
      case 3:
        return "640x480 YUV(4:2:2)";
      case 4:
        return "640x480 RGB";
      case 5:
        return "640x480 Mono";
      default:
        return "";
      }
  if(video_format==1)
    switch(video_mode)
      {
      case 0:
        return "800x600 YUV(4:2:2)" ;
      case 1:
        return "800x600 RGB";
      case 2:
        return "800x600 Mono";
      case 3:
        return "1024x768 YUV(4:2:2)";
      case 4:
        return "1024x768 RGB";
      case 5:
        return "1024x768 Mono(8 bits)";
      default:
        return "";
      }
  if(video_format==2);
    switch(video_mode)
      {
      case 0:
        return "1280x960 YUV(4:2:2)";
      case 1:
        return "1280x960 RGB";
      case 2:
        return "1280x960 Mono";
      case 3:
        return "1600x1200 YUV(4:2:2)";
      case 4:
        return "1600x1200 RGB";
      case 5:
        return "1600x1200 Mono";
      default:
        return "";
     }
    if(video_format==7)
      return "Partial Scan (ROIs)";
}
vcl_string cmu_1394_camera_params::frame_rate(const int rate_code) const
{
  switch(rate_code)
    {
    case 0:
      return "1.875";
    case 1:
      return "3.75";
    case 2: 
      return "7.5";
    case 3: 
      return "15.0";
    case 4:
      return "30.0";
    case 5:
      return "60.0";
    default:
      return "";
    }
}
vcl_ostream& operator << (vcl_ostream& os, const cmu_1394_camera_params& cp)
{
  os << "video_format: " << cp.video_format_ << vcl_endl;
  os << "video_mode: " << cp.video_mode_ << vcl_endl;
  os << "frame_rate: " << cp.frame_rate_ << vcl_endl;
  os << "shutter: " << cp.shutter_ << vcl_endl;
  os << "brightness: " << cp.brightness_ << vcl_endl;
  os << "sharpness: " << cp.sharpness_ << vcl_endl;
  os << "exposure: " << cp.exposure_ << vcl_endl;
  os << "capture: " << cp.capture_ << vcl_endl;
  os << "gain: " << cp.gain_ << vcl_endl;
  os << "rgb: " << cp.rgb_ << vcl_endl;
  return os;
}
