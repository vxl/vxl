#include "camera.h"
#include <vcl_cassert.h>

Camera::Camera()
{
}


void Camera::setLensModel(vcl_vector<bool> flags)
{
  assert(flags.size()==7);
  for (int i=0; i<7; i++) {
    if (flags[i])
      lm_.turnOn(i);
    else
      lm_.turnOff(i);
  }
}
