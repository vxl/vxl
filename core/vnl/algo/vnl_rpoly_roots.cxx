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
#pragma implementation
#endif
//
// Class: vnl_rpoly_roots
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 06 Aug 96
// Modifications:
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_complex.h>
#include "vnl_rpoly_roots.h"
#include <vnl/vnl_math.h>
#include <vcl/vcl_iostream.h>
#include <stdlib.h>

// - The constructor calculates the roots.  This is the most efficient interface
// as all the result variables are initialized to the correct size.
// The polynomial is @{$ a[0] x^d + a[1] x^{d-1} + \cdots + a[d] = 0 $@}.
// Note that if the routine fails, not all roots will be found.  In this case,
// the "realroots" and "roots" functions will return fewer than n roots.
vnl_rpoly_roots::vnl_rpoly_roots(const vnl_vector<double>& a)
  : coeffs_(a), r_(coeffs_.size()-1), i_(coeffs_.size()-1)
{
  // fsm : if the coefficients are NaNs then rpoly_ gets stuck in an 
  // infinite loop. of course, the caller shouldn't pass in NaNs, but
  // it would be nice to get an error message instead of hanging.
  bool has_nans=false;
  for (unsigned i=0;i<a.size();i++)
    if (vnl_math::isnan(a(i)))
      has_nans=true;

  if (has_nans) {
    cerr << __FILE__ " these coefficients are invalid :" << endl;
    for (unsigned i=0;i<a.size();i++)
      cerr << i << ' ' << a(i) << endl;
    abort();
  }

  compute();
}

// - Complex vector of all roots.
vnl_vector<vnl_double_complex> vnl_rpoly_roots::roots() const
{
  vnl_vector<vnl_double_complex> ret(num_roots_found_);
  for(int i = 0; i < num_roots_found_; ++i)
    ret[i] = vnl_double_complex(r_[i], i_[i]);
  return ret;
}

// - Return real roots only.  Roots are real if the absolute value
// of their imaginary part is less than the optional argument TOL.
// TOL defaults to 1e-12 [untested]
vnl_vector<double> vnl_rpoly_roots::realroots(double tol) const
{
  int c = 0;
  for(int i = 0; i < num_roots_found_; ++i)
    if (vnl_math::abs(i_[i]) < tol)
      ++c;

  vnl_vector<double> ret(c);
  c = 0;
  {for(int i = 0; i < num_roots_found_; ++i)
    if (vnl_math::abs(i_[i]) < tol)
      ret[c++] = r_[i];}

  return ret;
}

// Declare the fortran routine that does the real work.
extern "C"
void rpoly_(const double* op, int* degree, double *zeror, double *zeroi, int *fail);

// -- Compute roots using Jenkins-Traub algorithm.
// Calls rpoly and interprets failure codes.
bool vnl_rpoly_roots::compute()
{
  int fail = 0;
  int n = coeffs_.size() - 1;

  rpoly_(coeffs_.data_block(), &n, r_.data_block(), i_.data_block(), &fail);

  if (!fail) {
    num_roots_found_ = n;
    return true;
  }
  
  num_roots_found_ = n;

  if (coeffs_[0] == 0.0)
    cerr << "vnl_rpoly_roots: Leading coefficient is zero.  Not allowed.\n";
  else
    cerr << "vnl_rpoly_roots: Calculation failed, only " << n << " roots found\n";

  return false;
}
