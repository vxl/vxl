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
#ifdef __GNUC__
#pragma implementation "vnl_symmetric_eigensystem.h"
#endif
//
// Class: vnl_symmetric_eigensystem
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 29 Aug 96
// Modifications:
//
//-----------------------------------------------------------------------------

#include "vnl_symmetric_eigensystem.h"

vnl_vector<double> vnl_symmetric_eigensystem::get_eigenvector(int i) const
{
  return vnl_vector<double>(V.extract(n_,1,0,i).data_block(), n_);
}

// Declare fortran function
extern "C" int rs_(const int& nm, const int& n,
		   const double *a, double *w,
		   const int& matz, const double *z,
		   const double *fv1, const double *fv2,
		   int* ierr);

// - @{ Solve real symmetric eigensystem $A x = \lambda x$ @}
vnl_symmetric_eigensystem::vnl_symmetric_eigensystem(const vnl_matrix<double>& A)
  : n_(A.rows()), V(n_, n_), D(n_)
{
  vnl_vector<double> Dvec(n_);

  compute(A, V, Dvec);

  // Copy Dvec into diagonal of D
  for(int i = 0; i < n_; ++i)
    D(i,i) = Dvec[i];
}

bool vnl_symmetric_eigensystem::compute(const vnl_matrix<double>& A, vnl_matrix<double>& V, vnl_vector<double>& D)
{
  A.assert_finite();

  int n = A.rows();
  vnl_vector<double> work1(n);
  vnl_vector<double> work2(n);
  vnl_vector<double> Vvec(n*n);
  
  int want_eigenvectors = 1;
  int ierr = 0;

  // No need to transpose A, cos it's symmetric...
  rs_(n, n, A.data_block(), &D[0], want_eigenvectors, &Vvec[0], &work1[0], &work2[0], &ierr);
  
  if (ierr) {
    cerr << "SymmetricEigenSystem: ierr = " << ierr << endl;
    return false;
  }

  // Transpose-copy into V
  double *vptr = &Vvec[0];
  for(int c = 0; c < n; ++c)
    for(int r = 0; r < n; ++r)
      V(r,c) = *vptr++;
  
  return true;
}

//#include <vnl/FastOps.h>
vnl_vector<double> vnl_symmetric_eigensystem::solve(const vnl_vector<double>& b)
{
  //vnl_vector<double> ret(b.length());
  //FastOps::AtB(V, b, &ret);
  vnl_vector<double> ret(b*V); // same as V.tranpose()*b

  vnl_vector<double> tmp(b.size());
  D.solve(ret, &tmp);

  return V * tmp;
}

vnl_matrix<double> vnl_symmetric_eigensystem::pinverse() const
{
  unsigned n=D.n();
  vnl_diag_matrix<double> invD(n);
  for (unsigned i=0;i<n;i++)
    if (D(i,i))
      invD(i,i) = 1.0/D(i,i);
  return V * invD * V.transpose();
}
