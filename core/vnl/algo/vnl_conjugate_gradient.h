//-*- c++ -*-------------------------------------------------------------------
#ifndef vnl_conjugate_gradient_h_
#define vnl_conjugate_gradient_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME	vnl_conjugate_gradient - real function minimization
// .LIBRARY	vnl-algo
// .HEADER	vxl Package
// .INCLUDE	vnl/algo/vnl_conjugate_gradient.h
// .FILE	vnl_conjugate_gradient.cxx
//
// .SECTION Author
//     Geoffrey Cross, Oxford RRG, 15 Feb 99
//
//-----------------------------------------------------------------------------

#include <vcl/vcl_iostream.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>

class vnl_least_squares_function;

class vnl_conjugate_gradient {
public:
  // Constructors/Destructors--------------------------------------------------
  
  // -- Initialize with the function object that is to be minimized.
  vnl_conjugate_gradient(vnl_least_squares_function& f) { init( f); }

  // -- Initialize as above, and then run minimization.
  vnl_conjugate_gradient(vnl_least_squares_function& f, vnl_vector<double>& x) {
    init(f);
    minimize(x);
  }

  // -- Initialize all variables
  void init( vnl_least_squares_function &f);

  // -- Destructor.
  ~vnl_conjugate_gradient();

  // Operations----------------------------------------------------------------

  void diagnose_outcome( ostream& = cout) const;

  // Computations--------------------------------------------------------------

  // -- Minimize the function supplied in the constructor until convergence
  // or failure.  On return, x is such that f(x) is the lowest value achieved.
  // Returns true for convergence, false for failure.
  bool minimize(vnl_vector<double>& x);

  // Data Access---------------------------------------------------------------

  // -- Step size used to approximate the gradient
  void set_gradient_step_size( const double s) { gradstep_= s; }

  // -- Set maximum number of iterations
  void set_max_iterations( const int v) { max_number_of_iterations= v; }

  // -- Set tolerance on gradient for minimization
  void set_gradient_tolerance( const double t) { gradtolerance_= t; }

  // Data Control--------------------------------------------------------------

protected:
  // Data Members--------------------------------------------------------------

  vnl_least_squares_function *f_;

  double gradstep_;
  int max_number_of_iterations;
  double gradtolerance_;

  int number_of_iterations;
  double startresidue;
  double endresidue;
  int number_of_evaluations;

  // Helpers-------------------------------------------------------------------

  friend class vnl_conjugate_gradient_Activate;
#ifdef VCL_SUNPRO_CC
public:
#endif
  static double valuecomputer_( double *x);
  static int gradientcomputer_( double *g, double *x);
  static int valueandgradientcomputer_( double *v, double *g, double *x);
  static int preconditioner_( double *out, double *in);
#ifdef VCL_SUNPRO_CC
protected:
#endif
//   void approximate_gradient( const vnl_vector<double> &x, vnl_vector<double> &g, const double step);
//   void approximate_hessian( const vnl_vector<double> &x, vnl_matrix<double> &h, const double step);
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vnl_conjugate_gradient.

