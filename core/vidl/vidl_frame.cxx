// This is vxl/vidl/vidl_frame.cxx
#include "vidl_frame.h"
//:
// \file

#include <vidl/vidl_codec.h>
#include <vidl/vidl_frame_as_image.h>
#include <vil/vil_image.h>

//=========================================================================
//  Methods for vidl_frame.
//_________________________________________________________________________

//------------------------------------------------------------------------
// CONSTRUCTOR(S) AND DESTRUCTOR

//: Constructor, needs the frame position in the clip and the coder used.
vidl_frame::vidl_frame(int position, vidl_codec_sptr coder) :
position_(position), coder_(coder)
{
}

//: Destructor
vidl_frame::~vidl_frame()
{
}

//: Return the image.
vil_image vidl_frame::get_image()
{
  if (! image_) {
    image_ = vil_image(new vidl_frame_as_image(this));
  }
  return image_;
}

//: Get the pixels for the rectangular window starting at x0, y0 and width and height wide.
 bool vidl_frame::get_section(void* ib, int x0, int y0, int width, int height) const
{
  return coder_->get_section(position_, ib, x0, y0, width, height);
}
