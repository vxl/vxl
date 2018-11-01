// This is core/vidl/vidl_ffmpeg_pixel_format.cxx
//:
// \file
// \author Matt Leotta
// \author Gehua Yang
// \date 19 Jan 2016

//
//-----------------------------------------------------------------------------

#include <cstring>
#include <iostream>
#include "vidl_ffmpeg_pixel_format.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


#ifdef LIBAVFORMAT_BUILD
#if LIBAVFORMAT_BUILD <= 4623
// PIX_FMT_NONE is undefined in old versions of FFMPEG
#define PIX_FMT_NONE PixelFormat(-1)
#endif
#endif

#if LIBAVCODEC_BUILD >= ((52<<16)+(10<<8)+0)  // after ver 52.10.0
extern "C" {
#include <libswscale/swscale.h>
}
#endif

//--------------------------------------------------------------------------------

#if LIBAVCODEC_BUILD < ((54<<16)+(31<<8)+0) // before ver 54.31.0

// The enum values in the old versions have the form PIX_FMT_*,
// whereas the new ones are AV_PIX_FMT_*.
// Use macro definitions so that the code works on both.
#define AV_PIX_FMT_NONE          PIX_FMT_NONE
#define AV_PIX_FMT_YUV420P       PIX_FMT_YUV420P
#define AV_PIX_FMT_YUYV422       PIX_FMT_YUYV422
#define AV_PIX_FMT_RGB24         PIX_FMT_RGB24
#define AV_PIX_FMT_BGR24         PIX_FMT_BGR24
#define AV_PIX_FMT_YUV422P       PIX_FMT_YUV422P
#define AV_PIX_FMT_YUV444P       PIX_FMT_YUV444P
#define AV_PIX_FMT_YUV410P       PIX_FMT_YUV410P
#define AV_PIX_FMT_YUV411P       PIX_FMT_YUV411P
#define AV_PIX_FMT_GRAY8         PIX_FMT_GRAY8
#define AV_PIX_FMT_MONOWHITE     PIX_FMT_MONOWHITE
#define AV_PIX_FMT_MONOBLACK     PIX_FMT_MONOBLACK
#define AV_PIX_FMT_PAL8          PIX_FMT_PAL8
#define AV_PIX_FMT_YUVJ420P      PIX_FMT_YUVJ420P
#define AV_PIX_FMT_YUVJ422P      PIX_FMT_YUVJ422P
#define AV_PIX_FMT_YUVJ444P      PIX_FMT_YUVJ444P

#define AV_PIX_FMT_UYVY422       PIX_FMT_UYVY422
#define AV_PIX_FMT_UYYVYY411     PIX_FMT_UYYVYY411
#define AV_PIX_FMT_BGR8          PIX_FMT_BGR8
#define AV_PIX_FMT_BGR4          PIX_FMT_BGR4
#define AV_PIX_FMT_BGR4_BYTE     PIX_FMT_BGR4_BYTE
#define AV_PIX_FMT_RGB8          PIX_FMT_RGB8
#define AV_PIX_FMT_RGB4          PIX_FMT_RGB4
#define AV_PIX_FMT_RGB4_BYTE     PIX_FMT_RGB4_BYTE

#if !defined AV_PIX_FMT_RGB565
#define AV_PIX_FMT_RGB565        PIX_FMT_RGB565
#endif

#if !defined AV_PIX_FMT_RGB555
#define AV_PIX_FMT_RGB555        PIX_FMT_RGB555
#endif

#define AV_PIX_FMT_NV12          PIX_FMT_NV12
#define AV_PIX_FMT_NV21          PIX_FMT_NV21

#define AV_PIX_FMT_ARGB          PIX_FMT_ARGB
#define AV_PIX_FMT_RGBA          PIX_FMT_RGBA
#define AV_PIX_FMT_ABGR          PIX_FMT_ABGR
#define AV_PIX_FMT_BGRA          PIX_FMT_BGRA

#define AV_PIX_FMT_GRAY16BE      PIX_FMT_GRAY16BE
#define AV_PIX_FMT_GRAY16LE      PIX_FMT_GRAY16LE
#define AV_PIX_FMT_YUV440P       PIX_FMT_YUV440P
#define AV_PIX_FMT_YUVJ440P      PIX_FMT_YUVJ440P
#define AV_PIX_FMT_YUVA420P      PIX_FMT_YUVA420P

#endif


