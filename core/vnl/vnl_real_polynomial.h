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
#ifndef vnl_real_polynomial_h_
#define vnl_real_polynomial_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME        vnl_real_polynomial - Evaluation of real polynomials.
// .FILE        vnl/vnl_real_polynomial.cxx
//
// .SECTION Description
// @{
//    vnl_real_polynomial represents a univariate polynomial with real
//    coefficients, stored as a vector of doubles.  This allows
//    evaluation of the polynomial $p(x)$ at given values of $x$,
//    or of its derivative $p'(x)$.
//
//    Roots may be extracted using the roots() method.
// @}
//
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 06 Aug 96
//
// .SECTION Modifications:
//    23 may 97, Peter Vanroose - "NO_COMPLEX" option added (until "complex" type is standardised)
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_vector.h>

//:Evaluation of real polynomials at real and complex points.

class vnl_real_polynomial {
public:
  // -- Initialize polynomial.
  // The polynomial is @{$ a[0] x^d + a[1] x^{d-1} + \cdots + a[d] = 0 $@}.
  vnl_real_polynomial(vnl_vector<double> const & a): coeffs_(a) {}
  
  // -- Initialize polynomial from C vector.  The parameter len is the number
  // of coefficients, one greater than the degree.
  vnl_real_polynomial(double const * a, int len): coeffs_(a, len) {}
  
  // -- Initialize polynomial of a given degree.
  vnl_real_polynomial(int d): coeffs_(d+1) {}

  
  // -- Evaluate polynomial at value x
  double evaluate(double x) const;

  // -- Evaluate derivative at value x
private: // not implemented
  double devaluate(double x) const;
public:

#define VNL_USED_COMPLEX
#ifdef VNL_COMPLEX_AVAILABLE
  // -- Evaluate polynomial at complex value x
  vnl_double_complex evaluate(const vnl_double_complex& x) const;

  // -- Evaluate derivative at complex value x
  vnl_double_complex devaluate(const vnl_double_complex& x) const;
#endif

  // Data Access---------------------------------------------------------------
  
  // -- Return the degree (highest power of x) of the polynomial.
  int     degree() const { return coeffs_.size() - 1; }

  // -- Access to the polynomial coefficients
  double& operator [] (int i)       { return coeffs_[i]; }
  double  operator [] (int i) const { return coeffs_[i]; }

  // -- Return the vector of coefficients
  const vnl_vector<double>& coefficients() const { return coeffs_; }
        vnl_vector<double>& coefficients()       { return coeffs_; }

protected:
  vnl_vector<double> coeffs_;
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vnl_real_polynomial.
