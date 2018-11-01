// This is oxl/mvl/HomgPlane3D.cxx
//:
//  \file

#include <iostream>
#include "HomgPlane3D.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_double_3.h>
#include <mvl/Homg3D.h>
#include <mvl/HomgPoint3D.h>


//--------------------------------------------------------------
//
//: Constructor
HomgPlane3D::HomgPlane3D () = default;

//--------------------------------------------------------------
//
//: Constructor
HomgPlane3D::HomgPlane3D (double x, double y, double z, double w)
    : Homg3D (x, y, z, w)
{
}

//--------------------------------------------------------------
//
//: Constructor
HomgPlane3D::HomgPlane3D (const vnl_double_3& n, double d)
  : Homg3D (n[0], n[1], n[2], -d)
{
}

//--------------------------------------------------------------
//
//: Destructor
HomgPlane3D::~HomgPlane3D () = default;

//: closest point
HomgPoint3D HomgPlane3D::closest_point(const HomgPoint3D& p) const
{
  vnl_double_3 n(x(), y(), z());

  double s = 1.0/n.magnitude();
  double d = -w();

  n *= s;
  d *= s;

  vnl_double_3 x3 = p.get_double3();

  double dp = dot_product(x3, n) - d;

  vnl_double_3 px = x3 - dp * n;

  return HomgPoint3D(px[0], px[1], px[2], 1.0);
}

//: Distance point to plane
double HomgPlane3D::distance(const HomgPoint3D& p) const
{
  vnl_double_3 n(x(), y(), z());

  double s = 1.0/n.magnitude();
  double d = -w();

  vnl_double_3 x3 = p.get_double3();

  return (dot_product(x3, n) - d)*s;
}

//: print
std::ostream& operator<<(std::ostream& s, const HomgPlane3D& P)
{
  return s << P.get_vector();
}
