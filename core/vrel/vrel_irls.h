#ifndef vrel_irls_h_
#define vrel_irls_h_
//:
// \file
// \author Chuck Stewart (stewart@cs.rpi.edu)
// \date March 2001
// \brief Iteratively-reweighted least-squares minimization of an M-estimator
//
// \verbatim
//   2001-10-22 Amitha Perera. Reorganised and added comments.
// \endverbatim

#include <iostream>
#include <vector>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vrel/vrel_export.h>

class vrel_estimation_problem;
class vrel_wls_obj;

//: Iteratively-reweighted least-squares minimization of an M-estimator.
//
// IRLS is a search technique for solving M-estimation problems. The
// technique is to compute weights from a given parameter estimate,
// and use those weights to compute a new estimate via weighted least
// squares.
//
// Several options allow variation in the behavior of estimation:
//
// (1) Scale may be fixed through a parameter setting function or it
// may be estimated in the first few iterations of IRLS.  A parameter
// called iterations_for_scale_est_ determines the number of iterations
// during which scale is to be estimated.   Scale estimation is either
// weight-based or median-based.
//
// (2) The maximum number of iterations may be set, either in the
// constructor or through parameter setting member functions.
//
// (3) The convergence test, which is based on the objective function,
// may or may not be run.  Not running it makes each iteration faster,
// but may result in more iterations than necessary.  The convergence
// test is not applied until the scale is no longer allowed to
// change.  (The alternative, which was not implemented, is to include
// a ln(sigma) term to the objective function.)
//
// (4) The parameters, the scale, both or neither may be initialized.
//
// See also vrel_estimation_problem and vrel_wls_obj.

class vrel_irls
{
  //  default parameters
  static constexpr double dflt_convergence_tol_ = 1e-4;
  static constexpr int dflt_max_iterations_ = 25;
  static constexpr int dflt_iterations_for_scale_ = 1;

public:
  //: Constructor.
  vrel_irls(int max_iterations = dflt_max_iterations_);

  //: Destructor.
  ~vrel_irls() = default;

  // -----------------------------------------------------------
  // Functions related to setting / estimation / access to scale
  // -----------------------------------------------------------

  //: Set scale estimation and parameters.
  void
  set_est_scale(int iterations_for_scale = dflt_iterations_for_scale_, bool use_weighted_scale = false);

  //: Set lower bound of scale estimate
  void
  set_scale_lower_bound(double lower_scale);

  //: Set for no scale estimation.
  //  Scale must be initialized or supplied by the problem.
  void
  set_no_scale_est();

  //: Initialize the scale value.
  void
  initialize_scale(double scale);

  //: Indicate that scale has not been initialized.
  void
  reset_scale()
  {
    scale_initialized_ = false;
  }

  //: Get the scale estimate (or the fixed scale value).
  double
  scale() const;

  // -----------------------------------------------------------
  // Functions related to the number of iterations and convergence
  // -----------------------------------------------------------

  //: Set the maximum number of iterations.
  void
  set_max_iterations(int max_iterations = dflt_max_iterations_);

  //: Indicate that a convergence test is to be used.
  void
  set_convergence_test(double convergence_tol = dflt_convergence_tol_);

  //: Indicate that no convergence test is to be used.
  void
  set_no_convergence_test();


  // ------------------------------------------------------------
  //  Parameter initialization and re-initialization
  // ------------------------------------------------------------

  //: Initialize the parameter estimate.
  void
  initialize_params(const vnl_vector<double> & init_params);

  //: Reset the parameters.
  void
  reset_params()
  {
    params_initialized_ = false;
  }

  void
  set_trace_level(int level)
  {
    trace_level_ = level;
  }

  // ------------------------------------------------------------
  //  Main estimation functions
  // ------------------------------------------------------------

  //: Estimate the parameters.
  //  The initial step is to initialize the parameter estimate, if
  //  necessary, and then the scale estimate, if necessary.  Then the
  //  following basic loop is applied:
  //
  //  1. Calculate residuals
  //  2. Test for convergence, if desired.
  //  3. Calculate weights
  //  4. Calculate scale
  //  5. Calculate new estimate
  //
  //  The loop can end in one of three ways: (a) convergence, (b) the
  //  maximum number of iterations is reached, (c) the weighted
  //  least-squares estimate fails.
  bool
  estimate(const vrel_estimation_problem * problem, const vrel_wls_obj * m_estimator);

  // ------------------------------------------------------------
  //  Get information about results...
  // ------------------------------------------------------------

  //: Get the parameter estimate.
  const vnl_vector<double> &
  params() const;

  //: Get the cofactor matrix (the covariance matrix /  scale^2).
  const vnl_matrix<double> &
  cofactor() const;

  //: Determine if the estimation converged.
  bool
  converged() const
  {
    return test_converge_ && converged_;
  }

  //: The number of iterations that were used.
  int
  iterations_used() const;

  //: \brief Print the residuals.  Used for debugging.
  void
  trace_residuals(const std::vector<double> & residuals) const;

  //: \brief Print the IRLS weights.  Used for debugging.
  void
  trace_weights(const std::vector<double> & weights) const;

  //: \brief Print the set of parameters.
  void
  print_params() const;

private:
  bool
  has_converged(const std::vector<double> & residuals,
                const vrel_wls_obj * m_estimator,
                const vrel_estimation_problem * problem,
                vnl_vector<double> * params);

protected:
  //  Parameters
  int max_iterations_;
  bool test_converge_{ true };
  double convergence_tol_;
  bool est_scale_during_{ true };
  bool use_weighted_scale_{ true };
  int iterations_for_scale_est_;
  double scale_lower_bound_{ -1.0 };
  int trace_level_{ 0 };

  //  Variables of estimation.
  vnl_vector<double> params_;
  vnl_matrix<double> cofact_;
  bool params_initialized_{ false };
  double scale_;
  bool scale_initialized_{ false };
  double obj_fcn_{ 1e256 }, prev_obj_fcn_{ 1e256 };
  bool converged_{ false };
  int iteration_{ 0 };
};

#endif // vrel_irls_h_
