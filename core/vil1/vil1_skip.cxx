// This is core/vil1/vil1_skip.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm

#include "vil1_skip.h"
#include <vil1/vil1_skip_image_impl.h>

vil1_image vil1_skip(vil1_image const &i, unsigned sx, unsigned sy) {
  return new vil1_skip_image_impl(i, sx, sy);
}
