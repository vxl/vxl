// <begin copyright notice>
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1997 TargetJr Consortium
//               GE Corporate Research and Development (GE CRD)
//                             1 Research Circle
//                            Niskayuna, NY 12309
//                            All Rights Reserved
//              Reproduction rights limited as described below.
//                               
//      Permission to use, copy, modify, distribute, and sell this software
//      and its documentation for any purpose is hereby granted without fee,
//      provided that (i) the above copyright notice and this permission
//      notice appear in all copies of the software and related documentation,
//      (ii) the name TargetJr Consortium (represented by GE CRD), may not be
//      used in any advertising or publicity relating to the software without
//      the specific, prior written permission of GE CRD, and (iii) any
//      modifications are clearly marked and summarized in a change history
//      log.
//       
//      THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
//      EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
//      WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//      IN NO EVENT SHALL THE TARGETJR CONSORTIUM BE LIABLE FOR ANY SPECIAL,
//      INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND OR ANY
//      DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
//      WHETHER OR NOT ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, OR ON
//      ANY THEORY OF LIABILITY ARISING OUT OF OR IN CONNECTION WITH THE
//      USE OR PERFORMANCE OF THIS SOFTWARE.
//
// ---------------------------------------------------------------------------
// <end copyright notice>
//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation "HomgInterestPoint.h"
#endif
//
// Class: HomgInterestPoint
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 17 Aug 96
// Modifications:
//
//-----------------------------------------------------------------------------

#include "HomgInterestPoint.h"

#include <vcl/vcl_iostream.h>
#include <vnl/vnl_math.h>
#include <mvl/HomgMetric.h>

HomgInterestPoint::HomgInterestPoint()
{
}

// -- Create from HomgPoint2D in conditioned coordinates, using the given metric
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

ostream& operator<<(ostream& s, const HomgInterestPoint&) 
{
  return s;
}

bool operator==(const HomgInterestPoint& a, const HomgInterestPoint& b)
{
  return a._double2 == b._double2;
}
