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
#include <vcl/vcl_cassert.h>
#include <vcl/vcl_iostream.h>
#include <vnl/vnl_math.h> // pow()
#include <vnl/algo/vnl_netlib.h> // dpofa_(), dposl_(), dpoco_(), dpodi_()

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
  
