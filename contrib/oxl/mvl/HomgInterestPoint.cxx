// This is oxl/mvl/HomgInterestPoint.cxx
//:
//  \file

#include <iostream>
#include "HomgInterestPoint.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h>
#include <mvl/HomgMetric.h>

HomgInterestPoint::HomgInterestPoint() = default;

//: Create from vgl_homg_point_2d<double> in conditioned coordinates.
// Using the given metric to convert back to image coordinates.
HomgInterestPoint::HomgInterestPoint(const vgl_homg_point_2d<double>& h, const HomgMetric& metric, float mean_intensity):
  homg_(h.x(),h.y(),h.w()),
  double2_(0, 0),
  int2_(0, 0), // will be set below
  mean_intensity_(mean_intensity)
{
  double x, y;
  metric.homg_to_image(homg_, &x, &y);
  double2_[0] = x;
  double2_[1] = y;
  int2_[0]= vnl_math::rnd(x);
  int2_[1]= vnl_math::rnd(y);
}

//: Create from HomgPoint2D in conditioned coordinates.
// Using the given metric to convert back to image coordinates.
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
  int2_[0]= vnl_math::rnd(x);
  int2_[1]= vnl_math::rnd(y);
}

HomgInterestPoint::HomgInterestPoint(double x, double y, float mean_intensity):
  homg_(x, y, 1.0),
  double2_(x, y),
  int2_(vnl_math::rnd(x), vnl_math::rnd(y)),
  mean_intensity_(mean_intensity)
{
}

HomgInterestPoint::HomgInterestPoint(double x, double y, const HomgMetric& c, float mean_intensity):
  homg_(c.image_to_homg(x, y)),
  double2_(x, y),
  int2_(vnl_math::rnd(x), vnl_math::rnd(y)),
  mean_intensity_(mean_intensity)
{
}

HomgInterestPoint::HomgInterestPoint(const HomgInterestPoint& that) = default;

HomgInterestPoint& HomgInterestPoint::operator=(const HomgInterestPoint& that) = default;

HomgInterestPoint::~HomgInterestPoint() = default;

std::ostream& operator<<(std::ostream& s, const HomgInterestPoint&)
{
  return s;
}

bool operator==(const HomgInterestPoint& a, const HomgInterestPoint& b)
{
  return a.double2_ == b.double2_;
}
