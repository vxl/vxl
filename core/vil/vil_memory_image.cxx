// This is mul/vil2/vil2_memory_image.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Ian Scott


#include "vil2_memory_image.h"
#include <vil/vil_byte.h>
#include <vil/vil_rgb.h>
#include <vil2/vil2_image_view.h>
#include <vil2/vil2_pixel_format.h>

class vil2_image_view_base;

vil2_memory_image::vil2_memory_image():
   view_(new vil2_image_view<vil_byte>()) {}

vil2_memory_image::vil2_memory_image(unsigned nx,
                 unsigned ny,
                 unsigned nplanes,
                 vil2_pixel_format format):
  view_(0)
{
  // Haven't got anything else written yet.
  assert (format == VIL2_PIXEL_FORMAT_BYTE);

  switch (format)
  {
  case VIL2_PIXEL_FORMAT_BYTE:
    view_ = new vil2_image_view<vil_byte>(nx, ny, nplanes);
    break;
  case VIL2_PIXEL_FORMAT_RGB_BYTE:
    view_ = new vil2_image_view<vil_rgb<vil_byte> >(nx, ny, nplanes);
    break;
  case VIL2_PIXEL_FORMAT_UNSIGNED_INT:
    view_ = new vil2_image_view<unsigned int>(nx, ny, nplanes);
    break;
  case VIL2_PIXEL_FORMAT_BOOL:
    view_ = new vil2_image_view<bool>(nx, ny, nplanes);
    break;
  default:
    view_ = new vil2_image_view<vil_byte>();
  }
}

vil2_memory_image::~vil2_memory_image()
{
  delete view_;
}


//: Create a read/write view of a copy of this data.
// \return 0 if unable to get view of correct size.
vil2_image_view_base* vil2_memory_image::get_copy_view(unsigned x0, unsigned y0, unsigned plane0,
                                            unsigned nx, unsigned ny, unsigned nplanes) const
{
  assert(!"vil2_memory_image::get_copy_view Not yet implemented");
  return 0;
}

//: Create a read/write view of a copy of this data.
// \return 0 if unable to get view of correct size.
vil2_image_view_base* vil2_memory_image::get_view(unsigned x0, unsigned y0, unsigned plane0,
                                            unsigned nx, unsigned ny, unsigned nplanes) const
{
  if (view_->pixel_format() == VIL2_PIXEL_FORMAT_BYTE)
  {
    const vil2_image_view<vil_byte> &v = static_cast<const vil2_image_view<vil_byte> &>(
      *view_);
    return new vil2_image_view<vil_byte>(v.memory_chunk(), &v(x0,y0,plane0),
      nx, ny, nplanes, v.xstep(), v.ystep(), v.planestep());
  }
  return 0;
}


//: Put the data in this view back into the image source.
bool vil2_memory_image::put_view(const vil2_image_view_base& im, unsigned x0, unsigned y0, unsigned plane0)
{
  assert(!"vil2_memory_image::put_view Not yet implemented");
  return false;
}

