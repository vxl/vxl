// This is mul/vil2/vil2_crop.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Ian Scott.
//
//-----------------------------------------------------------------------------

#include "vil2_crop.h"


vil2_image_data_sptr vil2_crop(const vil2_image_data_sptr &src, unsigned x0,
                               unsigned y0, unsigned nx, unsigned ny)
{
  return new vil2_crop_image_data(src, x0, y0, nx, ny);
}


vil2_crop_image_data::vil2_crop_image_data(vil2_image_data_sptr const& gi, int x0, int y0, int nx, int ny):
  gi_(gi),
  x0_(x0),
  y0_(y0),
  nx_(nx),
  ny_(ny)
{
}

vil2_crop_image_data::~vil2_crop_image_data()
{
}


//: Return the name of the class;
vcl_string vil2_crop_image_data::is_a() const
{
  static const vcl_string class_name_="vil2_crop_image_data";
  return class_name_;
}

//: Return true if the name of the class matches the argument
bool vil2_crop_image_data::is_class(vcl_string const& s) const
{
  return s==vil2_crop_image_data::is_a() || vil2_image_data::is_class(s);
}
