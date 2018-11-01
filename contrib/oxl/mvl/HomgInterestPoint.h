// This is oxl/mvl/HomgInterestPoint.h
#ifndef HomgInterestPoint_h_
#define HomgInterestPoint_h_
//:
// \file
// \brief Homogeneous interest point/corner
//
//    HomgInterestPoint is a representation of an interest point
//    resulting from feature detection.  At Oxford this generally
//    means a Harris corner.
//
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 17 Aug 96
//
// \verbatim
// Modifications:
//   Peter Vanroose - 22 oct.02 - added vgl_homg_point_2d interface
// \endverbatim
//-----------------------------------------------------------------------------

#include <iostream>
#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_int_2.h>
#include <mvl/HomgPoint2D.h>
#include <vgl/vgl_homg_point_2d.h>

class HomgMetric;

class HomgInterestPoint
{
  // Data Members--------------------------------------------------------------
 public:
  HomgPoint2D  homg_;
  vnl_double_2 double2_;
  vnl_int_2    int2_;
  float        mean_intensity_;

 public:
  HomgInterestPoint();
  HomgInterestPoint(double x, double y, float mean_intensity = 0.0F);
  HomgInterestPoint(double x, double y, const HomgMetric& c, float mean_intensity = 0.0F);
  HomgInterestPoint(const HomgPoint2D& h, const HomgMetric& c, float mean_intensity = 0.0F);
  HomgInterestPoint(vgl_homg_point_2d<double> const& h, const HomgMetric& c, float mean_intensity = 0.0F);
  HomgInterestPoint(const HomgInterestPoint&); // copy constructor
  HomgInterestPoint& operator=(const HomgInterestPoint&); // assignment
  ~HomgInterestPoint();

  friend bool operator == (const HomgInterestPoint&, const HomgInterestPoint&);
};

std::ostream& operator << (std::ostream& s, const HomgInterestPoint&);

#endif // HomgInterestPoint_h_
