// This is vxl/vil/vil_skip.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm

#include "vil_skip.h"
#include <vil/vil_skip_image_impl.h>

vil_image vil_skip(vil_image const &i, unsigned sx, unsigned sy) {
  return new vil_skip_image_impl(i, sx, sy);
}
