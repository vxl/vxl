// This is oxl/mvl/HomgPoint2D.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
//  \file

#include "HomgPoint2D.h"
#include <vcl_iostream.h>
#include <vcl_cmath.h> // for fabs
#include <vnl/vnl_double_2.h>

//--------------------------------------------------------------
//
//: Return true if the point is at infinity (an ideal point).
// The method checks that $|w| <= \mbox{tol} \times \mbox{max}\{|x|, |y|\}$
bool
HomgPoint2D::check_infinity(double tol) const
{
  // quick return if possible
  if (tol==0.0) return w()==0;

  return vcl_fabs(w()) <= tol * vcl_fabs(x())
      || vcl_fabs(w()) <= tol * vcl_fabs(y());
}

//--------------------------------------------------------------
//
//: Return the non-homogeneous coordinates of the point.
// If the point is at infinity, return false.
bool
HomgPoint2D::get_nonhomogeneous(double& ex, double& ey) const
{
  double hx = x();
  double hy = y();
  double hz = w();

  if (hz == 0)
  {
    ex = ey = Homg::infinity;
    return false;
  }

  ex = hx / hz;
  ey = hy / hz;
  return true;
}

//-----------------------------------------------------------------------------
//
//: returns a non-homogeneous coolvector of length 2.
vnl_double_2 HomgPoint2D::get_double2() const
{
  double scale = 1.0 / (*this)[2];
  return vnl_double_2((*this)[0] * scale, (*this)[1] * scale);
}

//-----------------------------------------------------------------------------
//
//: returns a unit-norm scaled copy of this.
HomgPoint2D HomgPoint2D::get_unitized() const
{
  double norm = x()*x() + y()*y() + w()*w();

  if (norm == 0.0) {
    vcl_cerr << "HomgPoint2D::get_unitized() -- Zero length vector\n";
    return *this;
  }

  norm = 1.0/vcl_sqrt(norm);
  return HomgPoint2D(x()*norm, y()*norm, w()*norm);
}

//-----------------------------------------------------------------------------
//
//: Print to vcl_ostream as <HomgPoint2D x y z>
vcl_ostream& operator<<(vcl_ostream& s, const HomgPoint2D& p)
{
  return s << "<HomgPoint2D " << p.get_vector() << ">";
}

//-----------------------------------------------------------------------------
//
//: Read from ASCII vcl_istream.  Assumes points are stored in homogeneous form as 3 reals.
vcl_istream& operator>>(vcl_istream& is, HomgPoint2D& p)
{
  double x, y, z;
  is >> x >> y >> z;
  if (is.good())
    p.set(x,y,z);
  return is;
}

//-----------------------------------------------------------------------------
//
//: read from vcl_istream.  Default is to assume that points are nonhomogeneous
// 2D, set is_homogeneous to true if points are already in homogeneous form.
HomgPoint2D HomgPoint2D::read(vcl_istream& is, bool is_homogeneous)
{
  if (is_homogeneous) {
    HomgPoint2D ret;
    is >> ret;
    return ret;
  }
  double x, y;
  is >> x >> y;
  return HomgPoint2D(x, y, 1.0);
}
