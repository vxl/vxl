// This is core/vil1/vil1_crop_image_impl.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "vil1_crop_image_impl.h"

vil1_crop_image_impl::vil1_crop_image_impl(vil1_image const& gi, int x0, int y0, int w, int h):
  gi_(gi),
  x0_(x0),
  y0_(y0),
  width_(w),
  height_(h)
{
}

vil1_crop_image_impl::~vil1_crop_image_impl()
{
}

//: Return the name of the class;
vcl_string vil1_crop_image_impl::is_a() const
{
  static const vcl_string class_name_="vil1_crop_image_impl";
  return class_name_;
}

//: Return true if the name of the class matches the argument
bool vil1_crop_image_impl::is_class(vcl_string const& s) const
{
  return s==vil1_crop_image_impl::is_a() || vil1_image_impl::is_class(s);
}
