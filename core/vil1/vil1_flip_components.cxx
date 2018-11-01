// This is core/vil1/vil1_flip_components.cxx
#include "vil1_flip_components.h"
#include <vil1/vil1_flip_components_impl.h>

vil1_image vil1_flip_components(vil1_image const &I) {
  return new vil1_flip_components_impl(I);
}
