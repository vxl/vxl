//-*- c++ -*-------------------------------------------------------------------
#ifndef HomgInterestPoint_h_
#define HomgInterestPoint_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : HomgInterestPoint
//
// .SECTION Description
//    HomgInterestPoint is a representation of an interest point
//    resulting from feature detection.  At Oxford this generally
//    means a Harris corner.
//
// .NAME        HomgInterestPoint - Homogeneous interest point/corner.
// .LIBRARY     MViewBasics
// .HEADER	MultiView Package
// .INCLUDE     mvl/HomgInterestPoint.h
// .FILE        HomgInterestPoint.cxx
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 17 Aug 96
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include <vcl/vcl_iosfwd.h>
#include <vnl/vnl_int_2.h>
#include <mvl/HomgPoint2D.h>

class HomgMetric;

class HomgInterestPoint {
  // Data Members--------------------------------------------------------------
public:
  HomgPoint2D _homg;
  vnl_double_2 _double2;
  vnl_int_2 _int2;
public:
  float       _mean_intensity;

  HomgInterestPoint();
  HomgInterestPoint(double x, double y, float mean_intensity = 0.0F);
  HomgInterestPoint(double x, double y, const HomgMetric& c, float mean_intensity = 0.0F);
  HomgInterestPoint(const HomgPoint2D& h, const HomgMetric& c, float mean_intensity = 0.0F);
  HomgInterestPoint(const HomgInterestPoint&); // copy constructor
  HomgInterestPoint& operator=(const HomgInterestPoint&); // assignment
  ~HomgInterestPoint();

  friend bool operator == (const HomgInterestPoint&, const HomgInterestPoint&);
};

ostream& operator << (ostream& s, const HomgInterestPoint&);

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS HomgInterestPoint.

