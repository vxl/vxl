#ifndef HomgOperator1D_h_
#define HomgOperator1D_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME    HomgOperator1D - 1D homogeneous functions
// .LIBRARY MViewBasics
// .HEADER  MultiView Package
// .INCLUDE mvl/HomgOperator1D.h
// .FILE    HomgOperator1D.cxx
//
// .SECTION Description:
// HomgOperator1D implements one-dimensional homogeneous functions.
//
// .SECTION Author:
//     Peter Vanroose, ESAT/PSI, Nov. 1998.
//
//-----------------------------------------------------------------------------
#include <mvl/HomgPoint1D.h>

class HomgOperator1D {
public:
  static double CrossRatio(const Homg1D& a, const Homg1D& b,
                           const Homg1D& c, const Homg1D& d);
  static double Conjugate(double x1, double x2, double x3, double cr = -1);
  static Homg1D Conjugate(const Homg1D& a, const Homg1D& b, const Homg1D& c,
                          double cr = -1);

  static double dot(const Homg1D& a, const Homg1D& b);
  static double cross(const Homg1D& a, const Homg1D& b);
  static void unitize(Homg1D* a);

  static double distance (const HomgPoint1D& point1, const HomgPoint1D& point2);
  static double distance_squared (const HomgPoint1D& point1, const HomgPoint1D& point2);

  static bool is_within_distance(const HomgPoint1D& p1, const HomgPoint1D& p2, double d) {
    return distance(p1, p2) < d;
  }
  static HomgPoint1D midpoint (const HomgPoint1D& p1, const HomgPoint1D& p2);
};

#endif // HomgOperator1D_h_
