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
#ifndef vnl_rnpoly_solve_h_
#define vnl_rnpoly_solve_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME	vnl_rnpoly_solve - Solves for roots of system of real polynomials
// .LIBRARY	vnl/algo
// .HEADER	Numerics package
// .INCLUDE	vnl/algo/vnl_rnpoly_solve.h
// .FILE	vnl/algo/vnl_rnpoly_solve.cxx
//
// .SECTION Description
//    Calculates all the roots of a system of N polynomials in N variables
//    through continuation.
//    Adapted from the  PARALLEL CONTINUATION algorithm , written by Darrell
//    Stam, 1991, and further improved by  Kriegman and Ponce, 1992.
//
// .SECTION Author
//    Marc Pollefeys, ESAT-VISICS, K.U.Leuven, 12-08-97
//
// .SECTION Modifications:
//    Peter Vanroose, 20 Oct 1999: implementation simplified through "cmplx"
//                                 class for doing complex arithmetic.
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_vector.h>
#include <vnl/vnl_real_npolynomial.h>
#include <vcl/vcl_vector.h>

static const unsigned int M = 11;   // Maximum dimension of problem
static const unsigned int T = 2500; // Maximum number of terms in a polynomial

//: Solves for roots of system of real polynomials

class vnl_rnpoly_solve {
 
public:
  // Constructor---------------------------------------------------------------

  // -- The constructor already does all the calculations
  inline vnl_rnpoly_solve(vcl_vector<vnl_real_npolynomial*> const& ps) : ps_(ps) {compute();}

  // Operations----------------------------------------------------------------

// -- Array of real parts of roots
  inline vcl_vector<vnl_vector<double>*> real() { return r_; }

// -- Array of imaginary parts of roots
  inline vcl_vector<vnl_vector<double>*> imag() { return i_; }
  
// -- Return real roots only.  Roots are real if the absolute value
// of their imaginary part is less than the optional argument tol,
// which defaults to 1e-12 [untested]
  vcl_vector<vnl_vector<double>*> realroots(double tol = 1e-12);

  // Computations--------------------------------------------------------------

private:
  // -- Compute roots using continuation algorithm.
  bool compute();
    
  int Read_Input(int ideg[M], int terms[M],
		 int polyn[M][T][M], double coeff[M][T]);

  // Data Members--------------------------------------------------------------
  vcl_vector<vnl_real_npolynomial*> ps_;   // the input
  vcl_vector<vnl_vector<double>*> r_; // the output (real part)
  vcl_vector<vnl_vector<double>*> i_; // the output (imaginary part)
};

#endif // vnl_rnpoly_solve_h_
