//:
// \file

#include "vidl_vil1_frame_as_image.h"
#include <vidl_vil1/vidl_vil1_frame.h>
#include <vidl_vil1/vidl_vil1_codec.h>

//=========================================================================
//  Methods for vidl_vil1_frame_as_image.
//_________________________________________________________________________

//------------------------------------------------------------------------
// CONSTRUCTOR(S) AND DESTRUCTOR

//: Constructor.
// Take the characteristics of the first image of the movie
vidl_vil1_frame_as_image::vidl_vil1_frame_as_image(vidl_vil1_frame* frame) :
  frame_(frame)
{
}

// Destructor
vidl_vil1_frame_as_image::~vidl_vil1_frame_as_image()
{
}


//: Return the specified ROI from the Frame
bool vidl_vil1_frame_as_image::get_section(void* ib, int x0, int y0, int width, int heigth) const
{
  return frame_->get_section(ib, x0, y0, width, heigth);
}

int vidl_vil1_frame_as_image::components() const
{
  return frame_->get_bits_pixel() / 8;
}
