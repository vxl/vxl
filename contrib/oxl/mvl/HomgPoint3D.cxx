// This is oxl/mvl/HomgPoint3D.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
//  \file

#include "HomgPoint3D.h"
#include <vcl_iostream.h>
#include <vcl_cmath.h>

//--------------------------------------------------------------
//
// Constructor and Destructor Functions
//
//--------------------------------------------------------------

// - Constructor
HomgPoint3D::HomgPoint3D ()
{
}

// - Constructor
HomgPoint3D::HomgPoint3D (double x, double y, double z)
    : Homg3D (x, y, z, 1)
{
}

//: Constructor. The homogeneous parameter w defaults to 1.
HomgPoint3D::HomgPoint3D (double x, double y, double z, double w)
    : Homg3D (x, y, z, w)
{
}

// - Destructor
HomgPoint3D::~HomgPoint3D ()
{
}


//--------------------------------------------------------------
//
//: Return true if the point is at infinity
//--------------------------------------------------------------
//
//: Return true if the point is at infinity.
// Check that |w| <= max(|x|, |y|, |z|) * tol
bool
HomgPoint3D::check_infinity(double tol) const
{
  // quick return if possible
  if (tol==0.0) return w()==0;

  double hx = vcl_fabs(x());
  double hy = vcl_fabs(y());
  double hz = vcl_fabs(z());
  double hw = vcl_fabs(w());

  double max = hx;
  if (hy > max) max = hy;
  if (hz > max) max = hz;

  return hw <= tol * max;
}

//--------------------------------------------------------------
//
//: Return the non-homogeneous coordinates of the point.
// If the point is at infinity, return false and set the
// output values to Homg::infinity.
bool
HomgPoint3D::get_nonhomogeneous(double& vx, double& vy, double& vz) const
{
  double hx = x();
  double hy = y();
  double hz = z();
  double hw = w();
  if (hw == 0) {
    vx = vy = vz = Homg::infinity;
    return false;
  }

  hw = 1.0/hw;
  vx = hx * hw;
  vy = hy * hw;
  vz = hz * hw;

  return true;
}

//--------------------------------------------------------------
//
//: Return the non-homogeneous coordinates of the point as a vector of doubles
// If the point is at infinity, return a vector of Homg::infinity
vnl_double_3
HomgPoint3D::get_double3() const
{
  vnl_double_3 ret;
  get_nonhomogeneous(ret[0], ret[1], ret[2]);
  return ret;
}

//-----------------------------------------------------------------------------
//
//: Print to vcl_ostream
vcl_ostream& operator<<(vcl_ostream& s, const HomgPoint3D& p)
{
  return s << "<HomgPoint3D " << p.get_vector() << ">";
}
