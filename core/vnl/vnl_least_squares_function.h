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
#ifndef vnl_least_squares_function_h_
#define vnl_least_squares_function_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : vnl_least_squares_function
//
// .SECTION Description
//    vnl_least_squares_function is an abstract base for functions to be minimized
//    by an optimizer.  To define your own function to be minimized, subclass
//    from vnl_least_squares_function, and implement the pure virtual f (and
//    optionally grad_f).
//
//    Whether or not f ought to be const is a problem.  Clients might well
//    want to cache some information during the call, and if they're compute
//    objects, will almost certainly be writing to members during the
//    computation.  For the moment it's non-const, but we'll see...
//
// .NAME        vnl_least_squares_function - Many-to-many function.
// .LIBRARY     vnl
// .HEADER	Numerics Package
// .INCLUDE     vnl/vnl_least_squares_function.h
// .FILE        vnl/vnl_least_squares_function.cxx
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 31 Aug 96
//
// .SECTION Modifications:
//     280697 AWF Changed return type of f from double to void, as it wasn't used, and
//                people were going to extra trouble to compute it.
//     20 Apr 1999 FSM
//            Added failure flag so that f() and grad() may signal failure to the caller.
//
//-----------------------------------------------------------------------------

#include <vcl/vcl_string.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>

class vnl_least_squares_function {
public:
  // Constants-----------------------------------------------------------------
  enum  UseGradient {
    no_gradient,
    use_gradient
  };
  bool failure;
public:
  // Constructors/Destructors--------------------------------------------------
// -- Construct vnl_least_squares_function, passing number of parameters
// (unknowns, domain dimension) and number of residuals (range dimension).
// The optional argument should be no_gradient if the gradf function has not
// been implemented.
  vnl_least_squares_function(int number_of_unknowns, int number_of_residuals, UseGradient = use_gradient);

// -- Virtual destructor.
  virtual ~vnl_least_squares_function();

  // Operations----------------------------------------------------------------
  void throw_failure(void); // the virtuals may call this to a signal failure.
  void clear_failure(void); // 

  // Computations--------------------------------------------------------------

// -- The main function.  Given the parameter vector x, compute the vector
// of residuals fx.  Fx has been sized appropriately before the call.
  virtual void f(const vnl_vector<double>& x, vnl_vector<double>& fx) = 0;

// -- Calculate the Jacobian, given the parameter vector x.
  virtual void gradf(const vnl_vector<double>& x, vnl_matrix<double>& jacobian);

// -- Called after each LM iteration to print debugging etc.
  virtual void trace(int iteration, const vnl_vector<double>& x, const vnl_vector<double>& fx);

// -- Compute the rms error at x by calling f and returning the norm of the residual
// vector.
  double rms(const vnl_vector<double>& x);
  
  // Data Access---------------------------------------------------------------

// -- Return the number of unknowns
  int get_number_of_unknowns() const { return p_; }

// -- Return the number of residuals.
  int get_number_of_residuals() const { return n_; }

// -- Return true if the derived class has indicated that gradf has been implemented
  bool has_gradient() const { return use_gradient_; }

  // Data Control--------------------------------------------------------------

protected:
  // Data Members--------------------------------------------------------------
  int p_;
  int n_;
  bool use_gradient_;
  vcl_string print_x_fmt_;
  vcl_string print_f_fmt_;

  void init(int number_of_unknowns, int number_of_residuals);
  
private:
  // Helpers-------------------------------------------------------------------
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vnl_least_squares_function.

