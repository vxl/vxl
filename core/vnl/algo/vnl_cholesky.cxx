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
#pragma implementation "vnl_cholesky.h"
#endif
//
// Class: vnl_cholesky
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 08 Dec 96
// Modifications:
//
//-----------------------------------------------------------------------------

#include "vnl_cholesky.h"
#include <vnl/vnl_math.h> // for pow()
#include <assert.h>

// from netlib
extern "C" {
  int dpofa_(double *m, const int& lda, const int& n, int* info);
  int dposl_(const double *a, const int& lda, const int& n, double *b);
  int dpoco_(double *a, const int& lda, const int& n, double* rcond, double *z, int *info);
  int dpodi_(double *a, const int& lda, const int& n, double* det, const int& job);
};

// -- Make cholesky decomposition of M optionally computing
// the reciprocal condition number.  If mode is estimate_condition, the
// condition number and an approximate nullspace are estimated, at a cost
// of a factor of (1 + 18/n).  Here's a table of 1 + 18/n:
//<pre>
// n:              3      5     10     50    100    500   1000
// slowdown:     7.0    4.6    2.8    1.4   1.18   1.04   1.02
//</pre>

vnl_cholesky::vnl_cholesky(vnl_matrix<double> const & M, Operation mode):
  A_(M)
{
  assert(M.columns() == M.rows());
  int n = M.columns();
  num_dims_rank_def_ = -1;
  if (fabs(M(0,n-1) - M(n-1,0)) > 1e-8) {
    cerr << "vnl_cholesky: WARNING: unsymmetric: " << M << endl;
  }

  if (mode != estimate_condition) {
    // Quick factorization
    dpofa_(A_.data_block(), n, n, &num_dims_rank_def_);
    if (mode == verbose && num_dims_rank_def_ != 0)
      cerr << "vnl_cholesky:: " << num_dims_rank_def_ << " dimensions of non-posdeffness\n";
  } else {
    vnl_vector<double> nullvector(n);
    dpoco_(A_.data_block(), n, n, &rcond_, nullvector.data_block(), &num_dims_rank_def_);
  }
}

// -- Solve least squares problem M x = b.  The right-hand-side vcl_vector x may be
// b, which will give a fractional increase in speed.
void vnl_cholesky::solve(const vnl_vector<double>& b, vnl_vector<double>* x) const
{
  int n = A_.columns();
  assert(x->size() == n);
  assert(b.size() == n);

  *x = b;
  dposl_(A_.data_block(), n, n, x->data_block());
}

// -- Solve least squares problem M x = b.
vnl_vector<double> vnl_cholesky::solve(const vnl_vector<double>& b) const
{
  vnl_vector<double> ret = b;
  int n = A_.columns();
  assert(b.size() == n);
  dposl_(A_.data_block(), n, n, ret.data_block());
  return ret;
}

// -- Compute determinant.
double vnl_cholesky::determinant() const
{
  int n = A_.columns();
  double det[2];
  dpodi_((double*)A_.data_block(), n, n, det, 10);
  return det[0] * pow(10, det[1]);
}

// -- Compute inverse.  Not efficient.
vnl_matrix<double> vnl_cholesky::inverse() const
{
  int n = A_.columns();
  vnl_matrix<double> I = A_;
  dpodi_(I.data_block(), n, n, 0, 01);

  // Copy lower triangle into upper
  for(int i = 0; i < n; ++i)
    for(int j = i+1; j < n; ++j)
      I(i,j) = I(j,i);
  
  return I;
}

// -- Return Upper-triangular factor.
vnl_matrix<double> vnl_cholesky::upper_triangle() const
{
  int n = A_.columns();
  vnl_matrix<double> U(n,n);
  // Zap lower triangle and transpose
  for(int i = 0; i < n; ++i) {
    U(i,i) = A_(i,i);
    for(int j = i+1; j < n; ++j) {
      U(i,j) = A_(j,i);
      U(j,i) = 0;
    }
  }
  return U;
}
  
