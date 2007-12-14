// This is brl/bbas/vidl2/vidl2_iidc1394_params.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Matt Leotta
// \date   10 Jan 2006
//
//-----------------------------------------------------------------------------

#include "vidl2_iidc1394_params.h"
#include <vcl_cassert.h>

//-----------------------------------------------------------------------------


vidl2_iidc1394_params::vidl2_iidc1394_params()
  : guid_( 0 ),
    speed_( ISO_SPEED_400 ),
    b_mode_(false),
    frame_rate_( FRAMERATE_15 ),
    video_mode_( MODE_640x480_RGB8 )
{
}


//: Return the speed value for a speed setting
unsigned int
vidl2_iidc1394_params::
speed_val(speed_t s)
{
  switch (s)
  {
    case ISO_SPEED_100:  return 100;
    case ISO_SPEED_200:  return 200;
    case ISO_SPEED_400:  return 400;
    case ISO_SPEED_800:  return 800;
    case ISO_SPEED_1600: return 1600;
    case ISO_SPEED_3200: return 3200;
  }
  return 0;
}


//: Return the frame rate as a floating point value
float
vidl2_iidc1394_params::
frame_rate_val(frame_rate_t r)
{
  switch (r)
  {
    case FRAMERATE_1_875: return 1.875f;
    case FRAMERATE_3_75:  return 3.75f;
    case FRAMERATE_7_5:   return 7.5f;
    case FRAMERATE_15:    return 15.0f;
    case FRAMERATE_30:    return 30.0f;
    case FRAMERATE_60:    return 60.0f;
    case FRAMERATE_120:   return 120.0f;
    case FRAMERATE_240:   return 240.0f;
  }
  return 0.0f;
}


//: Return string describing the mode
vcl_string
vidl2_iidc1394_params::
video_mode_string(video_mode_t m)
{
  switch (m)
  {
    case MODE_160x120_YUV444:   return "160x120 YUV(4:4:4)";
    case MODE_320x240_YUV422:   return "320x240 YUV(4:2:2)";
    case MODE_640x480_YUV411:   return "640x480 YUV(4:1:1)";
    case MODE_640x480_YUV422:   return "640x480 YUV(4:2:2)";
    case MODE_640x480_RGB8:     return "640x480 RGB";
    case MODE_640x480_MONO8:    return "640x480 Mono(8 bit)";
    case MODE_640x480_MONO16:   return "640x480 Mono(16 bit)";
    case MODE_800x600_YUV422:   return "800x600 YUV(4:2:2)";
    case MODE_800x600_RGB8:     return "800x600 RGB";
    case MODE_800x600_MONO8:    return "800x600 Mono(8 bit)";
    case MODE_1024x768_YUV422:  return "1024x768 YUV(4:2:2)";
    case MODE_1024x768_RGB8:    return "1024x768 RGB";
    case MODE_1024x768_MONO8:   return "1024x768 Mono(8 bit)";
    case MODE_800x600_MONO16:   return "800x600 Mono(16 bit)";
    case MODE_1024x768_MONO16:  return "1024x768 Mono(16 bit)";
    case MODE_1280x960_YUV422:  return "1280x960 YUV(4:2:2)";
    case MODE_1280x960_RGB8:    return "1280x960 RGB";
    case MODE_1280x960_MONO8:   return "1280x960 Mono(8 bit)";
    case MODE_1600x1200_YUV422: return "1600x1200 YUV(4:2:2)";
    case MODE_1600x1200_RGB8:   return "1600x1200 RGB";
    case MODE_1600x1200_MONO8:  return "1600x1200 Mono(8 bit)";
    case MODE_1280x960_MONO16:  return "1280x960 Mono(16 bit)";
    case MODE_1600x1200_MONO16: return "1600x1200 Mono(16 bit)";
    case MODE_EXIF:      return "Exif";
    case MODE_FORMAT7_0: return "Format 7 : Mode 0";
    case MODE_FORMAT7_1: return "Format 7 : Mode 1";
    case MODE_FORMAT7_2: return "Format 7 : Mode 2";
    case MODE_FORMAT7_3: return "Format 7 : Mode 3";
    case MODE_FORMAT7_4: return "Format 7 : Mode 4";
    case MODE_FORMAT7_5: return "Format 7 : Mode 5";
    case MODE_FORMAT7_6: return "Format 7 : Mode 6";
    case MODE_FORMAT7_7: return "Format 7 : Mode 7";
  }
  return "invalid mode";
}


