// This is vxl/vil/vil_flip_components.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
#include "vil_flip_components.h"
#include <vil/vil_flip_components_impl.h>

vil_image vil_flip_components(vil_image const &I) {
  return new vil_flip_components_impl(I);
}
