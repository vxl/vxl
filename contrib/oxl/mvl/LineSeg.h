#ifndef LineSeg_h_
#define LineSeg_h_
//:
// \file
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 01 Aug 96
//
//-----------------------------------------------------------------------------
#include <vcl_iosfwd.h>

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
  LineSeg() {}
  LineSeg(float x0, float y0, float x1, float y1, float theta, float grad_mean);
  ~LineSeg() {}
};

vcl_ostream& operator<<(vcl_ostream&, const LineSeg& l);
vcl_istream& operator>>(vcl_istream&, LineSeg& l);

#endif // LineSeg_h_
