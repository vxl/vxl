// This is oxl/mvl/LineSeg.cxx

//:
//  \file

#include <vcl_iostream.h>
#include "LineSeg.h"

//: Constructor
LineSeg::LineSeg(float x0, float y0, float x1, float y1, float theta, float grad_mean)
{
  _x0 = x0;
  _y0 = y0;
  _x1 = x1;
  _y1 = y1;
  _theta = theta;
  _grad_mean = grad_mean;
}

//: Save to vcl_ostream
vcl_ostream& operator<<(vcl_ostream& s, const LineSeg& l)
{
  return s << l._x0 << " "
           << l._y0 << " "
           << l._x1 << " "
           << l._y1 << " "
           << l._theta << " "
           << l._grad_mean << vcl_endl;
}

//: Read from vcl_istream
vcl_istream& operator>>(vcl_istream& s, LineSeg& l)
{
  return s >> l._x0 >> l._y0 >> l._x1 >> l._y1 >> l._theta >> l._grad_mean;
}
