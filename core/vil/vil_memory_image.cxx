// This is mul/vil2/vil2_memory_image.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Ian Scott

#include "vil2_memory_image.h"
#include <vil2/vil2_byte.h>
#include <vil/vil_rgb.h>
#include <vil2/vil2_image_view.h>
#include <vil2/vil2_pixel_format.h>
#include <vcl_cassert.h>

class vil2_image_view_base;

vil2_memory_image::vil2_memory_image():
   view_(new vil2_image_view<vil2_byte>()) {}

vil2_memory_image::vil2_memory_image(unsigned ni,
  unsigned nj, unsigned nplanes, vil2_pixel_format format)
{
  // Haven't got anything else written yet.
  assert (format == VIL2_PIXEL_FORMAT_BYTE);

  switch (format)
  {
  case VIL2_PIXEL_FORMAT_BYTE:
    view_ = new vil2_image_view<vil2_byte>(ni, nj, nplanes);
    break;
  case VIL2_PIXEL_FORMAT_UINT_32:
    view_ = new vil2_image_view<vxl_uint_32>(ni, nj, nplanes);
    break;
  case VIL2_PIXEL_FORMAT_UINT_16:
    view_ = new vil2_image_view<vxl_uint_16>(ni, nj, nplanes);
    break;
  case VIL2_PIXEL_FORMAT_BOOL:
    view_ = new vil2_image_view<bool>(ni, nj, nplanes);
    break;
  default:
    view_ = new vil2_image_view<vil2_byte>();
  }
}


//: Create a read/write view of a copy of this data.
// Currently not yet implemented.
// \return 0 if unable to get view of correct size.
vil2_image_view_base_sptr vil2_memory_image::get_copy_view(unsigned i0, unsigned j0,
                                                           unsigned ni, unsigned nj) const
{
  assert(!"vil2_memory_image::get_copy_view Not yet implemented");
  return 0;
}

//: Create a read/write view of a copy of this data.
// \return 0 if unable to get view of correct size.
vil2_image_view_base_sptr vil2_memory_image::get_view(unsigned i0, unsigned j0,
                                                      unsigned ni, unsigned nj) const
{
  if (view_->pixel_format() == VIL2_PIXEL_FORMAT_BYTE)
  {
    const vil2_image_view<vil2_byte> &v = static_cast<const vil2_image_view<vil2_byte> &>(
      *view_);
    return new vil2_image_view<vil2_byte>(v.memory_chunk(), &v(i0,j0),
                                          ni, nj, v.nplanes(),
                                          v.istep(), v.jstep(), v.planestep());
  }
  return 0;
}


//: Put the data in this view back into the image source.
// Currently not yet implemented.
// \return true on success.
bool vil2_memory_image::put_view(const vil2_image_view_base& im,unsigned i0, unsigned y0)
{
  assert(!"vil2_memory_image::put_view Not yet implemented");
  return false;
}

