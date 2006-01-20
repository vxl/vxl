// This is brl/bbas/vidl2/vidl2_ffmpeg_convert.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Matt Leotta
// \date   19 Jan 2006
//
//-----------------------------------------------------------------------------

#include "vidl2_ffmpeg_convert.h"
#include "vidl2_frame.h"
#include <vcl_cstring.h>

//--------------------------------------------------------------------------------

//: Convert the pixel format of a frame using FFMPEG
//
// The \p in_frame->data() is converted from \p in_frame->pixel_format()
// to \p out_frame->pixel_format() and stored in \p out_frame->data()
// If the output frame data is not the correct size new memory
// will be allocated
bool vidl2_ffmpeg_convert(const vidl2_frame_sptr& in_frame,
                                vidl2_frame_sptr& out_frame)
{
  if(!in_frame || !out_frame)
    return false;

  PixelFormat in_fmt =
      vidl2_pixel_format_to_ffmpeg(in_frame->pixel_format());

  PixelFormat out_fmt =
      vidl2_pixel_format_to_ffmpeg(out_frame->pixel_format());

  if(in_fmt == PIX_FMT_NONE || out_fmt == PIX_FMT_NONE)
    return false;

  unsigned ni = in_frame->ni();
  unsigned nj = in_frame->nj();
  unsigned out_size = (unsigned) avpicture_get_size( out_fmt, ni, nj );

  if(out_frame->size() != out_size ||
     out_frame->ni() != ni ||
     out_frame->nj() != nj ||
     !out_frame->data() ){
    out_frame = new vidl2_memory_chunk_frame(ni, nj, out_frame->pixel_format(),
                                             new vil_memory_chunk(out_size, VIL_PIXEL_FORMAT_BYTE));
  }


  AVPicture in_pic;
  vcl_memset( &in_pic, 0, sizeof(in_pic) );
  avpicture_fill(&in_pic, (uint8_t*) in_frame->data(), in_fmt, ni, nj);

  AVPicture out_pic;
  vcl_memset( &out_pic, 0, sizeof(out_pic) );
  avpicture_fill(&out_pic, (uint8_t*) out_frame->data(), out_fmt, ni, nj);


  if( img_convert( &out_pic, out_fmt, &in_pic, in_fmt, ni, nj ) < 0 )
    return false;

  return true;
}


//: Find the vidl2 pixel format that matches a FFMPEG one
vidl2_pixel_format
vidl2_pixel_format_from_ffmpeg(PixelFormat ffmpeg_pix_fmt)
{
  switch(ffmpeg_pix_fmt){
    case PIX_FMT_YUV420P:   return VIDL2_PIXEL_FORMAT_YUV_420P;
    case PIX_FMT_YUV422:    return VIDL2_PIXEL_FORMAT_YUV_422;
    case PIX_FMT_RGB24:     return VIDL2_PIXEL_FORMAT_RGB_24;
    case PIX_FMT_BGR24:     return VIDL2_PIXEL_FORMAT_BGR_24;
    case PIX_FMT_YUV422P:   return VIDL2_PIXEL_FORMAT_YUV_422P;
    case PIX_FMT_YUV444P:   return VIDL2_PIXEL_FORMAT_YUV_444P;
    case PIX_FMT_RGBA32:    return VIDL2_PIXEL_FORMAT_RGBA_32;
    case PIX_FMT_YUV410P:   return VIDL2_PIXEL_FORMAT_YUV_410P;
    case PIX_FMT_YUV411P:   return VIDL2_PIXEL_FORMAT_YUV_411P;
    case PIX_FMT_RGB565:    return VIDL2_PIXEL_FORMAT_RGB_565;
    case PIX_FMT_RGB555:    return VIDL2_PIXEL_FORMAT_RGB_555;
    case PIX_FMT_GRAY8:     return VIDL2_PIXEL_FORMAT_MONO_8;
    case PIX_FMT_MONOWHITE: return VIDL2_PIXEL_FORMAT_MONO_1;
    case PIX_FMT_MONOBLACK: return VIDL2_PIXEL_FORMAT_MONO_1;
    case PIX_FMT_UYVY422:   return VIDL2_PIXEL_FORMAT_UYVY_422;
    case PIX_FMT_UYVY411:   return VIDL2_PIXEL_FORMAT_UYVY_411;
    default: break;
  }
  return VIDL2_PIXEL_FORMAT_UNKNOWN;
}


//: Find the FFMPEG pixel format that matches a vidl2 one
PixelFormat
vidl2_pixel_format_to_ffmpeg(vidl2_pixel_format vidl2_pix_fmt)
{
  switch(vidl2_pix_fmt){
    case VIDL2_PIXEL_FORMAT_RGB_24:   return PIX_FMT_RGB24;
    case VIDL2_PIXEL_FORMAT_BGR_24:   return PIX_FMT_BGR24;
    case VIDL2_PIXEL_FORMAT_RGBA_32:  return PIX_FMT_RGBA32;
    case VIDL2_PIXEL_FORMAT_RGB_565:  return PIX_FMT_RGB565;
    case VIDL2_PIXEL_FORMAT_RGB_555:  return PIX_FMT_RGB555;
    case VIDL2_PIXEL_FORMAT_YUV_444P: return PIX_FMT_YUV444P;
    case VIDL2_PIXEL_FORMAT_YUV_422:  return PIX_FMT_YUV422;
    case VIDL2_PIXEL_FORMAT_YUV_422P: return PIX_FMT_YUV422P;
    case VIDL2_PIXEL_FORMAT_YUV_420P: return PIX_FMT_YUV420P;
    case VIDL2_PIXEL_FORMAT_YUV_411P: return PIX_FMT_YUV411P;
    case VIDL2_PIXEL_FORMAT_YUV_410P: return PIX_FMT_YUV410P;
    case VIDL2_PIXEL_FORMAT_UYVY_422: return PIX_FMT_UYVY422;
    case VIDL2_PIXEL_FORMAT_UYVY_411: return PIX_FMT_UYVY411;
    case VIDL2_PIXEL_FORMAT_MONO_1:   return PIX_FMT_MONOBLACK;
    case VIDL2_PIXEL_FORMAT_MONO_8:   return PIX_FMT_GRAY8;
    default: break;
  }
  return PIX_FMT_NONE;
}
