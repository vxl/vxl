// This is oxl/mvl/LineSeg.cxx
#include <iostream>
#include "LineSeg.h"
//:
//  \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Constructor
LineSeg::LineSeg(float x0, float y0, float x1, float y1, float theta, float grad_mean)
{
  x0_ = x0;
  y0_ = y0;
  x1_ = x1;
  y1_ = y1;
  theta_ = theta;
  grad_mean_ = grad_mean;
}

//: Save to std::ostream
std::ostream& operator<<(std::ostream& s, const LineSeg& l)
{
  return s << l.x0_ << ' '
           << l.y0_ << ' '
           << l.x1_ << ' '
           << l.y1_ << ' '
           << l.theta_ << ' '
           << l.grad_mean_ << std::endl;
}

//: Read from std::istream
std::istream& operator>>(std::istream& s, LineSeg& l)
{
  return s >> l.x0_ >> l.y0_ >> l.x1_ >> l.y1_ >> l.theta_ >> l.grad_mean_;
}
