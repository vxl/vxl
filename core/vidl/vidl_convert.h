// This is core/vidl/vidl_convert.h
#ifndef vidl_convert_h_
#define vidl_convert_h_
//:
// \file
// \brief Convert frames to vil_image_views
//
// \author Matt Leotta
// \date 16 Jan 2006
//
// This file contains functions used to convert a vidl_frame_sptr
// into a vil_image_view.


#include "vidl_frame_sptr.h"
#include "vidl_frame.h"
#include <vidl/vidl_export.h>
#include <vil/vil_image_view_base.h>

//: Convert the frame into an image view
// possibly converts the pixel data type
// always create a deep copy of the data
// \param require_color restricts the color mode of the output
//         if set to UNKNOWN (default) the input color mode is used
VIDL_EXPORT
bool vidl_convert_to_view(const vidl_frame& frame,
                          vil_image_view_base& image,
                          vidl_pixel_color require_color = VIDL_PIXEL_COLOR_UNKNOWN);


//: Wrap the frame buffer in an image view if supported
// Returns a null pointer if not possible
VIDL_EXPORT
vil_image_view_base_sptr vidl_convert_wrap_in_view(const vidl_frame& frame);


//: Convert the pixel format of a frame
// The \p in_frame.data() is converted from \p in_frame.pixel_format()
// to \p out_frame.pixel_format() and stored in \p out_frame.data()
// \returns false if the output frame data is not the correct size.
VIDL_EXPORT
bool vidl_convert_frame(const vidl_frame& in_frame,
                              vidl_frame& out_frame);


//: Convert the pixel format of a frame
// Convert \p in_frame to a \p format by allocating a new frame buffer
VIDL_EXPORT
vidl_frame_sptr vidl_convert_frame(const vidl_frame_sptr& in_frame,
                                   vidl_pixel_format format);

//: Convert the image view smart pointer to a frame
// Will wrap the memory if possible, if not the image is converted to
// the closest vidl_pixel_format
VIDL_EXPORT
vidl_frame_sptr vidl_convert_to_frame(const vil_image_view_base_sptr& image);

//: Convert the image view to a frame
// Will wrap the memory if possible, if not the image is converted to
// the closest vidl_pixel_format
VIDL_EXPORT
vidl_frame_sptr vidl_convert_to_frame(const vil_image_view_base& image);

#endif // vidl_convert_h_
