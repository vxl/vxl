// This is core/vidl/vidl_ffmpeg_convert.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Matt Leotta
// \date   19 Jan 2006
//
//-----------------------------------------------------------------------------

#include "vidl_ffmpeg_convert.h"
#include "vidl_frame.h"
#include <vcl_cstring.h>


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

//: Convert the pixel format of a frame using FFMPEG
//
// The \p in_frame->data() is converted from \p in_frame->pixel_format()
// to \p out_frame->pixel_format() and stored in \p out_frame->data()
// If the output frame data is not the correct size new memory
// will be allocated
bool vidl_ffmpeg_convert(const vidl_frame_sptr& in_frame,
                               vidl_frame_sptr& out_frame)
{
  if (!in_frame || !out_frame)
    return false;

  PixelFormat in_fmt =
      vidl_pixel_format_to_ffmpeg(in_frame->pixel_format());

  PixelFormat out_fmt =
      vidl_pixel_format_to_ffmpeg(out_frame->pixel_format());

  if (in_fmt == PIX_FMT_NONE || out_fmt == PIX_FMT_NONE)
    return false;

  unsigned ni = in_frame->ni();
  unsigned nj = in_frame->nj();
  unsigned out_size = (unsigned) avpicture_get_size( out_fmt, ni, nj );

  if (out_frame->size() != out_size ||
      out_frame->ni() != ni ||
      out_frame->nj() != nj ||
      !out_frame->data() )
    out_frame = new vidl_memory_chunk_frame(ni, nj, out_frame->pixel_format(),
                                            new vil_memory_chunk(out_size, VIL_PIXEL_FORMAT_BYTE));

  AVPicture in_pic;
  vcl_memset( &in_pic, 0, sizeof(in_pic) );
  avpicture_fill(&in_pic, (uint8_t*) in_frame->data(), in_fmt, ni, nj);

  AVPicture out_pic;
  vcl_memset( &out_pic, 0, sizeof(out_pic) );
  avpicture_fill(&out_pic, (uint8_t*) out_frame->data(), out_fmt, ni, nj);

#if LIBAVCODEC_BUILD < ((52<<16)+(10<<8)+0)  // before ver 52.10.0
  if ( img_convert( &out_pic, out_fmt, &in_pic, in_fmt, ni, nj ) < 0 )
    return false;
#else
  SwsContext* ctx = sws_getContext( ni, nj, in_fmt,
                                    ni, nj, out_fmt,
                                    SWS_BILINEAR,
                                    NULL, NULL, NULL );
  sws_scale( ctx,
             in_pic.data, in_pic.linesize,
             0, nj,
             out_pic.data, out_pic.linesize );
  sws_freeContext( ctx );
#endif

  return true;
}


//: Find the vidl pixel format that matches a FFMPEG one
vidl_pixel_format
vidl_pixel_format_from_ffmpeg(PixelFormat ffmpeg_pix_fmt)
{
  switch (ffmpeg_pix_fmt)
  {
    case PIX_FMT_YUV420P:   return VIDL_PIXEL_FORMAT_YUV_420P;
    case PIX_FMT_YUYV422:   return VIDL_PIXEL_FORMAT_YUYV_422;
    case PIX_FMT_RGB24:     return VIDL_PIXEL_FORMAT_RGB_24;
    case PIX_FMT_BGR24:     return VIDL_PIXEL_FORMAT_BGR_24;
    case PIX_FMT_YUV422P:   return VIDL_PIXEL_FORMAT_YUV_422P;
    case PIX_FMT_YUV444P:   return VIDL_PIXEL_FORMAT_YUV_444P;
#ifdef PIX_FMT_RGBA
    case PIX_FMT_RGBA:      return VIDL_PIXEL_FORMAT_RGBA_32;
#endif
    case PIX_FMT_YUV410P:   return VIDL_PIXEL_FORMAT_YUV_410P;
    case PIX_FMT_YUV411P:   return VIDL_PIXEL_FORMAT_YUV_411P;
    case PIX_FMT_RGB565:    return VIDL_PIXEL_FORMAT_RGB_565;
    case PIX_FMT_RGB555:    return VIDL_PIXEL_FORMAT_RGB_555;
    case PIX_FMT_GRAY8:     return VIDL_PIXEL_FORMAT_MONO_8;
    case PIX_FMT_PAL8:      return VIDL_PIXEL_FORMAT_MONO_8;   //HACK: Treating 8-bit palette as greyscale image
    case PIX_FMT_MONOWHITE: return VIDL_PIXEL_FORMAT_MONO_1;
    case PIX_FMT_MONOBLACK: return VIDL_PIXEL_FORMAT_MONO_1;
    case PIX_FMT_UYVY422:   return VIDL_PIXEL_FORMAT_UYVY_422;
    case PIX_FMT_UYYVYY411: return VIDL_PIXEL_FORMAT_UYVY_411;
    default: break;
  }
  return VIDL_PIXEL_FORMAT_UNKNOWN;
}


//: Find the FFMPEG pixel format that matches a vidl one
PixelFormat
vidl_pixel_format_to_ffmpeg(vidl_pixel_format vidl_pix_fmt)
{
  switch (vidl_pix_fmt)
  {
    case VIDL_PIXEL_FORMAT_RGB_24:   return PIX_FMT_RGB24;
    case VIDL_PIXEL_FORMAT_BGR_24:   return PIX_FMT_BGR24;
#ifdef PIX_FMT_RGBA
    case VIDL_PIXEL_FORMAT_RGBA_32:  return PIX_FMT_RGBA;
#endif
    case VIDL_PIXEL_FORMAT_RGB_565:  return PIX_FMT_RGB565;
    case VIDL_PIXEL_FORMAT_RGB_555:  return PIX_FMT_RGB555;
    case VIDL_PIXEL_FORMAT_YUV_444P: return PIX_FMT_YUV444P;
    case VIDL_PIXEL_FORMAT_YUYV_422: return PIX_FMT_YUYV422;
    case VIDL_PIXEL_FORMAT_YUV_422P: return PIX_FMT_YUV422P;
    case VIDL_PIXEL_FORMAT_YUV_420P: return PIX_FMT_YUV420P;
    case VIDL_PIXEL_FORMAT_YUV_411P: return PIX_FMT_YUV411P;
    case VIDL_PIXEL_FORMAT_YUV_410P: return PIX_FMT_YUV410P;
    case VIDL_PIXEL_FORMAT_UYVY_422: return PIX_FMT_UYVY422;
    case VIDL_PIXEL_FORMAT_UYVY_411: return PIX_FMT_UYYVYY411;
    case VIDL_PIXEL_FORMAT_MONO_1:   return PIX_FMT_MONOBLACK;
    case VIDL_PIXEL_FORMAT_MONO_8:   return PIX_FMT_GRAY8;
    default: break;
  }
  return PIX_FMT_NONE;
}
