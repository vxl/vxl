// This is rpl/rrel/rrel_irls.cxx
#include <iostream>
#include <vector>
#include "rrel_irls.h"
//:
// \file

#include <rrel/rrel_estimation_problem.h>
#include <rrel/rrel_wls_obj.h>
#include <rrel/rrel_util.h>

#include "vnl/vnl_math.h"
#include "vnl/vnl_vector.h"
#include "vnl/vnl_matrix.h"

#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include <cassert>

// -------------------------------------------------------------------------
rrel_irls::rrel_irls(int max_iterations)
    : max_iterations_(max_iterations), convergence_tol_(dflt_convergence_tol_),
      iterations_for_scale_est_(dflt_iterations_for_scale_) {
  assert( max_iterations > 0 );
}

// -------------------------------------------------------------------------
void
rrel_irls::set_est_scale( int iterations_for_scale_est,
                          bool use_weighted_scale )
{
  est_scale_during_ = true;
  use_weighted_scale_ = use_weighted_scale;
  iterations_for_scale_est_ = iterations_for_scale_est;
  if ( iterations_for_scale_est_ < 0 )
    std::cerr << "rrel_irls::est_scale_during WARNING last_scale_est_iter is\n"
             << "negative, so scale will not be estimated!\n";
}

// -------------------------------------------------------------------------
//: Set lower bound of scale estimate
void
rrel_irls::set_scale_lower_bound( double lower_scale )
{
  scale_lower_bound_ = lower_scale;
}

// -------------------------------------------------------------------------
void
rrel_irls::set_no_scale_est()
{
  est_scale_during_ = false;
}

// -------------------------------------------------------------------------
void
rrel_irls::initialize_scale( double scale )
{
  scale_ = scale;
  scale_initialized_ = true;
}

// -------------------------------------------------------------------------
double
rrel_irls::scale() const
{
  assert( scale_initialized_ );
  return scale_;
}


// -------------------------------------------------------------------------
void
rrel_irls::set_max_iterations( int max_iterations )
{
  max_iterations_ = max_iterations;
  assert( max_iterations_ > 0 );
}


// -------------------------------------------------------------------------
void
rrel_irls::set_convergence_test( double convergence_tol )
{
  test_converge_ = true;
  convergence_tol_ = convergence_tol;
  assert( convergence_tol_ > 0 );
}


// -------------------------------------------------------------------------
void
rrel_irls::set_no_convergence_test( )
{
  test_converge_ = false;
}


// -------------------------------------------------------------------------
void
rrel_irls::initialize_params( const vnl_vector<double>& init_params )
{
  params_ = init_params;
  params_initialized_ = true;
}


bool
rrel_irls::estimate( const rrel_estimation_problem* problem,
                     const rrel_wls_obj* obj )
{
  iteration_ = 0;
  obj_fcn_ = 1e256;
  unsigned int num_for_fit = problem->num_samples_to_instantiate();
  bool allow_convergence_test = true;
  std::vector<double> residuals( problem->num_samples() );
  std::vector<double> weights( problem->num_samples() );
  bool failed = false;

  //  Parameter initialization, if necessary
  if ( ! params_initialized_ )
  {
    if ( ! problem->weighted_least_squares_fit( params_, cofact_ ) )
      return false;
    allow_convergence_test = false;
    params_initialized_ = true;
  }


  //  Scale initialization, if necessary
  if ( obj->requires_prior_scale() && problem->scale_type() == rrel_estimation_problem::NONE ) {
    std::cerr << "irls::estimate: Objective function requires a prior scale, and the problem does not provide one.\n"
             << "                Aborting estimation.\n";
    return false;
  }
  else if ( problem->scale_type() == rrel_estimation_problem::NONE && ! scale_initialized_ ) {
    problem->compute_residuals( params_, residuals );
    scale_ = rrel_util_median_abs_dev_scale( residuals.begin(), residuals.end(), num_for_fit );
    allow_convergence_test = false;
    scale_initialized_ = true;
  }

  if ( trace_level_ >= 1 )
    std::cout << "Initial estimate: " << params_ << ", scale = " << scale_ <<  std::endl;

  assert( params_initialized_ && scale_initialized_ );
  if ( scale_ <= 1e-8 ) {
    unsigned int dof = problem-> param_dof();
    cofact_ = 1e-16 * vnl_matrix<double>(dof, dof, vnl_matrix_identity);
    scale_ = 0.0;
    converged_ = true;
    std::cerr << "rrel_irls::estimate: initial scale is zero - cannot estimate\n";
    // usually, This means that it already has an exact fitting.
    // Thus, no harm if return true
    return true;
  }


  //  Basic loop:
  //  1. Calculate residuals
  //  2. Test for convergence, if desired.
  //  3. Calculate weights
  //  4. Calculate scale
  //  5. Calculate new estimate
  //

  converged_ = false;
  while ( true ) {
    //  Step 1.  Residuals
    problem->compute_residuals( params_, residuals );
    if ( trace_level_ >= 2 ) trace_residuals( residuals );

    //  Step 2.  Convergence.  The allow_convergence_test parameter
    //  prevents use of the convergence test until after the
    //  iterations involving scale estimation are finished.
    if ( test_converge_ && allow_convergence_test &&
         has_converged( residuals, obj, problem, &params_ ) ) {
      converged_ = true;
      break;
    }
    ++ iteration_;
    if ( iteration_ > max_iterations_ ) break;
    if ( trace_level_ >= 1 ) std::cout << "\nIteration: " << iteration_ << '\n';

    //  Step 3. Weights
    problem->compute_weights( residuals, obj, scale_, weights );
    if ( trace_level_ >= 2 ) trace_weights( weights );

    //  Step 4.  Scale.  Note: the residuals are reordered and therefore useless after
    //  rrel_util_median_abs_dev_scale.
    if ( est_scale_during_ && iteration_ <= iterations_for_scale_est_ ) {
      allow_convergence_test = false;
      if ( trace_level_ >= 1 ) std::cout << "num samples for fit = " << num_for_fit << '\n';
      if ( use_weighted_scale_ ) {
        assert( residuals.size() == weights.size() );
        scale_ = rrel_util_weighted_scale( residuals.begin(), residuals.end(),
                                           weights.begin(), num_for_fit, (double*)nullptr );
      }
      else {
        scale_ = rrel_util_median_abs_dev_scale( residuals.begin(), residuals.end(), num_for_fit );
      }
      if ( trace_level_ >= 1 ) std::cout << "Scale estimated: " << scale_ << std::endl;
      if ( scale_ <= 1e-8 ) {  //  fit exact enough to yield 0 scale estimate
        unsigned int dof = problem-> param_dof();
        cofact_ = 1e-16 * vnl_matrix<double>(dof, dof, vnl_matrix_identity);
        scale_ = 0.0;
        converged_ = true;
        std::cerr << "rrel_irls::estimate:  scale has gone to 0.\n";
        break;
      }

      // check lower bound
      if ( scale_lower_bound_ > 0 && scale_ < scale_lower_bound_ )
        scale_ = scale_lower_bound_;
    }
    else
      allow_convergence_test = true;

    // Step 5.  Weighted least-squares
    // Test to see if the sum of the weights is less or equal to zero.
    double sum_wgt = 0;

    for (double weight : weights) {
      sum_wgt += weight;
    }

    if (sum_wgt <= 0)
    {
      failed = true;
      break;
    }

    if ( !problem->weighted_least_squares_fit( params_, cofact_, &weights ) ) {
      failed = true;
      break;
    }
    if ( trace_level_ >= 1 ) std::cout << "Fit: " << params_ << std::endl;
  }

  return !failed;
}


