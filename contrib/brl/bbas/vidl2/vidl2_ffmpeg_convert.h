// This is brl/bbas/vidl2/vidl2_ffmpeg_convert.h
#ifndef vidl2_ffmpeg_convert_h_
#define vidl2_ffmpeg_convert_h_
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

#include "vidl2_frame_sptr.h"
#include "vidl2_pixel_format.h"
#include <ffmpeg/avcodec.h>

//: Convert the pixel format of a frame using FFMPEG
//
// The \p in_frame->data() is converted from \p in_frame->pixel_format()
// to \p out_frame->pixel_format() and stored in \p out_frame->data()
// If the output frame data is not the correct size new memory
// will be allocated
bool vidl2_ffmpeg_convert(const vidl2_frame_sptr& in_frame,
                                vidl2_frame_sptr& out_frame);


//: Find the vidl2 pixel format that matches a FFMPEG one
vidl2_pixel_format
vidl2_pixel_format_from_ffmpeg(PixelFormat ffmpeg_pix_fmt);


//: Find the FFMPEG pixel format that matches a vidl2 one
PixelFormat
vidl2_pixel_format_to_ffmpeg(vidl2_pixel_format vidl2_pix_fmt);

#endif // vidl2_ffmpeg_convert_h_

