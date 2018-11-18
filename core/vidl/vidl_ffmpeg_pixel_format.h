// This is core/vidl/vidl_ffmpeg_pixel_format.h
#ifndef vidl_ffmpeg_pixel_format_h_
#define vidl_ffmpeg_pixel_format_h_

//:
// \file
// \brief Use FFMPEG for optimized pixel format conversions
//
// \author Matt Leotta
// \author Gehua Yang
// \date 19 Jan 2016
//

#include "vidl_frame_sptr.h"
#include "vidl_pixel_format.h"

#include <vidl/vidl_config.h>
extern "C" {
#if FFMPEG_IN_SEVERAL_DIRECTORIES
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

#if LIBAVCODEC_VERSION_MAJOR < 56
//  The pixel format was named PixelFormat in old versions of FFMPeg.
//  Use a typedef to maintain backward compatibility
typedef PixelFormat AVPixelFormat;
#endif



//: Find the vidl pixel format that matches a FFMPEG one
vidl_pixel_format
vidl_pixel_format_from_ffmpeg(AVPixelFormat ffmpeg_pix_fmt);


//: Find the FFMPEG pixel format that matches a vidl one
AVPixelFormat
vidl_pixel_format_to_ffmpeg(vidl_pixel_format vidl_pix_fmt);

#endif // vidl_ffmpeg_pixel_format_h_
