// This is oxl/mvl/HomgPoint3D.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
//  \file

#include <vcl_iostream.h>
#include <vcl_cmath.h>
//#include <vcl_memory.h>
#include <vcl_cstdlib.h>

////////////////#include <Geometry/IUPoint.h>
#include <mvl/Homg3D.h>
#include <mvl/HomgPoint3D.h>

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
bool
HomgPoint3D::check_infinity(void) const
{
  return get_w() == 0;
}

//--------------------------------------------------------------
//
//: Return true if the point is at infinity.
// Check that max(|x|, |y|, |z|) < tol * |w|
bool
HomgPoint3D::check_infinity(double tol) const
{
  double hx = vcl_fabs(get_x());
  double hy = vcl_fabs(get_y());
  double hz = vcl_fabs(get_z());
  double hw = vcl_fabs(get_w());

  double max = hx;
  if (hy > max) max = hy;
  if (hz > max) max = hz;

  return max < tol * hw;
}

//--------------------------------------------------------------
//
//: Return the non-homogeneous coordinates of the point.
// If the point is at infinity, return false and set the
// output values to Homg::infinity.
bool
HomgPoint3D::get_nonhomogeneous(double& x, double& y, double& z) const
{
  double hx = get_x();
  double hy = get_y();
  double hz = get_z();
  double hw = get_w();
  if (hw == 0) {
    x = y = z = Homg::infinity;
    return false;
  }

  hw = 1.0/hw;
  x = hx * hw;
  y = hy * hw;
  z = hz * hw;

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
