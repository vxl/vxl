// This is core/vil1/vil1_flipud.cxx
//:
// \file
// \author fsm

#include "vil1_flipud.h"
#include <vil1/vil1_flipud_impl.h>

vil1_image vil1_flipud(vil1_image const &I) {
  return new vil1_flipud_impl(I);
}
