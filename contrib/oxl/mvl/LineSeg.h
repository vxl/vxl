#ifndef LineSeg_h_
#define LineSeg_h_
//:
// \file
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 01 Aug 96
//
//-----------------------------------------------------------------------------
#include <iostream>
#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class LineSeg
{
 public:
  // Data Members--------------------------------------------------------------
  float x0_;
  float y0_;
  float x1_;
  float y1_;
  float theta_;
  float grad_mean_;

 public:
  // Constructors/Destructors--------------------------------------------------
  LineSeg() = default;
  LineSeg(float x0, float y0, float x1, float y1, float theta, float grad_mean);
  ~LineSeg() = default;
};

std::ostream& operator<<(std::ostream&, const LineSeg& l);
std::istream& operator>>(std::istream&, LineSeg& l);

#endif // LineSeg_h_
