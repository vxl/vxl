// This is brl/bbas/vidl2/v4l2_pixel_format.cxx
//:
// \file
//
// \author Antonio Garrido
// \verbatim
//  Modifications
//     15 Apr 2008 Created (A. Garrido)
//\endverbatim

#include "v4l2_pixel_format.h"

extern "C" {
#include <asm/types.h>          /* for videodev2.h */
#include <sys/time.h>
#include <linux/videodev2.h>
};

// TO DO: Comparison between videl2_pixel_format.h and
// http://v4l2spec.bytesex.org/spec-single/v4l2.html#AEN4850 !!!
// correspondences extracted from vidl2_v4l and intuition.... not sure about them :(
int vidl2_to_v4l2 (vidl2_pixel_format pf)
{
  switch (pf)
  {
    case VIDL2_PIXEL_FORMAT_UNKNOWN: return -1;
    case VIDL2_PIXEL_FORMAT_RGB_24: return V4L2_PIX_FMT_BGR24;
    case VIDL2_PIXEL_FORMAT_RGB_24P: return -1;
    case VIDL2_PIXEL_FORMAT_BGR_24: return -1;
    case VIDL2_PIXEL_FORMAT_RGBA_32: return  V4L2_PIX_FMT_BGR32;
    case VIDL2_PIXEL_FORMAT_RGBA_32P: return -1;
    case VIDL2_PIXEL_FORMAT_RGB_565: return  V4L2_PIX_FMT_RGB565;
    case VIDL2_PIXEL_FORMAT_RGB_555: return  V4L2_PIX_FMT_RGB555;
    case VIDL2_PIXEL_FORMAT_YUV_444P: return -1;
    case VIDL2_PIXEL_FORMAT_YUV_422P: return  V4L2_PIX_FMT_YUV422P;
    case VIDL2_PIXEL_FORMAT_YUV_420P: return  V4L2_PIX_FMT_YUV420;
    case VIDL2_PIXEL_FORMAT_YVU_420P: return  V4L2_PIX_FMT_YVU420; 
    case VIDL2_PIXEL_FORMAT_YUV_411P: return  V4L2_PIX_FMT_YUV411P;
    case VIDL2_PIXEL_FORMAT_YUV_410P: return  V4L2_PIX_FMT_YVU410;
    case VIDL2_PIXEL_FORMAT_UYV_444: return -1;
    case VIDL2_PIXEL_FORMAT_YUYV_422: return  V4L2_PIX_FMT_YUYV;
    case VIDL2_PIXEL_FORMAT_UYVY_422: return  V4L2_PIX_FMT_UYVY;
    case VIDL2_PIXEL_FORMAT_UYVY_411: return -1;
    case VIDL2_PIXEL_FORMAT_MONO_1: return -1;
    case VIDL2_PIXEL_FORMAT_MONO_8: return  V4L2_PIX_FMT_GREY ;
  //  case VIDL2_PIXEL_FORMAT_MONO_16: return V4L2_PIX_FMT_Y16; // ?????????return -1;
    default: return -1;
  };
}



vidl2_pixel_format v4l2_to_vidl2(int pf)
{
  switch (pf)
  {
    case V4L2_PIX_FMT_BGR24: return VIDL2_PIXEL_FORMAT_RGB_24;
    case V4L2_PIX_FMT_BGR32: return VIDL2_PIXEL_FORMAT_RGBA_32;
    case V4L2_PIX_FMT_RGB565: return VIDL2_PIXEL_FORMAT_RGB_565;
    case V4L2_PIX_FMT_RGB555: return VIDL2_PIXEL_FORMAT_RGB_555;
    case V4L2_PIX_FMT_YUV422P: return VIDL2_PIXEL_FORMAT_YUV_422P;
    case V4L2_PIX_FMT_YUV420: return VIDL2_PIXEL_FORMAT_YUV_420P;
    case V4L2_PIX_FMT_YVU420: return VIDL2_PIXEL_FORMAT_YVU_420P;
    case V4L2_PIX_FMT_YUV411P: return VIDL2_PIXEL_FORMAT_YUV_411P;
    case V4L2_PIX_FMT_YVU410: return VIDL2_PIXEL_FORMAT_YUV_410P;
    case V4L2_PIX_FMT_YUYV: return VIDL2_PIXEL_FORMAT_YUYV_422;
    case V4L2_PIX_FMT_UYVY: return VIDL2_PIXEL_FORMAT_UYVY_422;
    case V4L2_PIX_FMT_GREY: return VIDL2_PIXEL_FORMAT_MONO_8;
    //case V4L2_PIX_FMT_Y16: return VIDL2_PIXEL_FORMAT_MONO_16; //????
    default: return VIDL2_PIXEL_FORMAT_UNKNOWN;
  };
}
