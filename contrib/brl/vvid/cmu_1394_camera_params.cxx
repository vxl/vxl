//this-sets-emacs-to-*-c++-*-mode
#include <vvid/cmu_1394_camera_params.h>

cmu_1394_camera_params::~cmu_1394_camera_params(){

}

cmu_1394_camera_params::cmu_1394_camera_params(int video_format, int video_mode,
                                               int frame_rate, int brightness,
                                               int sharpness,
                                               int exposure, int gain,
                                               bool capture, bool rgb)
{
  _video_format=video_format;
  _video_mode=video_mode;
  _frame_rate=frame_rate;
  _brightness=brightness;
  _sharpness=sharpness;
  _exposure=exposure;
  _gain = gain;
  _capture=capture;
  _rgb=rgb;
  this->constrain();
}
void cmu_1394_camera_params::set_params(const cmu_1394_camera_params& cp)
{
  _video_format=cp._video_format;
  _video_mode=cp._video_mode;
  _frame_rate=cp._frame_rate;
  _brightness=cp._brightness;
  _sharpness=cp._sharpness;
  _exposure=cp._exposure;
  _gain = cp._gain;
  _capture=cp._capture;
  _rgb=cp._rgb;
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
  if(_rgb)
    _video_mode = 4;
  else
    _video_mode = 5;
}
vcl_ostream& operator << (vcl_ostream& os, const cmu_1394_camera_params& cp)
{
  os << "video_format: " << cp._video_format << vcl_endl;
  os << "video_mode: " << cp._video_mode << vcl_endl;
  os << "frame_rate: " << cp._frame_rate << vcl_endl;  
  os << "brightness: " << cp._brightness << vcl_endl;
  os << "sharpness: " << cp._sharpness << vcl_endl;
  os << "exposure: " << cp._exposure << vcl_endl;
  os << "capture: " << cp._capture << vcl_endl;
  os << "gain: " << cp._gain << vcl_endl;
  os << "rgb: " << cp._rgb << vcl_endl;
  return os;
}