// -------------------------------------------------------------------------
const vnl_vector<double>&
rrel_irls::params() const
{
  assert( params_initialized_ );
  return params_;
}


// -------------------------------------------------------------------------
const vnl_matrix<double>&
rrel_irls::cofactor() const
{
  assert( params_initialized_ );
  return cofact_;
}


// -------------------------------------------------------------------------
int
rrel_irls::iterations_used() const
{
  return iteration_-1;
}


// -------------------------------------------------------------------------
bool
rrel_irls::has_converged( const std::vector<double>& residuals,
                          const rrel_wls_obj* obj,
                          const rrel_estimation_problem* problem,
                          vnl_vector<double>* params )
{
  prev_obj_fcn_ = obj_fcn_;
  switch ( problem->scale_type() )
  {
   case rrel_estimation_problem::NONE:
    obj_fcn_ = obj->fcn( residuals.begin(), residuals.end(), scale_, params );
    break;
   case rrel_estimation_problem::SINGLE:
    obj_fcn_ = obj->fcn( residuals.begin(), residuals.end(), problem->prior_scale(), params );
    break;
   case rrel_estimation_problem::MULTIPLE:
    obj_fcn_ = obj->fcn( residuals.begin(), residuals.end(), problem->prior_multiple_scales().begin(), params );
    break;
   default:
    assert(!"invalid scale_type");
  }

  if ( trace_level_ >= 1 )
    std::cout << "  prev obj fcn = " << prev_obj_fcn_
             << ",  new obj fcn = " << obj_fcn_ << std::endl;

  return vnl_math::abs( obj_fcn_ ) < convergence_tol_  ||
    vnl_math::abs(obj_fcn_ - prev_obj_fcn_) < convergence_tol_ * obj_fcn_;
}


// -------------------------------------------------------------------------
void
rrel_irls::trace_residuals( const std::vector<double>& residuals ) const
{
  std::cout << "Residuals:\n";
  for ( unsigned int i=0; i<residuals.size(); ++i )
    std::cout << "  " << i << ": " << residuals[i] << '\n';
}


// -------------------------------------------------------------------------
void
rrel_irls::trace_weights( const std::vector<double>& weights ) const
{
  std::cout << "Weights:\n";
  for ( unsigned int i=0; i<weights.size(); ++i )
    std::cout << "  " << i << ": " << weights[i] << '\n';
}


// -------------------------------------------------------------------------
void
rrel_irls::print_params() const
{
  std::cout << "  max_iterations_ = " << max_iterations_ << '\n'
           << "  test_converge_ = " << test_converge_ << '\n'
           << "  convergence_tol_ = " << convergence_tol_ << '\n'
           << "  est_scale_during_ = " << est_scale_during_ << '\n'
           << "  iterations_for_scale_est_ = " << iterations_for_scale_est_
           << std::endl;
}
