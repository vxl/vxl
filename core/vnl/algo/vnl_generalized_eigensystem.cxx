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
#pragma implementation "vnl_generalized_eigensystem.h"
#endif
//
// Class: vnl_generalized_eigensystem
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 29 Aug 96
// Modifications:
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_complex.h>
#include "vnl_generalized_eigensystem.h"
#include <vnl/vnl_fortran_copy.h>
#include <vnl/vnl_matops.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <vnl/algo/vnl_svd.h>

vnl_generalized_eigensystem::vnl_generalized_eigensystem(const vnl_matrix<double>& A,
					       const vnl_matrix<double>& B)
  :
  n(A.rows()), V(n,n), D(n)
{
  compute_eispack(A, B);
}

// Declare Fortran function.

extern "C" int rsg_ (const int& nm, const int& n, const double *a, const double *b,
		     double *w, const int& matz, double *z, double *fv1, double *fv2,
		     int *ierr);

// *****************************************************************************

void vnl_generalized_eigensystem::compute_eispack(const vnl_matrix<double>& A,
					     const vnl_matrix<double>& B)
{
  // Copy source matrices into fortran storage
  vnl_fortran_copy<double> a(A);
  vnl_fortran_copy<double> b(B);

  // Make workspace and storage for V transpose
  vnl_vector<double> work1(n);
  vnl_vector<double> work2(n);
  vnl_vector<double> V1(n*n);

  int want_eigenvectors = 1;
  int ierr = -1;

  // Call EISPACK rsg.  
  rsg_ (n, n, a, b, D.data_block(), 
	want_eigenvectors, 
	V1.begin(), 
	work1.begin(), 
	work2.begin(), &ierr);
  
  // If b was not pos-def, retry with projection onto nullspace
  if (ierr == 7*n+1) {
    const double THRESH = 1e-8;
    vnl_symmetric_eigensystem eig(B);
    if (eig.D(0,0) < -THRESH) {
      cerr << "**** vnl_generalized_eigensystem: ERROR\n";
      cerr << "Matrix B is not nonneg-definite\n";
      MATLABPRINT(B);
      cerr << "**** eigenvalues(B) = " << eig.D << endl;
      return;
    }
    // hmmmmm -- all this crap below is worse than
    // whatever the default is...
    return;
#if 0 // so don't compile it then...
    int rank_deficiency = 0;
    while (eig.D(rank_deficiency,rank_deficiency) < THRESH)
      ++rank_deficiency;
    int rank = B.columns() - rank_deficiency;

    cerr << "vnl_generalized_eigensystem: B rank def by " << rank_deficiency << endl;
    // M is basis for non-nullspace of B
    vnl_matrix<double> M = eig.V.get_n_columns(rank_deficiency, rank);
    vnl_matrix<double> N = eig.V.get_n_columns(0, rank_deficiency);

    vnl_svd<double> svd(M.transpose()*A*N);

    vnl_generalized_eigensystem reduced(M.transpose() * A * M,
					M.transpose() * B * M);
    
    cerr << "AN: " << reduced.D << endl;

    vnl_matrix<double> V05 = M * reduced.V.transpose();
    vnl_svd<double> sv6(V05.transpose());
    V.update(V05, 0, 0);
    V.update(sv6.nullspace(), 0, rank - 1);
    for(int i = 0; i < rank; ++i)
      D(i,i) = reduced.D(i,i);
    for(unsigned i = rank; i < B.columns(); ++i)
      D(i,i) = 0;
    cerr << "AN: " << D << endl;
    
    return;
#endif
  }
  
  // vnl_transpose-copy V1 to V
  {
    double *vptr = &V1[0];
    for(int c = 0; c < n; ++c)
      for(int r = 0; r < n; ++r)
	V(r,c) = *vptr++;
  }
  
  // Diagnose errors
  if (ierr) {
    if (ierr == 10*n)
      cerr << "vnl_generalized_eigensystem: N is greater than NM.  Bug in interface to rsg.f\n";
    else {
      cerr << "vnl_generalized_eigensystem: The " <<
	ierr << "-th eigenvalue has not been determined after 30 iterations.\n";
      cerr << "The eigenvalues should be correct for indices 1.." << ierr-1;
      cerr << ", but no eigenvcl_vcl_vcl_vcl_vectors are computed.\n";
      cerr << "A = " << A << endl;
      cerr << "singular values(A) = " << vnl_svd<double>(A).W() << endl;
      cerr << "B = " << B << endl;
      cerr << "singular values(B) = " << vnl_svd<double>(B).W() << endl;
    }
  }
}
