
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
#ifndef vnl_levenberg_marquardt_h_
#define vnl_levenberg_marquardt_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME        vnl_levenberg_marquardt - Levenberg Marquardt nonlinear least squares
// .LIBRARY     vnl
// .HEADER	vnl Package
// .INCLUDE     vnl/algo/vnl_levenberg_marquardt.h
// .FILE        vnl/algo/vnl_levenberg_marquardt.cxx
// .EXAMPLE     vnl/examples/rosenbrock_example.cxx
//
// .SECTION Description
//    vnl_levenberg_marquardt is an interface to the MINPACK routine lmdif,
//    and implements Levenberg Marquardt nonlinear fitting.  The function
//    to be minimized is passed as a vnl_least_squares_function object, which
//    may or may not wish to provide derivatives.  If derivatives are not
//    supplied, they are calculated by forward differencing, which costs
//    one function evaluation per dimension, but is perfectly accurate.
//    (See Hartley in ``Applications of Invariance in Computer Vision''
//    for example).
//
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 31 Aug 96
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include <vcl/vcl_iosfwd.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_nonlinear_minimizer.h>

class vnl_least_squares_function;

//: Levenberg Marquardt nonlinear least squares

class vnl_levenberg_marquardt : public vnl_nonlinear_minimizer {
public:
  // Constructors/Destructors--------------------------------------------------

// -- Initialize with the function object that is to be minimized.
  vnl_levenberg_marquardt(vnl_least_squares_function& f) { init(&f); }

// -- Initialize as above, and then run minimization.
  vnl_levenberg_marquardt(vnl_least_squares_function& f, vnl_vector<double>& x) {
    init(&f);
    minimize(x);
  }

// -- Destructor.
  ~vnl_levenberg_marquardt();

  // Operations----------------------------------------------------------------
  // Computations--------------------------------------------------------------

// -- Minimize the function supplied in the constructor until convergence
// or failure.  On return, x is such that f(x) is the lowest value achieved.
// Returns true for convergence, false for failure.
  bool minimize(vnl_vector<double>& x);
  bool minimize_using_gradient(vnl_vector<double>& x);

  // Data Access---------------------------------------------------------------

  // Coping with failure-------------------------------------------------------

// -- Provide an ASCII diagnosis of the last minimization on ostream.
  void diagnose_outcome(ostream& = cerr) const;

// -- Return J'*J computed at last minimum.
  vnl_matrix<double> const& get_JtJ();

protected:

  vnl_least_squares_function* f_;
  vnl_matrix<double>* fdjac_; // Computed during lmdif/lmder
  vnl_vector<int>*    ipvt_;  // Also computed, both needed to get J'*J at end.

  vnl_matrix<double>* covariance_;
  bool set_covariance_; // Set if covariance_ holds J'*J

  void init(vnl_least_squares_function* f);

  // Communication with callback
  friend class vnl_levenberg_marquardt_Activate;
  static int lmdif_lsqfun(int* m, int* n, const double* x, double* fx, int* iflag);
  static int lmder_lsqfun(int* m, int* n, const double* x, double* fx, double* fJ, int&, int* iflag);
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vnl_levenberg_marquardt.

