#include "camera.h"
#include <vcl_cassert.h>

camera::camera(int id) : id_(id)
{
}


void camera::set_lens_model(vcl_vector<bool> flags)
{
  assert(flags.size()==7);
  for (int i=0; i<7; i++) {
    if (flags[i])
      lm_.turn_on(i);
    else
      lm_.turn_off(i);
  }
}
