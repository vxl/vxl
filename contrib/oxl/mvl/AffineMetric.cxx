// <begin copyright notice>
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1997 TargetJr Consortium
//               GE Corporate Research and Development (GE CRD)
//                             1 Research Circle
//                            Niskayuna, NY 12309
//                            All Rights Reserved
//              Reproduction rights limited as described below.
//                               
//      Permission to use, copy, modify, distribute, and sell this software
//      and its documentation for any purpose is hereby granted without fee,
//      provided that (i) the above copyright notice and this permission
//      notice appear in all copies of the software and related documentation,
//      (ii) the name TargetJr Consortium (represented by GE CRD), may not be
//      used in any advertising or publicity relating to the software without
//      the specific, prior written permission of GE CRD, and (iii) any
//      modifications are clearly marked and summarized in a change history
//      log.
//       
//      THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
//      EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
//      WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//      IN NO EVENT SHALL THE TARGETJR CONSORTIUM BE LIABLE FOR ANY SPECIAL,
//      INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND OR ANY
//      DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
//      WHETHER OR NOT ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, OR ON
//      ANY THEORY OF LIABILITY ARISING OUT OF OR IN CONNECTION WITH THE
//      USE OR PERFORMANCE OF THIS SOFTWARE.
//
// ---------------------------------------------------------------------------
// <end copyright notice>
//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation "AffineMetric.h"
#endif
//
// Class: AffineMetric
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 24 Feb 97
// Modifications:
//   970224 AWF Initial version.
//
//-----------------------------------------------------------------------------

#include "AffineMetric.h"

#include <vcl/vcl_iostream.h>

#include <vnl/algo/vnl_svd.h>

#include <mvl/HomgPoint2D.h>

// -- Construct and AffineMetric from the matrix A.
// The last row of A should be (0,0,lambda).
AffineMetric::AffineMetric()
{
  A_.set_identity();
  A_inverse_.set_identity();
}  



// -- Construct and AffineMetric from the matrix A.
// The last row of A should be (0,0,lambda).
AffineMetric::AffineMetric(const vnl_matrix<double>& A):
  A_(A),
  A_inverse_(vnl_svd<double>(A).inverse())
{
  assert(A(2,0) == 0);
  assert(A(2,1) == 0);
}  

void AffineMetric::set(const vnl_matrix<double>& A)
{
  A_ = A;
  A_inverse_ = vnl_svd<double>(A).inverse();
  assert(A(2,0) == 0);
  assert(A(2,1) == 0);
}  

// -- set [a11  0 a13; 0 a22 a23; 0 0 a33]
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

// @{ Implementation of ImageMetric @}

// -- Transform p
HomgPoint2D AffineMetric::homg_to_imagehomg(const HomgPoint2D& p)
{
  return A_ * p.get_vector();
}

// -- Inverse-transform p
HomgPoint2D AffineMetric::imagehomg_to_homg(const HomgPoint2D& p)
{
  return A_inverse_ * p.get_vector();
}

HomgPoint2D AffineMetric::image_to_homg(double x, double y)
{
  double** a = A_inverse_.data_array();
  double h1 = a[0][0] * x + a[0][1] * y + a[0][2];
  double h2 = a[1][0] * x + a[1][1] * y + a[1][2];
  double h3 = a[2][0] * x + a[2][1] * y + a[2][2];

  return HomgPoint2D(h1, h2, h3);
}

// -- Convert conditioned point p to image coordinates
vnl_double_2 AffineMetric::homg_to_image(const HomgPoint2D& p)
{
  vnl_double_3 x = A_ * p.get_vector();
  double s = 1/x[2];
  
  return vnl_double_2(x[0] * s, x[1] * s);
}

/*
// -- Return forward transformation matrix
const vnl_matrix<double>& AffineMetric::get_C() const
{
  return A_;
}

// -- Return inverse transformation matrix
const vnl_matrix<double>& AffineMetric::get_C_inverse() const
{
  return A_inverse_;
}

// -- Declare that this is a linear transformation
bool AffineMetric::is_linear() const
{
  return true;
}

// -- Declare that this is not an isometry
bool AffineMetric::can_invert_distance() const
{
  return false;
}
*/

// -- print to ostream
ostream& AffineMetric::print(ostream& s) const
{
  return s << "AffineMetric [" << A_.get_row(0) << ";" << A_.get_row(1) << ";" << A_.get_row(2) << "]";
}
