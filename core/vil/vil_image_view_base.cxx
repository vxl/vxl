// This is core/vil2/vil2_image_view_base.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Tim Cootes - Manchester

#include "vil2_image_view_base.h"

//: True if this is (or is derived from) class s
bool vil2_image_view_base::is_class(vcl_string const& s) const
{
  return s=="vil2_image_view_base";
}

