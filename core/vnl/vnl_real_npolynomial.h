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
#ifndef vnl_real_npolynomial_h_
#define vnl_real_npolynomial_h_
#ifdef __GNUC__
#pragma interface
#endif

//
// .NAME        vnl_real_npolynomial - real polynomial in N variables.
// .LIBRARY     vnl
// .HEADER      Numerics package
// .INCLUDE     vnl/vnl_real_npolynomial.h
// .FILE        vnl/vnl_real_npolynomial.cxx
//
// .SECTION Description
//    vnl_real_npolynomial represents a polynomial in multiple variables.
//    Used by vnl_rnpoly_solve which solves systems of polynomial equations.
//    Representation:  an N-omial (N terms) is represented by (1) a vcl_vector
//    with the N coefficients (vnl_vector<double>), and (2) a matrix with
//    N rows, the i-th row representing the exponents of term i, as follows:
//    (vnl_matrix<int>) column k contains the (integer) exponent of variable
//    k.  Example: the polynomial A*X^3 + B*X*Y + C*Y^2 + D*X*Y^2 is
//    represented by the coefficients vcl_vector [A B C D] and the exponents
//    matrix
//    [3 0]
//    [1 1]
//    [0 2]
//    [1 2].
//
// .SECTION Author
//    Marc Pollefeys, ESAT-VISICS, K.U.Leuven, 12-08-97
//
// .SECTION Modifications:
//    Peter Vanroose 10 Oct 1999 - added simplify();
//                                 determine nterms_ nvar_ ideg_ automatically
//    Peter Vanroose 20 Oct 1999 - Added operator+(), - * and ostream <<
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>

//: real polynomial in N variables.

class vnl_real_npolynomial {
  friend class vnl_rnpoly_solve;
public:
// Constructor-----------------------------------------------------------------
  vnl_real_npolynomial() { } // don't use this. only here for the STL vcl_vector class.
  vnl_real_npolynomial(const vnl_vector<double>& c, const vnl_matrix<int>& p);

  // Computations--------------------------------------------------------------
  
  double eval(const vnl_vector<double>& x);
  int degree();
  vnl_real_npolynomial operator-() const; // unary minus
  vnl_real_npolynomial operator+(vnl_real_npolynomial const& ) const;
  vnl_real_npolynomial operator-(vnl_real_npolynomial const& ) const;
  vnl_real_npolynomial operator*(vnl_real_npolynomial const& ) const;
  vnl_real_npolynomial operator+(double ) const;
  vnl_real_npolynomial operator-(double P) const { return operator+(-P); }
  vnl_real_npolynomial operator*(double ) const;
  vnl_real_npolynomial& operator*=(double P) { coeffs_ *= P; return *this; }
  vnl_real_npolynomial operator/(double P) const { return operator*(1.0/P); }
  vnl_real_npolynomial& operator/=(double P) { return operator*=(1.0/P); }
  friend ostream& operator<<(ostream& , vnl_real_npolynomial const& );

private:
  void simplify();
  double eval(const vnl_matrix<double>& xn); 

  // Data Members--------------------------------------------------------------
  vnl_vector<double> coeffs_; // coefficients
  vnl_matrix<int>    polyn_;  // degrees of every term for every variable
  int                nvar_;   // number of variables = # columns of polyn_
  int                nterms_; // number of terms of polynomial
  int                ideg_;   // max. degree of polynomial
};

#endif // vnl_real_npolynomial_h_
