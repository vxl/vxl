// This is oxl/mvl/HomgPoint2D.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
//  \file

#include <vcl_iostream.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_double_2.h>
#include <mvl/HomgPoint2D.h>

//--------------------------------------------------------------
//
//: Return true if the point is at infinity (an ideal point).
// The method checks that $|w| < \mbox{infinitesimal\_tol} \times \mbox{min}\{|x|, |y|\}$
bool
HomgPoint2D::check_infinity(double tol) const
{
  return vnl_math_abs(get_w()) < tol * vnl_math_min(vnl_math_abs(get_x()), vnl_math_abs(get_y()));
}

//--------------------------------------------------------------
//
//: Return the non-homogeneous coordinates of the point.
// If the point is at infinity, return false.
bool
HomgPoint2D::get_nonhomogeneous(double& ex, double& ey) const
{
  double x = get_x();
  double y = get_y();
  double z = get_w();

  if (z == 0)
    return false;

  // double scale = 1.0 / z;
  ex = x / z;
  ey = y / z;
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
  double x = get_x();
  double y = get_y();
  double z = get_w();

  double norm = vcl_sqrt (vnl_math_sqr(x) + vnl_math_sqr(y) + vnl_math_sqr(z));

  if (norm == 0.0) {
    vcl_cerr << "HomgPoint2D::get_unitized() -- Zero length vector\n";
    return *this;
  }

  norm = 1.0/norm;
  return HomgPoint2D(x*norm, y*norm, z*norm);
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
