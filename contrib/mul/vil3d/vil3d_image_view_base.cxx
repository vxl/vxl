// This is mul/vil3d/vil3d_image_view_base.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief Implementation of vil2_image_base (necessary in order to create virtual tables in gcc)
// \author Tim Cootes - Manchester

#include <vil3d/vil3d_image_view_base.h>

//: True if this is (or is derived from) class s
bool vil3d_image_view_base::is_class(vcl_string const& s) const
{
  return s=="vil3d_image_view_base";
}

