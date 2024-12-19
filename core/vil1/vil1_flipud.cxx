// This is core/vil1/vil1_flipud.cxx
//:
// \file
// \author fsm

#include "vil1_flipud.h"
#include "vil1/vil1_flipud_impl.h"

vil1_image
vil1_flipud(const vil1_image & I)
{
  return new vil1_flipud_impl(I);
}
