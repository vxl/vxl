//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation "vnl_nonlinear_minimizer.h"
#endif
//
// Class: vnl_nonlinear_minimizer
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 22 Aug 99
// Modifications:
//   990822 AWF Initial version.
//
//-----------------------------------------------------------------------------

#include "vnl_nonlinear_minimizer.h"

// -- Default ctor sets verbosity etc.
vnl_nonlinear_minimizer::vnl_nonlinear_minimizer()
{
  xtol = 1e-8;           // Termination tolerance on X (solution vector)
  maxfev = 2000; // Termination maximum number of iterations.
  ftol = xtol * 0.01;    // Termination tolerance on F (sum of squared residuals)
  gtol = 1e-5;           // Termination tolerance on Grad(F)' * F = 0
  epsfcn = xtol * 0.001; // Step length for FD Jacobian
  trace = false;
  verbose_ = false;
}


vnl_nonlinear_minimizer::~vnl_nonlinear_minimizer()
{
}

vnl_matrix<double> const& vnl_nonlinear_minimizer::get_covariance()
{
  static vnl_matrix<double> null;
  return null;
}
