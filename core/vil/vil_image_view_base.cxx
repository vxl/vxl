// This is core/vil/vil_image_view_base.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Tim Cootes - Manchester

#include "vil_image_view_base.h"

//: True if this is (or is derived from) class s
bool vil_image_view_base::is_class(vcl_string const& s) const
{
  return s=="vil_image_view_base";
}

