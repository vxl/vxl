// This is brl/bbas/vidl2/vidl2_v4l_params.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Brendan McCane
// \date   16 Mar 2006
//
//-----------------------------------------------------------------------------

#include "vidl2_pixel_format.h"
#include "vidl2_v4l_params.h"


//: convert from vidl2 pixel format to v4l pixel format
int vidl2_v4l_params::vidl2pf_to_v4lpf(vidl2_pixel_format vid_pf)
{
    switch (vid_pf)
    {
    case VIDL2_PIXEL_FORMAT_UNKNOWN: return -1;
    case VIDL2_PIXEL_FORMAT_RGB_24: return VIDEO_PALETTE_RGB24;
    case VIDL2_PIXEL_FORMAT_RGB_24P: return -1;
    case VIDL2_PIXEL_FORMAT_BGR_24: return -1;
    case VIDL2_PIXEL_FORMAT_RGBA_32: return VIDEO_PALETTE_RGB32;
    case VIDL2_PIXEL_FORMAT_RGBA_32P: return -1;
    case VIDL2_PIXEL_FORMAT_RGB_565: return VIDEO_PALETTE_RGB565;
    case VIDL2_PIXEL_FORMAT_RGB_555: return VIDEO_PALETTE_RGB555;
    case VIDL2_PIXEL_FORMAT_YUV_444P: return -1;
    case VIDL2_PIXEL_FORMAT_YUV_422P: return VIDEO_PALETTE_YUV422P;
    case VIDL2_PIXEL_FORMAT_YUV_420P: return VIDEO_PALETTE_YUV420P;
    case VIDL2_PIXEL_FORMAT_YVU_420P: return -1;
    case VIDL2_PIXEL_FORMAT_YUV_411P: return VIDEO_PALETTE_YUV411P;
    case VIDL2_PIXEL_FORMAT_YUV_410P: return VIDEO_PALETTE_YUV410P;
    case VIDL2_PIXEL_FORMAT_UYV_444: return -1;
    case VIDL2_PIXEL_FORMAT_YUYV_422: return VIDEO_PALETTE_YUYV;
    case VIDL2_PIXEL_FORMAT_UYVY_422: return VIDEO_PALETTE_UYVY;
    case VIDL2_PIXEL_FORMAT_UYVY_411: return -1;
    case VIDL2_PIXEL_FORMAT_MONO_1: return -1;
    case VIDL2_PIXEL_FORMAT_MONO_8: return VIDEO_PALETTE_GREY;
    case VIDL2_PIXEL_FORMAT_MONO_16: return -1;
    default: return -1;
    };
}

//: convert from v4l pixel format to vidl2 pixel format
vidl2_pixel_format vidl2_v4l_params::v4lpf_to_vidl2pf(int v4l_pf)
{
    switch (v4l_pf)
    {
    case VIDEO_PALETTE_RGB24: return VIDL2_PIXEL_FORMAT_RGB_24;
    case VIDEO_PALETTE_RGB32: return VIDL2_PIXEL_FORMAT_RGBA_32;
    case VIDEO_PALETTE_RGB565: return VIDL2_PIXEL_FORMAT_RGB_565;
    case VIDEO_PALETTE_RGB555: return VIDL2_PIXEL_FORMAT_RGB_555;
    case VIDEO_PALETTE_YUV422P: return VIDL2_PIXEL_FORMAT_YUV_422P;
    case VIDEO_PALETTE_YUV420P: return VIDL2_PIXEL_FORMAT_YUV_420P;
    case VIDEO_PALETTE_YUV411P: return VIDL2_PIXEL_FORMAT_YUV_411P;
    case VIDEO_PALETTE_YUV410P: return VIDL2_PIXEL_FORMAT_YUV_410P;
    case VIDEO_PALETTE_YUYV: return VIDL2_PIXEL_FORMAT_YUYV_422;
    case VIDEO_PALETTE_UYVY: return VIDL2_PIXEL_FORMAT_UYVY_422;
    case VIDEO_PALETTE_GREY: return VIDL2_PIXEL_FORMAT_MONO_8;
    default: return VIDL2_PIXEL_FORMAT_UNKNOWN;
    };
}

