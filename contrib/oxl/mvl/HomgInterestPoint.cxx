// This is oxl/mvl/HomgInterestPoint.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
//  \file

#include "HomgInterestPoint.h"

#include <vcl_iostream.h>
#include <vnl/vnl_math.h>
#include <mvl/HomgMetric.h>

HomgInterestPoint::HomgInterestPoint()
{
}

//: Create from HomgPoint2D in conditioned coordinates.
// Using the given metric to convert back to image coords.
HomgInterestPoint::HomgInterestPoint(const HomgPoint2D& h, const HomgMetric& metric, float mean_intensity):
  homg_(h),
  double2_(0, 0),
  int2_(0, 0), // will be set below
  mean_intensity_(mean_intensity)
{
  double x, y;
  metric.homg_to_image(h, &x, &y);
  double2_[0] = x;
  double2_[1] = y;
  int2_[0]= vnl_math_rnd(x);
  int2_[1]= vnl_math_rnd(y);
}

HomgInterestPoint::HomgInterestPoint(double x, double y, float mean_intensity):
  homg_(x, y, 1.0),
  double2_(x, y),
  int2_(vnl_math_rnd(x), vnl_math_rnd(y)),
  mean_intensity_(mean_intensity)
{
}

HomgInterestPoint::HomgInterestPoint(double x, double y, const HomgMetric& c, float mean_intensity):
  homg_(c.image_to_homg(x, y)),
  double2_(x, y),
  int2_(vnl_math_rnd(x), vnl_math_rnd(y)),
  mean_intensity_(mean_intensity)
{
}

HomgInterestPoint::HomgInterestPoint(const HomgInterestPoint& that):
  homg_(that.homg_),
  double2_(that.double2_),
  int2_(that.int2_),
  mean_intensity_(that.mean_intensity_)
{
}

HomgInterestPoint& HomgInterestPoint::operator=(const HomgInterestPoint& that)
{
  homg_ = that.homg_;
  double2_ = that.double2_;
  int2_ = that.int2_;
  mean_intensity_ = that.mean_intensity_;
  return *this;
}

HomgInterestPoint::~HomgInterestPoint()
{
}

vcl_ostream& operator<<(vcl_ostream& s, const HomgInterestPoint&)
{
  return s;
}

bool operator==(const HomgInterestPoint& a, const HomgInterestPoint& b)
{
  return a.double2_ == b.double2_;
}