//: Find the vidl pixel format that matches a FFMPEG one
vidl_pixel_format
vidl_pixel_format_from_ffmpeg(AVPixelFormat ffmpeg_pix_fmt)
{
  switch (ffmpeg_pix_fmt)
  {
  case AV_PIX_FMT_YUV420P:   return VIDL_PIXEL_FORMAT_YUV_420P;
  case AV_PIX_FMT_YUYV422:   return VIDL_PIXEL_FORMAT_YUYV_422;
  case AV_PIX_FMT_RGB24:     return VIDL_PIXEL_FORMAT_RGB_24;
  case AV_PIX_FMT_BGR24:     return VIDL_PIXEL_FORMAT_BGR_24;
  case AV_PIX_FMT_YUV422P:   return VIDL_PIXEL_FORMAT_YUV_422P;
  case AV_PIX_FMT_YUV444P:   return VIDL_PIXEL_FORMAT_YUV_444P;
#ifdef AV_PIX_FMT_RGBA
  case AV_PIX_FMT_RGBA:      return VIDL_PIXEL_FORMAT_RGBA_32;
#endif
  case AV_PIX_FMT_YUV410P:   return VIDL_PIXEL_FORMAT_YUV_410P;
  case AV_PIX_FMT_YUV411P:   return VIDL_PIXEL_FORMAT_YUV_411P;
  case AV_PIX_FMT_RGB565:    return VIDL_PIXEL_FORMAT_RGB_565;
  case AV_PIX_FMT_RGB555:    return VIDL_PIXEL_FORMAT_RGB_555;
  case AV_PIX_FMT_GRAY8:     return VIDL_PIXEL_FORMAT_MONO_8;
  case AV_PIX_FMT_PAL8:      return VIDL_PIXEL_FORMAT_MONO_8;   //HACK: Treating 8-bit palette as greyscale image
  case AV_PIX_FMT_MONOWHITE: return VIDL_PIXEL_FORMAT_MONO_1;
  case AV_PIX_FMT_MONOBLACK: return VIDL_PIXEL_FORMAT_MONO_1;
  case AV_PIX_FMT_UYVY422:   return VIDL_PIXEL_FORMAT_UYVY_422;
  case AV_PIX_FMT_UYYVYY411: return VIDL_PIXEL_FORMAT_UYVY_411;
  default: break;
  }
  return VIDL_PIXEL_FORMAT_UNKNOWN;
}


//: Find the FFMPEG pixel format that matches a vidl one
AVPixelFormat
vidl_pixel_format_to_ffmpeg(vidl_pixel_format vidl_pix_fmt)
{
  switch (vidl_pix_fmt)
  {
  case VIDL_PIXEL_FORMAT_RGB_24:   return AV_PIX_FMT_RGB24;
  case VIDL_PIXEL_FORMAT_BGR_24:   return AV_PIX_FMT_BGR24;
#ifdef AV_PIX_FMT_RGBA
  case VIDL_PIXEL_FORMAT_RGBA_32:  return AV_PIX_FMT_RGBA;
#endif
  case VIDL_PIXEL_FORMAT_RGB_565:  return AV_PIX_FMT_RGB565;
  case VIDL_PIXEL_FORMAT_RGB_555:  return AV_PIX_FMT_RGB555;
  case VIDL_PIXEL_FORMAT_YUV_444P: return AV_PIX_FMT_YUV444P;
  case VIDL_PIXEL_FORMAT_YUYV_422: return AV_PIX_FMT_YUYV422;
  case VIDL_PIXEL_FORMAT_YUV_422P: return AV_PIX_FMT_YUV422P;
  case VIDL_PIXEL_FORMAT_YUV_420P: return AV_PIX_FMT_YUV420P;
  case VIDL_PIXEL_FORMAT_YUV_411P: return AV_PIX_FMT_YUV411P;
  case VIDL_PIXEL_FORMAT_YUV_410P: return AV_PIX_FMT_YUV410P;
  case VIDL_PIXEL_FORMAT_UYVY_422: return AV_PIX_FMT_UYVY422;
  case VIDL_PIXEL_FORMAT_UYVY_411: return AV_PIX_FMT_UYYVYY411;
  case VIDL_PIXEL_FORMAT_MONO_1:   return AV_PIX_FMT_MONOBLACK;
  case VIDL_PIXEL_FORMAT_MONO_8:   return AV_PIX_FMT_GRAY8;
  default: break;
  }
  std::cerr << "Warning: unknown FFMPeg pixel format type : " << vidl_pix_fmt << '\n';
  return AV_PIX_FMT_NONE;
}
