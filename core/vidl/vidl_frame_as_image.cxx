//:
// \file

#include "vidl_frame_as_image.h"
#include <vidl/vidl_frame.h>
#include <vidl/vidl_codec.h>

//=========================================================================
//  Methods for vidl_frame_as_image.
//_________________________________________________________________________

//------------------------------------------------------------------------
// CONSTRUCTOR(S) AND DESTRUCTOR

//: Constructor.
// Take the characteristics of the first image of the movie
vidl_frame_as_image::vidl_frame_as_image(vidl_frame* frame) :
  frame_(frame)
{
}

// Destructor
vidl_frame_as_image::~vidl_frame_as_image()
{
}


//: Return the specified ROI from the Frame
bool vidl_frame_as_image::get_section(void* ib, int x0, int y0, int width, int heigth) const
{
  return frame_->get_section(ib, x0, y0, width, heigth);
}

int vidl_frame_as_image::components() const
{
  return frame_->get_bits_pixel() / 8;
}
