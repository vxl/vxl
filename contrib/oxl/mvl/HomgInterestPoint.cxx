#ifdef __GNUC__
#pragma implementation
#endif

#include "HomgInterestPoint.h"

#include <vcl_iostream.h>
#include <vnl/vnl_math.h>
#include <mvl/HomgMetric.h>

HomgInterestPoint::HomgInterestPoint()
{
}

//: Create from HomgPoint2D in conditioned coordinates, using the given metric
// to convert back to image coords.
HomgInterestPoint::HomgInterestPoint(const HomgPoint2D& h, const HomgMetric& metric, float mean_intensity):
  _homg(h),
  _double2(0, 0),
  _int2(0, 0), // will be set below
  _mean_intensity(mean_intensity)
{
  double x, y;
  metric.homg_to_image(h, &x, &y);
  _double2[0] = x;
  _double2[1] = y;
  _int2[0]= vnl_math_rnd(x);
  _int2[1]= vnl_math_rnd(y);
}

HomgInterestPoint::HomgInterestPoint(double x, double y, float mean_intensity):
  _homg(x, y, 1.0),
  _double2(x, y),
  _int2(vnl_math_rnd(x), vnl_math_rnd(y)),
  _mean_intensity(mean_intensity)
{
}

HomgInterestPoint::HomgInterestPoint(double x, double y, const HomgMetric& c, float mean_intensity):
  _homg(c.image_to_homg(x, y)),
  _double2(x, y),
  _int2(vnl_math_rnd(x), vnl_math_rnd(y)),
  _mean_intensity(mean_intensity)
{
}

HomgInterestPoint::HomgInterestPoint(const HomgInterestPoint& that):
  _homg(that._homg),
  _double2(that._double2),
  _int2(that._int2),
  _mean_intensity(that._mean_intensity)
{
}

HomgInterestPoint& HomgInterestPoint::operator=(const HomgInterestPoint& that)
{
  _homg = that._homg;
  _double2 = that._double2;
  _int2 = that._int2;
  _mean_intensity = that._mean_intensity;
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
  return a._double2 == b._double2;
}
