// This is oxl/mvl/HomgPoint3D.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "HomgPoint3D.h"
#include <vcl_iostream.h>

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
//--------------------------------------------------------------
//
//: Rescale point
// If the point is at infinity, return false
bool
HomgPoint3D::rescale_w(double new_w)
{
  double hx = x();
  double hy = y();
  double hz = z();
  double hw = w();

  if (hw == 0)
    return false;

  hw = new_w/hw;

  homg_vector_[0] = hx*hw;
  homg_vector_[1] = hy*hw;
  homg_vector_[2] = hz*hw;
  homg_vector_[3] = new_w;

  return true;
}

//
//: Print to vcl_ostream
vcl_ostream& operator<<(vcl_ostream& s, const HomgPoint3D& p)
{
  return s << "<HomgPoint3D " << p.get_vector() << '>';
}
