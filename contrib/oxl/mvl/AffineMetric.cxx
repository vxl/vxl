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
AffineMetric::AffineMetric(const vnl_matrix<double>& A):
  A_(A),
  _A_inverse(vnl_svd<double>(A).inverse())
{
  assert(A(2,0) == 0);
  assert(A(2,1) == 0);
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
  return _A_inverse * p.get_vector();
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
  return _A_inverse;
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
  return s << "AffineMetric [" << A_.get_row(1) << ";" << A_.get_row(2) << ";" << A_.get_row(3) << "]";
}
