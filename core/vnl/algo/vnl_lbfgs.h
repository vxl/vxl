#ifndef vnl_lbfgs_h_
#define vnl_lbfgs_h_
#ifdef __GNUC__
#pragma interface
#endif

//:
// \file
// \brief Limited memory Broyden Fletcher Goldfarb Shannon minimization
// \author Andrew W. Fitzgibbon, Oxford RRG
// \date   22 Aug 99
//
// \verbatim
// Modifications
//  990822 AWF Initial version.
//  dac (Manchester) 28/03/2001: tidied up documentation
// \endverbatim
//

#include <vnl/vnl_cost_function.h>
#include <vnl/vnl_nonlinear_minimizer.h>

//: Limited memory Broyden Fletcher Goldfarb Shannon minimization
// Considered to be the best optimisation algorithm for functions 
// which are well behaved (i.e. locally smooth
// without too many local minima,) and have 1st derivatives available. 
//
// This limited-memory rank-2 quasi-newton optimisation method
// maintains an estimate of (the inverse of) the Hessian matrix of f(x).
// Unlike Newton's method, it doesn't need 2nd derivatives of f(x),
// has superlinear rather than quadratic convergence and is
// better behaved away from minima. 2 ranks of this matrix are updated at each
// step. In order to limit memory use, this limited memory version of BFGS,
// only maintains a certain number of ranks of the inverse hessian estimate.

class vnl_lbfgs : public vnl_nonlinear_minimizer {
public:
  vnl_lbfgs();
  vnl_lbfgs(vnl_cost_function& f);

  bool minimize(vnl_vector<double>& x);

  //: Number of direction vectors to keep.
  // Effectively the number of ranks of the inverse hessian estimate that
  // are kept. Sensible values are start from about 5.
  // It rarely worth using a value much larger than the dimensionality of
  // the parameter x.
  //
  // Memory requirements will be roughly Const+sizeof(element)*dim(X)*memory;
  int memory;

  //: Accuracy of line search.
  // If function evaluations are cheap wrt the actual minimization steps,
  // change to 0.1, from default of 0.9;
  double line_search_accuracy;

  //: Default step length in line search.
  // If, on tracing, the STP is always 1, then you could try setting this to a
  // higher value to see how far along the gradient the minimum typically is.
  // Then set this to a number just below that to get maximally far with the
  // single evaluation.
  double default_step_length;

private:
  void init_parameters();
  vnl_cost_function* f_;
  //  vnl_lbfgs() {} // default constructor makes no sense
  // does too.  Can set values for parameters.
};

#endif // vnl_lbfgs_h_
