// This is brl/bbas/vidl2/vidl2_convert.h
#ifndef vidl2_convert_h_
#define vidl2_convert_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Convert frames to vil_image_views
//
// \author Matt Leotta
// \date 16 Jan 2006
//
// This file contains functions used to convert a vidl2_frame_sptr
// into a vil_image_view.


#include "vidl2_frame_sptr.h"
#include "vidl2_frame.h"
#include <vil/vil_image_view.h>



//: Convert the frame into an image view
// possibly converts the pixel data type
// always create a deep copy of the data
// \param force_rgb determines whether YUV formats are converted to RGB
bool vidl2_convert_to_view(const vidl2_frame& frame,
                           vil_image_view_base& image,
                           bool force_rgb = true);


//: Wrap the frame buffer in an image view if supported
// Returns a null pointer if not possible
vil_image_view_base_sptr vidl2_convert_wrap_in_view(const vidl2_frame& frame);


//: Convert the pixel format of a frame
// The \p in_frame.data() is converted from \p in_frame.pixel_format()
// to \p out_frame.pixel_format() and stored in \p out_frame.data()
// \returns false if the output frame data is not the correct size.
bool vidl2_convert_frame(const vidl2_frame& in_frame,
                               vidl2_frame& out_frame);


//: Convert the pixel format of a frame
// Convert \p in_frame to a \p format by allocating a new frame buffer
vidl2_frame_sptr vidl2_convert_frame(const vidl2_frame_sptr& in_frame,
                                     vidl2_pixel_format format);

//: Convert the image view to a frame
// Will wrap the memory if possible, if not the image is converted to
// the closest vidl2_pixel_format
vidl2_frame_sptr vidl2_convert_to_frame(const vil_image_view_base_sptr& image);

#endif // vidl2_convert_h_
