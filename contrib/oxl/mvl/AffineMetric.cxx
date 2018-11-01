// This is oxl/mvl/AffineMetric.cxx
//:
// \file

#include <iostream>
#include "AffineMetric.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

#include <vnl/vnl_inverse.h>

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <mvl/HomgPoint2D.h>

#include <vgl/algo/vgl_homg_operators_2d.h> // for matrix * point

//: Construct and AffineMetric from the matrix A.
// The last row of A should be (0,0,lambda).
AffineMetric::AffineMetric()
{
  A_.set_identity();
  A_inverse_.set_identity();
}


//: Construct and AffineMetric from the 3x3 matrix A.
// The last row of A should be (0,0,lambda).
AffineMetric::AffineMetric(vnl_matrix_fixed<double,3,3> const& A):
  A_(A),
  A_inverse_(vnl_inverse(A))
{
  assert(A(2,0) == 0);
  assert(A(2,1) == 0);
}

void AffineMetric::set(vnl_matrix_fixed<double,3,3> const& A)
{
  A_ = A;
  A_inverse_ = vnl_inverse(A);
  assert(A(2,0) == 0);
  assert(A(2,1) == 0);
}

//: set [a11  0 a13; 0 a22 a23; 0 0 a33]
void AffineMetric::set(double a11, double a13,
                       double a22, double a23,
                       double a33)
{
  A_(0,0) = a11;
  A_(0,1) =   0;
  A_(0,2) = a13;
  A_(1,0) =   0;
  A_(1,1) = a22;
  A_(1,2) = a23;
  A_(2,0) =   0;
  A_(2,1) =   0;
  A_(2,2) = a33;

  A_inverse_ = vnl_inverse(A_);
}

// == Implementation of AffineMetric ==

//: Convert homogeneous point in conditioned coordinates to one in image coordinates
vgl_homg_point_2d<double> AffineMetric::homg_to_imagehomg(vgl_homg_point_2d<double> const& p) const
{
  return A_ * p;
}

//: Transform p
HomgPoint2D AffineMetric::homg_to_imagehomg(const HomgPoint2D& p) const
{
   return HomgPoint2D(A_ * p.get_vector());
}

//: Inverse-transform p
vgl_homg_point_2d<double> AffineMetric::imagehomg_to_homg(vgl_homg_point_2d<double> const& p) const
{
  return A_inverse_ * p;
}

//: Inverse-transform p
HomgPoint2D AffineMetric::imagehomg_to_homg(const HomgPoint2D& p) const
{
  return HomgPoint2D(A_inverse_ * p.get_vector());
}

vgl_homg_point_2d<double> AffineMetric::image_to_homg(vgl_point_2d<double> const& p) const
{
  const vnl_double_3x3& a = A_inverse_;
  double h1 = a[0][0] * p.x() + a[0][1] * p.y() + a[0][2];
  double h2 = a[1][0] * p.x() + a[1][1] * p.y() + a[1][2];
  double h3 = a[2][0] * p.x() + a[2][1] * p.y() + a[2][2];

  return {h1, h2, h3};
}

HomgPoint2D AffineMetric::image_to_homg(double x, double y) const
{
  const vnl_double_3x3& a = A_inverse_;
  double h1 = a[0][0] * x + a[0][1] * y + a[0][2];
  double h2 = a[1][0] * x + a[1][1] * y + a[1][2];
  double h3 = a[2][0] * x + a[2][1] * y + a[2][2];

  return HomgPoint2D(h1, h2, h3);
}

HomgPoint2D AffineMetric::image_to_homg(const vnl_double_2& p) const
{
  return image_to_homg(p[0], p[1]);
}

//: Convert conditioned point p to image coordinates
vgl_point_2d<double> AffineMetric::homg_to_image(vgl_homg_point_2d<double> const& p) const
{
  return A_ * p;
}

//: Convert conditioned point p to image coordinates
vnl_double_2 AffineMetric::homg_to_image(const HomgPoint2D& p) const
{
  vnl_double_3 x = A_ * p.get_vector();
  double s = 1/x[2];

  return {x[0] * s, x[1] * s};
}

//: print to std::ostream
std::ostream& AffineMetric::print(std::ostream& s) const
{
  return s << "AffineMetric ["
           << A_.get_row(0) << ';'
           << A_.get_row(1) << ';'
           << A_.get_row(2) << ']';
}
