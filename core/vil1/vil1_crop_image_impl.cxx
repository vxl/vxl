// This is ./vxl/vil/vil_crop_image_impl.cxx
#ifdef __GNUC__
#pragma implementation
#endif

//:
// \file

#include "vil_crop_image_impl.h"

vil_crop_image_impl::vil_crop_image_impl(vil_image const& gi, int x0, int y0, int w, int h):
  gi_(gi),
  x0_(x0),
  y0_(y0),
  width_(w),
  height_(h)
{
}

vil_crop_image_impl::~vil_crop_image_impl()
{
}

/* START_MANCHESTER_BINARY_IO_CODE */

//: Return the name of the class;
vcl_string vil_crop_image_impl::is_a() const
{
  static const vcl_string class_name_="vil_crop_image_impl";
  return class_name_;
}

//: Return true if the name of the class matches the argument
bool vil_crop_image_impl::is_class(vcl_string const& s) const
{
  return s==vil_crop_image_impl::is_a() || vil_image_impl::is_class(s);
}

/* END_MANCHESTER_BINARY_IO_CODE */
