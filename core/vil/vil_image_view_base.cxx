// This is mul/vil2/vil2_image_view_base.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
//  \file
// \brief Implementation of vil2_image_base (needed to create virtual tables in gcc)
// \author Tim Cootes - Manchester

#include <vil2/vil2_image_view_base.h>

//: Return the name of the class
vcl_string vil2_image_view_base::is_a() const
{
  static const vcl_string class_name_="vil2_image_view_base";
  return class_name_;
}

//: True if this is (or is derived from) class s
bool vil2_image_view_base::is_class(vcl_string const& s) const
{
  return s==vil2_image_view_base::is_a();
}

