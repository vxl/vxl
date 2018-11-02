#include <iostream>
#include <cmath>
#include "brip_line_generator.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

bool brip_line_generator::generate(bool& init, float xs, float ys,
                                   float xe, float ye,
                                   float& x, float& y)
{
  static float dx, dy, mag, xinc, yinc;
  static int x1, y1;
  if (init)
  {
    dx = xe-xs;
    dy = ye-ys;
    mag = (float)std::sqrt(dx*dx + dy*dy);
    if (mag<1)//Can't reach the next pixel under any circumstances
      return false;
    xinc = dx/mag;
    yinc = dy/mag;
    x1 = int(xe);
    y1 = int(ye);
    x = xs;
    y = ys;
    init = false;
//  done = false;
    return true;
  }

  //Previous pixel location
  int xp = int(x);
  int yp = int(y);
  //Increment along the line until the motion is greater than one pixel
  for (int i = 0; i<5; i++)
  {
    x += (float)0.5*xinc;
    y += (float)0.5*yinc;
    //Check to see if we have finished the span
    int xc = int(x), yc = int(y);
    bool passed_xe = ((xinc>=0)&&(xc>x1))||((xinc<0)&&(xc<x1));
    bool passed_ye = ((yinc>=0)&&(yc>y1))||((yinc<0)&&(yc<y1));
    if (passed_xe||passed_ye)
      return false;
    //Check if we have advanced by at least 1 pixel
    if (std::fabs(static_cast<float>(xc-xp))>=1||std::fabs(static_cast<float>(yc-yp))>=1)
      return true;
  }
  std::cout << "In brip_line_generator: - shouldn't happen - "
           << "xs="<<xs<<" ys="<<ys<<" xe="<<xe<<" ye="<< ye << std::endl;
  return false;
}