//: Return string describing the feature
vcl_string
vidl2_iidc1394_params::
feature_string(feature_t f)
{
  switch (f)
  {
    case FEATURE_BRIGHTNESS:      return "Brightness";
    case FEATURE_EXPOSURE:        return "Exposure";
    case FEATURE_SHARPNESS:       return "Sharpness";
    case FEATURE_WHITE_BALANCE:   return "White Balance";
    case FEATURE_HUE:             return "Hue";
    case FEATURE_SATURATION:      return "Saturation";
    case FEATURE_GAMMA:           return "Gamma";
    case FEATURE_SHUTTER:         return "Shutter";
    case FEATURE_GAIN:            return "Gain";
    case FEATURE_IRIS:            return "Iris";
    case FEATURE_FOCUS:           return "Focus";
    case FEATURE_TEMPERATURE:     return "Temperature";
    case FEATURE_TRIGGER:         return "Trigger";
    case FEATURE_TRIGGER_DELAY:   return "Trigger Delay";
    case FEATURE_WHITE_SHADING:   return "White Shading";
    case FEATURE_FRAME_RATE:      return "Frame Rate";
    case FEATURE_ZOOM:            return "Zoom";
    case FEATURE_PAN:             return "Pan";
    case FEATURE_TILT:            return "Tilt";
    case FEATURE_OPTICAL_FILTER:  return "Optical Filter";
    case FEATURE_CAPTURE_SIZE:    return "Capture Size";
    case FEATURE_CAPTURE_QUALITY: return "Capture Quality";
  }
  return "invalid feature";
}


//: Return string describing the feature mode
vcl_string
vidl2_iidc1394_params::
feature_mode_string(feature_mode_t fm)
{
  switch (fm)
  {
    case FEATURE_MODE_MANUAL:         return "Manual";
    case FEATURE_MODE_AUTO:           return "Automatic";
    case FEATURE_MODE_ONE_PUSH_AUTO:  return "One Push";
  }
  return "invalid feature mode";
}


//: Return the format number from the video mode enumeration
unsigned int
vidl2_iidc1394_params::
video_format_val(video_mode_t m)
{
  if ((m>=MODE_160x120_YUV444)&&(m<=MODE_640x480_MONO16)) {
    return 0;
  }
  else if ((m>=MODE_800x600_YUV422)&&(m<=MODE_1024x768_MONO16)) {
    return 1;
  }
  else if ((m>=MODE_1280x960_YUV422)&&(m<=MODE_1600x1200_MONO16)) {
    return 2;
  }
  else if (m==MODE_EXIF) {
    return 6;
  }
  else if ((m>=MODE_FORMAT7_0)&&(m<=MODE_FORMAT7_7)) {
    return 7;
  }
  // unknown format
  return unsigned(-1);
}


//: Return the mode number from the video mode enumeration
unsigned int
vidl2_iidc1394_params::
video_mode_val(video_mode_t m)
{
  if ((m>=MODE_160x120_YUV444)&&(m<=MODE_640x480_MONO16)) {
    return m - MODE_160x120_YUV444;
  }
  else if ((m>=MODE_800x600_YUV422)&&(m<=MODE_1024x768_MONO16)) {
    return m - MODE_800x600_YUV422;
  }
  else if ((m>=MODE_1280x960_YUV422)&&(m<=MODE_1600x1200_MONO16)) {
    return m - MODE_1280x960_YUV422;
  }
  else if (m==MODE_EXIF) {
    return 0;
  }
  else if ((m>=MODE_FORMAT7_0)&&(m<=MODE_FORMAT7_7)) {
    return m - MODE_FORMAT7_0;
  }
  // unknown mode value
  return unsigned(-1);
}


