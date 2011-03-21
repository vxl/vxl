// This is core/vidl/vidl_ffmpeg_convert.h
#ifndef vidl_ffmpeg_convert_h_
#define vidl_ffmpeg_convert_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Use FFMPEG for optimized pixel format conversions
//
// \author Matt Leotta
// \date 19 Jan 2006
//

#include "vidl_frame_sptr.h"
#include "vidl_pixel_format.h"

#include <vidl/vidl_config.h>
extern "C" {
#if FFMPEG_IN_SEVERAL_DIRECTORIES
#ifndef __STDC_CONSTANT_MACROS
#  define __STDC_CONSTANT_MACROS
#endif
#include <libavcodec/avcodec.h>
#else
#include <ffmpeg/avcodec.h>
#endif
}

//: Convert the pixel format of a frame using FFMPEG
//
// The \p in_frame->data() is converted from \p in_frame->pixel_format()
// to \p out_frame->pixel_format() and stored in \p out_frame->data()
// If the output frame data is not the correct size new memory
// will be allocated
bool vidl_ffmpeg_convert(const vidl_frame_sptr& in_frame,
                               vidl_frame_sptr& out_frame);


//: Find the vidl pixel format that matches a FFMPEG one
vidl_pixel_format
vidl_pixel_format_from_ffmpeg(PixelFormat ffmpeg_pix_fmt);


//: Find the FFMPEG pixel format that matches a vidl one
PixelFormat
vidl_pixel_format_to_ffmpeg(vidl_pixel_format vidl_pix_fmt);

#endif // vidl_ffmpeg_convert_h_

