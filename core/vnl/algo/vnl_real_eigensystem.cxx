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
#pragma implementation "vnl_real_eigensystem.h"
#endif
//
// Class: vnl_real_eigensystem
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 23 Jan 97
// Modifications:
//
//-----------------------------------------------------------------------------

#include "vnl_real_eigensystem.h"

#include <vnl/vnl_matops.h>
#include <vnl/vnl_fortran_copy.h>

extern "C"
int rg_(const int& nm, const int& n, const double* a, double* wr, double* wi, const int& matz, double* z,
	int* iv1, double* fv1, int* ierr);

// -- Extract eigensystem of unsymmetric matrix M, using the EISPACK routine
// rg.  Should probably switch to using LAPACK's dgeev to avoid transposing.
vnl_real_eigensystem::vnl_real_eigensystem(vnl_matrix<double> const & M):
  Vreal(M.rows(), M.columns()),
  V(M.rows(), M.columns()),
  D(M.rows())
{
  int n = M.rows();
  assert(n == (int)M.columns());

  vnl_fortran_copy<double> mf(M);

  vnl_vector<double> wr(n);
  vnl_vector<double> wi(n);
  vnl_vector<int> iv1(n);
  vnl_vector<double> fv1(n);
  vnl_matrix<double> devout(n, n);

  int ierr = 0;
  int matz = 1;
  rg_(n, n, mf, wr.data_block(), wi.data_block(), matz, devout.data_block(), iv1.data_block(), fv1.data_block(), &ierr);
  
  if (ierr != 0) {
    cerr << " *** vnl_real_eigensystem: Failed on " << ierr << "th eigenvalue\n";
    cerr << M << endl;
  }

  // Copy out eigenvalues and eigenvectors
  for(int c = 0; c < n; ++c) {
    D(c,c) = vnl_double_complex(wr[c], wi[c]);
    if (wi[c] != 0) {
      // Complex -- copy conjugates and inc c.
      D(c+1, c+1) = vnl_double_complex(wr[c], -wi[c]);
      for(int r = 0; r < n; ++r) {
	V(r, c) = vnl_double_complex(devout(c,r), devout(c+1,r));
	V(r, c+1) = vnl_double_complex(devout(c,r), -devout(c+1,r));
      }
      
      ++c;
    } else
      for(int r = 0; r < n; ++r) {
	V(r, c) = vnl_double_complex(devout(c,r), 0);
	Vreal(r,c) = devout(c,r);
      }
  }
}
