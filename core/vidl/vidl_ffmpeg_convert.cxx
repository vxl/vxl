// This is core/vidl/vidl_ffmpeg_convert.cxx
//:
// \file
// \author Matt Leotta
// \author Gehua Yang
// \date 19 Jan 2016
//
//-----------------------------------------------------------------------------

#include <cstring>
#include <iostream>
#include "vidl_ffmpeg_convert.h"
#include "vidl_ffmpeg_pixel_format.h"
#include "vidl_frame.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vidl/vidl_config.h>
extern "C" {
#if FFMPEG_IN_SEVERAL_DIRECTORIES
# include <libavcodec/version.h>
# if LIBAVCODEC_VERSION_MAJOR >= 56
#   include <libavutil/pixfmt.h>
# else
#   ifndef __STDC_CONSTANT_MACROS
#     define __STDC_CONSTANT_MACROS
#   endif
#   include <libavcodec/avcodec.h>
# endif
#else
# include <ffmpeg/avcodec.h>
#endif
}

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

#if LIBAVCODEC_VERSION_MAJOR < 56

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

//: Convert the pixel format of a frame using FFMPEG
//
// The \p in_frame.data() is converted from \p in_frame.pixel_format()
// to \p out_frame->pixel_format() and stored in \p out_frame->data()
// If the output frame data is not the correct size new memory
// will be allocated
bool vidl_ffmpeg_convert(vidl_frame const& in_frame,
                         vidl_frame      & out_frame)
{
  AVPixelFormat in_fmt =
    vidl_pixel_format_to_ffmpeg(in_frame.pixel_format());

  AVPixelFormat out_fmt =
    vidl_pixel_format_to_ffmpeg(out_frame.pixel_format());

  if (in_fmt == AV_PIX_FMT_NONE || out_fmt == AV_PIX_FMT_NONE)
    return false;

  unsigned ni = in_frame.ni();
  unsigned nj = in_frame.nj();
  auto out_size = (unsigned)avpicture_get_size(out_fmt, ni, nj);

  if (out_frame.size() != out_size ||
      out_frame.ni() != ni ||
      out_frame.nj() != nj ||
      !out_frame.data())
  {
    //assert(typeid(out_frame) == typeid(vidl_memory_chunk_frame));  // must be the same type
    //static_cast<vidl_memory_chunk_frame&>(out_frame)
    //  = vidl_memory_chunk_frame(ni, nj, out_frame.pixel_format(),
    //                            new vil_memory_chunk(out_size, VIL_PIXEL_FORMAT_BYTE));
    return false;
  }

  AVPicture in_pic;
  std::memset(&in_pic, 0, sizeof(in_pic));
  avpicture_fill(&in_pic, (uint8_t*)in_frame.data(), in_fmt, ni, nj);

  AVPicture out_pic;
  std::memset(&out_pic, 0, sizeof(out_pic));
  avpicture_fill(&out_pic, (uint8_t*)out_frame.data(), out_fmt, ni, nj);

#if LIBAVCODEC_BUILD < ((52<<16)+(10<<8)+0)  // before ver 52.10.0
  if (img_convert(&out_pic, out_fmt, &in_pic, in_fmt, ni, nj) < 0)
    return false;
#else
  SwsContext* ctx = sws_getContext(ni, nj, in_fmt,
                                   ni, nj, out_fmt,
                                   SWS_BILINEAR,
                                   nullptr, nullptr, nullptr);
  sws_scale(ctx,
            in_pic.data, in_pic.linesize,
            0, nj,
            out_pic.data, out_pic.linesize);
  sws_freeContext(ctx);
#endif

  return true;
}

bool vidl_ffmpeg_convert(const vidl_frame_sptr& in_frame,
                         vidl_frame_sptr& out_frame)
{
  if (!in_frame || !out_frame)
    return false;

  AVPixelFormat in_fmt =
    vidl_pixel_format_to_ffmpeg(in_frame->pixel_format());

  AVPixelFormat out_fmt =
    vidl_pixel_format_to_ffmpeg(out_frame->pixel_format());

  if (in_fmt == AV_PIX_FMT_NONE || out_fmt == AV_PIX_FMT_NONE)
    return false;

  unsigned ni = in_frame->ni();
  unsigned nj = in_frame->nj();
  auto out_size = (unsigned)avpicture_get_size(out_fmt, ni, nj);

  if (out_frame->size() != out_size ||
      out_frame->ni() != ni ||
      out_frame->nj() != nj ||
      !out_frame->data())
    out_frame = new vidl_memory_chunk_frame(ni, nj, out_frame->pixel_format(),
                                            new vil_memory_chunk(out_size, VIL_PIXEL_FORMAT_BYTE));

  AVPicture in_pic;
  std::memset(&in_pic, 0, sizeof(in_pic));
  avpicture_fill(&in_pic, (uint8_t*)in_frame->data(), in_fmt, ni, nj);

  AVPicture out_pic;
  std::memset(&out_pic, 0, sizeof(out_pic));
  avpicture_fill(&out_pic, (uint8_t*)out_frame->data(), out_fmt, ni, nj);

#if LIBAVCODEC_BUILD < ((52<<16)+(10<<8)+0)  // before ver 52.10.0
  if (img_convert(&out_pic, out_fmt, &in_pic, in_fmt, ni, nj) < 0)
    return false;
#else
  SwsContext* ctx = sws_getContext(ni, nj, in_fmt,
                                   ni, nj, out_fmt,
                                   SWS_BILINEAR,
                                   nullptr, nullptr, nullptr);
  sws_scale(ctx,
            in_pic.data, in_pic.linesize,
            0, nj,
            out_pic.data, out_pic.linesize);
  sws_freeContext(ctx);
#endif

  return true;
}
