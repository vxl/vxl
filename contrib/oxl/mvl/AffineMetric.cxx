// This is oxl/mvl/AffineMetric.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "AffineMetric.h"

#include <vcl_iostream.h>
#include <vcl_cassert.h>

#include <vnl/algo/vnl_svd.h>

#include <mvl/HomgPoint2D.h>

//: Construct and AffineMetric from the matrix A.
// The last row of A should be (0,0,lambda).
AffineMetric::AffineMetric():
A_(3,3), A_inverse_(3,3)
{
  A_.set_identity();
  A_inverse_.set_identity();
}


//: Construct and AffineMetric from the matrix A.
// The last row of A should be (0,0,lambda).
AffineMetric::AffineMetric(const vnl_matrix<double>& A):
  A_(A),
  A_inverse_(vnl_svd<double>(A).inverse())
{
  assert(A(2,0) == 0);
  assert(A(2,1) == 0);
}

void AffineMetric::set(vnl_matrix<double> const& A)
{
  A_ = A;
  A_inverse_ = vnl_svd<double>(A).inverse();
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

  A_inverse_ = vnl_svd<double>(A_).inverse();
}

// == Implementation of ImageMetric ==

//: Transform p
HomgPoint2D AffineMetric::homg_to_imagehomg(const HomgPoint2D& p) const
{
   return A_ * p.get_vector();
}

//: Inverse-transform p
HomgPoint2D AffineMetric::imagehomg_to_homg(const HomgPoint2D& p) const
{
  return A_inverse_ * p.get_vector();
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
  return image_to_homg(p.x(), p.y());
}

//: Convert conditioned point p to image coordinates
vnl_double_2 AffineMetric::homg_to_image(const HomgPoint2D& p) const
{
  vnl_double_3 x = A_ * p.get_vector();
  double s = 1/x[2];

  return vnl_double_2(x[0] * s, x[1] * s);
}

#if 0
const vnl_matrix<double>& AffineMetric::get_C() const
{
  return A_;
}

const vnl_matrix<double>& AffineMetric::get_C_inverse() const
{
  return A_inverse_;
}

 bool AffineMetric::is_linear() const
{
  return true;
}

bool AffineMetric::can_invert_distance() const
{
  return false;
}
#endif

//: print to vcl_ostream
vcl_ostream& AffineMetric::print(vcl_ostream& s) const
{
  return s << "AffineMetric [" << A_.get_row(0) << ";" << A_.get_row(1) << ";" << A_.get_row(2) << "]";
}
