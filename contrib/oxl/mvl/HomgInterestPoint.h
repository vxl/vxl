#ifndef HomgInterestPoint_h_
#define HomgInterestPoint_h_
#ifdef __GNUC__
#pragma interface
#endif
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
//-----------------------------------------------------------------------------

#include <vcl_iosfwd.h>
#include <vnl/vnl_int_2.h>
#include <mvl/HomgPoint2D.h>

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
  HomgInterestPoint(const HomgInterestPoint&); // copy constructor
  HomgInterestPoint& operator=(const HomgInterestPoint&); // assignment
  ~HomgInterestPoint();

  friend bool operator == (const HomgInterestPoint&, const HomgInterestPoint&);
};

vcl_ostream& operator << (vcl_ostream& s, const HomgInterestPoint&);

#endif // HomgInterestPoint_h_
