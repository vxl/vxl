// This is oxl/mvl/HomgPlane3D.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
//  \file

#include <vcl_iostream.h>
#include <vcl_cmath.h>
//#include <vcl_memory.h>
#include <vcl_cstdlib.h>
#include <vnl/vnl_double_3.h>
#include <mvl/Homg3D.h>
#include <mvl/HomgPlane3D.h>
#include <mvl/HomgPoint3D.h>


//--------------------------------------------------------------
//
//: Constructor
HomgPlane3D::HomgPlane3D ()
{
}

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
HomgPlane3D::HomgPlane3D (const vnl_vector<double>& n, double d)
  : Homg3D (n[0], n[1], n[2], -d)
{
}

//--------------------------------------------------------------
//
//: Destructor
HomgPlane3D::~HomgPlane3D ()
{
}

//: closest point
HomgPoint3D HomgPlane3D::closest_point(const HomgPoint3D& x) const
{
  vnl_double_3 n(_homg_vector[0], _homg_vector[1], _homg_vector[2]);

  double s = 1.0/n.magnitude();
  double d = -_homg_vector[3];

  n *= s;
  d *= s;

  vnl_double_3 x3 = x.get_double3();

  double dp = dot_product(x3, n) - d;

  vnl_double_3 px = x3 - dp * n;

  return HomgPoint3D(px[0], px[1], px[2], 1.0);
}

//: Distance point to plane
double HomgPlane3D::distance(const HomgPoint3D& x) const
{
  vnl_double_3 n(_homg_vector[0], _homg_vector[1], _homg_vector[2]);

  double s = 1.0/n.magnitude();
  double d = -_homg_vector[3];

  vnl_double_3 x3 = x.get_double3();

  return (dot_product(x3, n) - d)*s;
}

//: print
vcl_ostream& operator<<(vcl_ostream& s, const HomgPlane3D& P)
{
  return s << P.get_vector();
}
