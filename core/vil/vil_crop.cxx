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


vil2_image_data_sptr vil2_crop(const vil2_image_data_sptr &src, unsigned i0,
                               unsigned ni, unsigned j0, unsigned nj)
{
  return new vil2_crop_image_data(src, i0, ni, j0, nj);
}


vil2_crop_image_data::vil2_crop_image_data(vil2_image_data_sptr const& gi,
                                           unsigned i0, unsigned ni,
                                           unsigned j0, unsigned nj):
  gi_(gi),
  i0_(i0),
  ni_(ni),
  j0_(j0),
  nj_(nj)
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
