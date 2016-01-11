// This is core/vidl/vidl_v4l2_pixel_format.cxx
//:
// \file
//
// \author Antonio Garrido
// \verbatim
//  Modifications
//     15 Apr 2008 Created (A. Garrido)
//\endverbatim

#include "vidl_v4l2_pixel_format.h"

extern "C" {
//#include <asm/types.h>          /* for videodev2.h */
#include <sys/time.h>
#include <linux/videodev2.h>
};

// TO DO: Comparison between videl2_pixel_format.h and
// http://v4l2spec.bytesex.org/spec-single/v4l2.html#AEN4850 !!!
// correspondences extracted from vidl_v4l and intuition.... not sure about them :(
unsigned int vidl_to_v4l2 (vidl_pixel_format pf)
{
  switch (pf)
  {
    case VIDL_PIXEL_FORMAT_UNKNOWN: return 0;
    case VIDL_PIXEL_FORMAT_RGB_24: return V4L2_PIX_FMT_BGR24;
    case VIDL_PIXEL_FORMAT_RGB_24P: return 0;
    case VIDL_PIXEL_FORMAT_BGR_24: return 0;
    case VIDL_PIXEL_FORMAT_RGBA_32: return  V4L2_PIX_FMT_BGR32;
    case VIDL_PIXEL_FORMAT_RGBA_32P: return 0;
    case VIDL_PIXEL_FORMAT_RGB_565: return  V4L2_PIX_FMT_RGB565;
    case VIDL_PIXEL_FORMAT_RGB_555: return  V4L2_PIX_FMT_RGB555;
    case VIDL_PIXEL_FORMAT_YUV_444P: return 0;
    case VIDL_PIXEL_FORMAT_YUV_422P: return  V4L2_PIX_FMT_YUV422P;
    case VIDL_PIXEL_FORMAT_YUV_420P: return  V4L2_PIX_FMT_YUV420;
    case VIDL_PIXEL_FORMAT_YVU_420P: return  V4L2_PIX_FMT_YVU420;
    case VIDL_PIXEL_FORMAT_YUV_411P: return  V4L2_PIX_FMT_YUV411P;
    case VIDL_PIXEL_FORMAT_YUV_410P: return  V4L2_PIX_FMT_YVU410;
    case VIDL_PIXEL_FORMAT_UYV_444: return 0;
    case VIDL_PIXEL_FORMAT_YUYV_422: return  V4L2_PIX_FMT_YUYV;
    case VIDL_PIXEL_FORMAT_UYVY_422: return  V4L2_PIX_FMT_UYVY;
    case VIDL_PIXEL_FORMAT_UYVY_411: return 0;
    case VIDL_PIXEL_FORMAT_MONO_1: return -1;
    case VIDL_PIXEL_FORMAT_MONO_8: return  V4L2_PIX_FMT_GREY ;
  //  case VIDL_PIXEL_FORMAT_MONO_16: return V4L2_PIX_FMT_Y16; // ?????????return -1;
    default: return -1;
  };
}



vidl_pixel_format v4l2_to_vidl(unsigned int pf)
{
  switch (pf)
  {
    case V4L2_PIX_FMT_BGR24: return VIDL_PIXEL_FORMAT_RGB_24;
    case V4L2_PIX_FMT_BGR32: return VIDL_PIXEL_FORMAT_RGBA_32;
    case V4L2_PIX_FMT_RGB565: return VIDL_PIXEL_FORMAT_RGB_565;
    case V4L2_PIX_FMT_RGB555: return VIDL_PIXEL_FORMAT_RGB_555;
    case V4L2_PIX_FMT_YUV422P: return VIDL_PIXEL_FORMAT_YUV_422P;
    case V4L2_PIX_FMT_YUV420: return VIDL_PIXEL_FORMAT_YUV_420P;
    case V4L2_PIX_FMT_YVU420: return VIDL_PIXEL_FORMAT_YVU_420P;
    case V4L2_PIX_FMT_YUV411P: return VIDL_PIXEL_FORMAT_YUV_411P;
    case V4L2_PIX_FMT_YVU410: return VIDL_PIXEL_FORMAT_YUV_410P;
    case V4L2_PIX_FMT_YUYV: return VIDL_PIXEL_FORMAT_YUYV_422;
    case V4L2_PIX_FMT_UYVY: return VIDL_PIXEL_FORMAT_UYVY_422;
    case V4L2_PIX_FMT_GREY: return VIDL_PIXEL_FORMAT_MONO_8;
    //case V4L2_PIX_FMT_Y16: return VIDL_PIXEL_FORMAT_MONO_16; //????
    default: return VIDL_PIXEL_FORMAT_UNKNOWN;
  };
}