//: Return the video mode enumeration for a format and mode
vidl2_iidc1394_params::video_mode_t
vidl2_iidc1394_params::
video_mode(unsigned int format, unsigned int mode)
{
  assert(mode < 8);
  switch(format){
    case 0:
      return video_mode_t(MODE_160x120_YUV444 + mode);
    case 1:
      return video_mode_t(MODE_800x600_YUV422 + mode);
    case 2:
      return video_mode_t(MODE_1280x960_YUV422 + mode);
    case 6:
      return MODE_EXIF;
    case 7:
      return video_mode_t(MODE_FORMAT7_0 + mode);
    default:
      break;
  }

  assert(false);
  return MODE_FORMAT7_0;
}


//: Return the pixel format of the video mode
vidl2_pixel_format
vidl2_iidc1394_params::pixel_format(video_mode_t m)
{
  switch (m)
  {
    case MODE_160x120_YUV444:
      return VIDL2_PIXEL_FORMAT_UYV_444;

    case MODE_320x240_YUV422:
    case MODE_640x480_YUV422:
    case MODE_800x600_YUV422:
    case MODE_1024x768_YUV422:
    case MODE_1280x960_YUV422:
    case MODE_1600x1200_YUV422:
      return VIDL2_PIXEL_FORMAT_UYVY_422;

    case MODE_640x480_YUV411:
      return VIDL2_PIXEL_FORMAT_UYVY_411;

    case MODE_640x480_RGB8:
    case MODE_800x600_RGB8:
    case MODE_1024x768_RGB8:
    case MODE_1280x960_RGB8:
    case MODE_1600x1200_RGB8:
      return VIDL2_PIXEL_FORMAT_RGB_24;

    case MODE_640x480_MONO8:
    case MODE_800x600_MONO8:
    case MODE_1024x768_MONO8:
    case MODE_1280x960_MONO8:
    case MODE_1600x1200_MONO8:
      return VIDL2_PIXEL_FORMAT_MONO_8;

    case MODE_640x480_MONO16:
    case MODE_800x600_MONO16:
    case MODE_1024x768_MONO16:
    case MODE_1280x960_MONO16:
    case MODE_1600x1200_MONO16:
      return VIDL2_PIXEL_FORMAT_MONO_16;

    default:
      break;
  }
  return VIDL2_PIXEL_FORMAT_UNKNOWN;
}


//: Return the image resolution of the video mode
bool
vidl2_iidc1394_params::resolution(video_mode_t m, unsigned& ni, unsigned& nj)
{
  switch(m){
    case MODE_160x120_YUV444:
      ni = 160;  nj = 120;  break;
    case MODE_320x240_YUV422:
      ni = 320;  nj = 240;  break;
    case MODE_640x480_YUV411:
    case MODE_640x480_YUV422:
    case MODE_640x480_RGB8:
    case MODE_640x480_MONO8:
    case MODE_640x480_MONO16:
      ni = 640;  nj = 480;  break;
    case MODE_800x600_YUV422:
    case MODE_800x600_RGB8:
    case MODE_800x600_MONO8:
    case MODE_800x600_MONO16:
      ni = 800;  nj = 600;  break;
    case MODE_1024x768_YUV422:
    case MODE_1024x768_RGB8:
    case MODE_1024x768_MONO8:
    case MODE_1024x768_MONO16:
      ni = 1024;  nj = 768;  break;
    case MODE_1280x960_YUV422:
    case MODE_1280x960_RGB8: 
    case MODE_1280x960_MONO8:
    case MODE_1280x960_MONO16:
      ni = 1280;  nj = 960;  break;
    case MODE_1600x1200_YUV422:
    case MODE_1600x1200_RGB8:
    case MODE_1600x1200_MONO8:
    case MODE_1600x1200_MONO16:
      ni = 1600;  nj = 1200;  break;
    default:
      return false;
  }
  return true;
}
