// This is brl/vvid/cmu_1394_camera_params.cxx
#include "cmu_1394_camera_params.h"
#include <vcl_iostream.h>

cmu_1394_camera_params::~cmu_1394_camera_params()
{
}

cmu_1394_camera_params::cmu_1394_camera_params(int video_format,
                                               int video_mode,
                                               int frame_rate,
                                               bool auto_exposure,
                                               bool auto_gain,
                                               bool manual_shutter_control,
                                               bool auto_exposure_control,
                                               int shutter,
                                               int min_shutter,
                                               int max_shutter,
                                               int gain,
                                               int min_gain,
                                               int max_gain,
                                               int brightness,
                                               int min_brightness,
                                               int max_brightness,
                                               int sharpness,
                                               int min_sharpness,
                                               int max_sharpness,
                                               int exposure,
                                               int min_exposure,
                                               int max_exposure,
                                               bool capture,
                                               bool rgb,
                                               bool autowhitebalance,
                                               int whitebalanceU,
                                               int whitebalanceV,
                                               bool onepushWBbalance,
                                               int min_WB,
                                               int max_WB)
 
{ 

 video_format_ = video_format;   
 video_mode_ = video_mode;       
 frame_rate_ = frame_rate;
 auto_exposure_ = auto_exposure;
 auto_gain_ = auto_gain;
  manual_shutter_control_ = manual_shutter_control;
  shutter_ = shutter;
  min_shutter_ = min_shutter;
  max_shutter_ = max_shutter;
  gain_ = gain;
  min_gain_ = min_gain;
  max_gain_ = max_gain;
  brightness_ = brightness;
  min_brightness_ = min_brightness;
  max_brightness_ = max_brightness;
  sharpness_ = sharpness;
  min_sharpness_ = min_sharpness;
  max_sharpness_ = max_sharpness;
  auto_exposure_control_ = auto_exposure_control;
  exposure_ = exposure;
  min_exposure_ = min_exposure;
  max_exposure_ = max_exposure;
  capture_ = capture;
  rgb_ = rgb;
  autowhitebalance_=autowhitebalance;
  whitebalanceU_=whitebalanceU;
  whitebalanceV_=whitebalanceV;
  onepushWBbalance_=onepushWBbalance;
  min_WB_=min_WB;
  max_WB_=max_WB;
}

void cmu_1394_camera_params::set_params(const cmu_1394_camera_params& cp)
{
  video_format_ = cp.video_format_;
  video_mode_ = cp.video_mode_;
  frame_rate_ = cp.frame_rate_;
  auto_exposure_ = cp.auto_exposure_;
  auto_gain_ = cp.auto_gain_;
  manual_shutter_control_ =  cp.manual_shutter_control_;
  shutter_ = cp.shutter_;
  min_shutter_ = cp.min_shutter_;
  max_shutter_ = cp.max_shutter_;
  gain_ = cp.gain_;
  min_gain_ = cp.min_gain_;
  max_gain_ = cp.max_gain_;
  brightness_ = cp.brightness_;
  min_brightness_ = cp.min_brightness_;
  max_brightness_ = cp.max_brightness_;
  sharpness_ = cp.sharpness_;
  min_sharpness_ = cp.min_sharpness_;
  max_sharpness_ = cp.max_sharpness_;
  auto_exposure_control_ = cp.auto_exposure_control_;
  exposure_ = cp.exposure_;
  min_exposure_ = cp.min_exposure_;
  max_exposure_ = cp.max_exposure_;
  capture_ = cp.capture_;
  rgb_ = cp.rgb_;
  autowhitebalance_=cp.autowhitebalance_;
  whitebalanceU_=cp.whitebalanceU_;
  whitebalanceV_=cp.whitebalanceV_;
  onepushWBbalance_=cp.onepushWBbalance_;
  min_WB_=cp.min_WB_;
  max_WB_=cp.max_WB_;
}

cmu_1394_camera_params::cmu_1394_camera_params(const cmu_1394_camera_params& cp)
{
  this->set_params(cp);
}

//ensure that the parameters are consistent with the bounds
void cmu_1394_camera_params::constrain()
{
  if (!manual_shutter_control_&&auto_exposure_control_)
    auto_exposure_ = true;

  //shutter
  if (shutter_<min_shutter_)
    shutter_ = min_shutter_;
  if (shutter_>max_shutter_)
    shutter_ = max_shutter_;
  //gain
  if (gain_<min_gain_)
    gain_ = min_gain_;
  if (gain_>max_gain_)
    gain_ = max_gain_;
  //brightness
  if (brightness_<min_brightness_)
    brightness_ = min_brightness_;
  if (brightness_>max_brightness_)
    brightness_ = max_brightness_;
  //sharpness
  if (sharpness_<min_sharpness_)
    sharpness_ = min_sharpness_;
  if (sharpness_>max_sharpness_)
    sharpness_ = max_sharpness_;
  //exposure
  if (exposure_<min_exposure_)
    exposure_ = min_exposure_;
  if (exposure_>max_exposure_)
    exposure_ = max_exposure_;
}

vcl_string cmu_1394_camera_params::
video_configuration(const int video_format, const int video_mode) const
{
  if (video_format<0||video_mode<0)
    return "";
  else if (video_format==0)
    switch (video_mode)
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
  else if (video_format==1)
    switch (video_mode)
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
  else if (video_format==2)
    switch (video_mode)
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
  else if (video_format==7)
    return "Partial Scan (ROIs)";
  else
    return "";
}

vcl_string cmu_1394_camera_params::frame_rate(const int rate_code) const
{
  switch (rate_code)
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

vcl_ostream& operator<<(vcl_ostream& os, const cmu_1394_camera_params& cp)
{
  return
  os << "video_format: " << cp.video_format_
     << "\nvideo_mode: " << cp.video_mode_
     << "\nframe_rate: " << cp.frame_rate_
     << "\nmanual shutter control: "<< (cp.manual_shutter_control_?"YES" : "NO")
     << " | auto exposure control: "<< (cp.auto_exposure_control_ ?"YES" : "NO")
     << "\nauto_exposure: "<< (cp.auto_exposure_ ? "ON" : "OFF")
     << " | auto_gain: "   << (cp.auto_gain_     ? "ON" : "OFF")
     << "\nshutter: " << cp.min_shutter_ << " < " << cp.shutter_
     <<         " < " << cp.max_shutter_
     << "\ngain: " << cp.min_gain_ << " < " << cp.gain_ << " < " << cp.max_gain_
     << "\nbrightness: " << cp.min_brightness_ << " < " << cp.brightness_
     <<            " < " << cp.max_brightness_
     << "\nsharpness: " << cp.min_sharpness_ << " < " << cp.sharpness_
     <<           " < " << cp.max_sharpness_
     << "\nexposure: " << cp.min_exposure_ << " < " << cp.exposure_
     <<          " < " << cp.max_exposure_
     << "\ncapture: " << cp.capture_
     << "\nrgb: " << cp.rgb_ 
     << "\nWB U "<<cp.min_WB_<<"<"<<cp.whitebalanceU_<<">"<<cp.max_WB_
     << "\nWB V "<<cp.min_WB_<<"<"<<cp.whitebalanceV_<<">"<<cp.max_WB_<<vcl_endl;
}
