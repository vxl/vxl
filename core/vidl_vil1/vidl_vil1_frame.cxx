// This is core/vidl_vil1/vidl_vil1_frame.cxx
#include "vidl_vil1_frame.h"
//:
// \file

#include <vidl_vil1/vidl_vil1_codec.h>
#include <vidl_vil1/vidl_vil1_frame_as_image.h>
#include <vil1/vil1_image.h>

//=========================================================================
//  Methods for vidl_vil1_frame.
//_________________________________________________________________________

//------------------------------------------------------------------------
// CONSTRUCTOR(S) AND DESTRUCTOR

//: Constructor, needs the frame position in the clip and the coder used.
vidl_vil1_frame::vidl_vil1_frame(int position, vidl_vil1_codec_sptr coder) :
position_(position), coder_(coder)
{
}

//: Destructor
vidl_vil1_frame::~vidl_vil1_frame()
{
}

//: Return the image.
vil1_image vidl_vil1_frame::get_image()
{
  if (! image_) {
    image_ = vil1_image(new vidl_vil1_frame_as_image(this));
  }
  return image_;
}

//: Get the pixels for the rectangular window starting at x0, y0 and width and height wide.
 bool vidl_vil1_frame::get_section(void* ib, int x0, int y0, int width, int height) const
{
  return coder_->get_section(position_, ib, x0, y0, width, height);
}
