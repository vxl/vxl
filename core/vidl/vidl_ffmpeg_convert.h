// This is core/vidl/vidl_ffmpeg_convert.h
#ifndef vidl_ffmpeg_convert_h_
#define vidl_ffmpeg_convert_h_
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

//: Convert the pixel format of a frame using FFMPEG
// The \p in_frame.data() is converted from \p in_frame.pixel_format()
// to \p out_frame.pixel_format() and stored in \p out_frame.data()
// If the output frame data is not the correct size new memory
// will be allocated
bool vidl_ffmpeg_convert(const vidl_frame & in_frame,
                               vidl_frame & out_frame);

//: Convert the pixel format of a frame using FFMPEG
//
// The \p in_frame->data() is converted from \p in_frame->pixel_format()
// to \p out_frame->pixel_format() and stored in \p out_frame->data()
// If the output frame data is not the correct size new memory
// will be allocated
bool vidl_ffmpeg_convert(const vidl_frame_sptr& in_frame,
                               vidl_frame_sptr& out_frame);


#endif // vidl_ffmpeg_convert_h_
