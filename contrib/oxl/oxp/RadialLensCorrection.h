// This is oxl/oxp/RadialLensCorrection.h
#ifndef RadialLensCorrection_h_
#define RadialLensCorrection_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Andrew W. Fitzgibbon, Oxford RRG, 13 Jul 98
//
//-----------------------------------------------------------------------------

#include <oxp/Mapping_2d_2d.h>

class RadialLensCorrection : public Mapping_2d_2d
{
 public:
  RadialLensCorrection(double cx, double cy, double sx, double sy, double k2, double k4 = 0);
  RadialLensCorrection(double k2);
  RadialLensCorrection(int w, int h, double k2);

 protected: // implementation of Mapping_2d_2d
  virtual void implement_map(double x1, double y1, double* x2, double* y2);
  virtual void implement_inverse_map(double x2, double y2, double* x1, double* y1);

  void init(double cx, double cy, double sx, double sy, double k2, double k4);

  double _cx;
  double _cy;
  double _sx;
  double _sy;
  double _k2;
  double _k4;
  double _invsx;
  double _invsy;
};

#endif // RadialLensCorrection_h_
