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
#pragma implementation "vnl_real_polynomial.h"
#endif
//
// Class: vnl_real_polynomial
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 23 Aug 96
// Modifications:
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_complex.h>
#include "vnl_real_polynomial.h"

// This is replacing a member template...
template <class T> 
T vnl_real_polynomial_evaluate(const double *a, int n, const T& x)
{
  --n;
  T acc = a[n];
  T xn = x;
  
  do {
    acc += a[--n] * xn;
    xn *= x;
  } while (n);
  
  return acc;  
}

#ifdef WIN32
#define SELECT(T) <T >
#else
#define SELECT(T) 
#endif

// Instantiate templates before use
template double         vnl_real_polynomial_evaluate SELECT(double        )(const double*, int, const double        &);
template vnl_double_complex vnl_real_polynomial_evaluate SELECT(vnl_double_complex)(const double*, int, const vnl_double_complex&);

// -- Evaluate polynomial at value x
double vnl_real_polynomial::evaluate(double x) const
{
  return vnl_real_polynomial_evaluate SELECT(double)(coeffs_.data_block(), coeffs_.size(), x);
}

// -- Evaluate polynomial at complex value x
vnl_double_complex vnl_real_polynomial::evaluate(const vnl_double_complex& x) const
{
  return vnl_real_polynomial_evaluate SELECT(vnl_double_complex)
     (coeffs_.data_block(), coeffs_.size(), x);
}

// -- Evaluate derivative at value x. Not implemented.
double vnl_real_polynomial::devaluate(double /*x*/) const
{
  return HUGE_VAL;
}

// -- Evaluate derivative at complex value x. Not implemented.
vnl_double_complex vnl_real_polynomial::devaluate(const vnl_double_complex& /*x*/) const
{
  return HUGE_VAL;
}
