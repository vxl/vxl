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
#ifndef vnl_rpoly_roots_h_
#define vnl_rpoly_roots_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME        vnl_rpoly_roots - Roots of real polynomial.
// .LIBRARY     vnl/algo
// .HEADER	Numerics package
// .INCLUDE     vnl/algo/vnl_rpoly_roots.h
// .FILE        vnl/algo/vnl_rpoly_roots.cxx
//
// .SECTION Description
//    Find the roots of a real polynomial.  Uses algorithm 493 from
//    ACM Trans. Math. Software - the Jenkins-Traub algorithm, described
//    by Numerical Recipes under "Other sure-fire techniques" as
//    "practically a standard in black-box polynomial rootfinders".
//    (See M.A. Jenkins, ACM TOMS 1 (1975) pp. 178-189.).
//
//    This class is not very const-correct as it is intended as a compute object
//    rather than a data object.
//
// .SECTION Author
//    Andrew W. Fitzgibbon, Oxford RRG, 06 Aug 96
//
// .SECTION Modifications:
//    23 may 97, Peter Vanroose - "NO_COMPLEX" option added (until "complex" type is standardised)
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_vector.h>

class vnl_real_polynomial;

//: Roots of real polynomial.
class vnl_rpoly_roots {
public:
  // Constructors/Destructors--------------------------------------------------

// -- The constructor calculates the roots.  This is the most efficient interface
// as all the result variables are initialized to the correct size.
// @{The polynomial is $ a[0] x^d + a[1] x^{d-1} + \cdots + a[d] = 0 $.@}
  vnl_rpoly_roots(const vnl_vector<double>& a);

// -- Calculate roots of RealPolynomial.
  vnl_rpoly_roots(const vnl_real_polynomial& poly);

  // Operations----------------------------------------------------------------

#define VNL_USED_COMPLEX
#ifdef VNL_COMPLEX_AVAILABLE

// -- Return i'th complex root
  vnl_double_complex operator [] (int i) const { return vnl_double_complex(r_[i], i_[i]); }

// -- Complex vector of all roots.
  vnl_vector<vnl_double_complex> roots() const;
#endif

// -- Real part of root I.
  const double& real(int i) const { return r_[i]; }

// -- Imaginary part of root I.
  const double& imag(int i) const { return i_[i]; }

// -- Vector of real parts of roots
  vnl_vector<double>& real() { return r_; }

// -- Vector of imaginary parts of roots
  vnl_vector<double>& imag() { return i_; }
  
// -- Return real roots only.  Roots are real if the absolute value
// of their imaginary part is less than the optional argument TOL.
// TOL defaults to 1e-12 [untested]
  vnl_vector<double> realroots(double tol = 1e-12) const;

  // Computations--------------------------------------------------------------

  // -- Compute roots using Jenkins-Traub algorithm.
  bool compute();
  
  // -- Compute roots using QR decomposition of companion matrix. [unimplemented]
  bool compute_qr();
  
  // -- Compute roots using Laguerre algorithm. [unimplemented]
  bool compute_laguerre();
  
  // Data Access---------------------------------------------------------------

  // Data Control--------------------------------------------------------------

protected:
  // Data Members--------------------------------------------------------------
  vnl_vector<double> coeffs_;

  vnl_vector<double> r_;
  vnl_vector<double> i_;
  
  int num_roots_found_;
  
private:
  // Helpers-------------------------------------------------------------------
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vnl_rpoly_roots.
