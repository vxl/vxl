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
  float _x0;
  float _y0;
  float _x1;
  float _y1;
  float _theta;
  float _grad_mean;

public:
  // Constructors/Destructors--------------------------------------------------
  LineSeg() {}
  LineSeg(float x0, float y0, float x1, float y1, float theta, float grad_mean);
  ~LineSeg() {}
};

vcl_ostream& operator<<(vcl_ostream&, const LineSeg& l);
vcl_istream& operator>>(vcl_istream&, LineSeg& l);

#endif // LineSeg_h_
