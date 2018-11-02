// This is bbas/bpgl/algo/bpgl_fm_compute_reg_ransac.cxx
#ifndef bpgl_fm_compute_reg_ransac_cxx_
#define bpgl_fm_compute_reg_ransac_cxx_

#include <iostream>
#include <cmath>
#include "bpgl_fm_compute_reg_ransac.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/algo/vgl_homg_operators_2d.h>
#include <rrel/rrel_ran_sam_search.h>
#include <rrel/rrel_muset_obj.h>


//------------------------------------------
bool
bpgl_fm_compute_reg_ransac::compute(
  const std::vector< vgl_point_2d<double> >& pr,
  const std::vector< vgl_point_2d<double> >& pl,
  bpgl_reg_fundamental_matrix<double>& fm )
{
  // Check that there is at least 1 point.
  if ( pr.size() < 1 || pl.size() < 1 ){
    std::cerr << "bpgl_fm_compute_ransac: Need at least 1 point pair.\n"
             << "Number in each set: " << pr.size() << ", " << pl.size() << std::endl;
    return false;
  }

  // Check that the correspondence lists are the same size.
  if ( pr.size() != pl.size() ){
    std::cerr << "bpgl_fm_compute_reg_ransac: Need correspondence lists of same size.\n";
    return false;
  }

  // The following block is hacked from similar code in rrel_homography2d_est.
  auto* estimator = new rrel_fm_reg_problem( pr, pl );
  estimator->verbose=true;
  auto* ransac = new rrel_muset_obj((int)std::floor(pr.size()*.75));
  estimator->set_prior_scale( 1.0 );
  auto* ransam = new rrel_ran_sam_search;
  ransam->set_trace_level(params_->trace_level);
  ransam->set_sampling_params( params_->max_outlier_frac,
                               params_->desired_prob_good,
                               params_->max_pops );
  bool ransac_succeeded = ransam->estimate( estimator, ransac );

  if ( ransac_succeeded )
  estimator->params_to_fm( ransam->params(), fm );

  // Get a list of the outliers.
  std::vector<double> residuals;
  estimator->compute_residuals( ransam->params(), residuals );

  outliers = std::vector<bool>();
  for ( unsigned i = 0; i < pr.size(); i++ ){
    if ( residuals[i] > params_->residual_thresh )
      outliers.push_back( true );
    else
      outliers.push_back( false );
  }

  delete ransac;
  delete ransam;
  delete estimator;

  return ransac_succeeded;
}


//------------------------------------------
bpgl_fm_compute_reg_ransac_params::bpgl_fm_compute_reg_ransac_params() :
  max_outlier_frac(0.5),
  desired_prob_good(0.99),
  max_pops(1),
  trace_level(0),
  residual_thresh(1)
{
}


//------------------------------------------
rrel_fm_reg_problem::rrel_fm_reg_problem(
  const std::vector< vgl_point_2d<double> >& pr,
  const std::vector< vgl_point_2d<double> >& pl ) :
  rrel_estimation_problem(2,1) // Really should be 1,1 but can't deal with div/0
{
  assert( pr.size() == pl.size() );

  for ( unsigned int i=0; i < pr.size(); i++ )
  {
    pr_.push_back( pr[i] );
    pl_.push_back( pl[i] );
  }
  verbose = false;
}


//------------------------------------------
bool
rrel_fm_reg_problem::fit_from_minimal_set(
  const std::vector<int>& point_indices,
  vnl_vector<double>& params ) const
{
  if ( verbose ) std::cerr << "rrel_fm_reg_problem::fit_from_minimal_set\n";
  assert( point_indices.size() == 1 );

  bpgl_reg_fundamental_matrix<double> fm( pr_[point_indices[0]], pl_[point_indices[0]] );

  // 7 point algorithm returns a list of possible fundamental matrices.  I'm not
  // sure which one to take, so i'm taking the first.
  fm_to_params( fm, params );
  if ( verbose ) std::cerr << "params: " << params << '\n';
  return true;
}


//------------------------------------------
void
rrel_fm_reg_problem::compute_residuals(
  const vnl_vector<double>& params,
  std::vector<double>& residuals ) const
{
  if ( verbose ) std::cerr << "rrel_fm_reg_problem::compute_residuals\n";

  bpgl_reg_fundamental_matrix<double> fm;
  params_to_fm(params, fm);

  if ( residuals.size() != pr_.size() )
    residuals.resize( pr_.size() );

  // The residual for each correspondence is the sum of the squared distances from
  // the points to their epipolar lines.
  for ( unsigned i = 0; i < pr_.size(); i++ ){
    vgl_homg_line_2d<double> lr =
      fm.r_epipolar_line( vgl_homg_point_2d<double>( pl_[i] ) );
    vgl_homg_line_2d<double> ll =
      fm.l_epipolar_line( vgl_homg_point_2d<double>( pr_[i] ) );
    residuals[i] = vgl_homg_operators_2d<double>::perp_dist_squared( lr,
                                                                     vgl_homg_point_2d<double>( pr_[i] ) )
                 + vgl_homg_operators_2d<double>::perp_dist_squared( ll,
                                                                     vgl_homg_point_2d<double>( pl_[i] ) );
  }
}


//-------------------------------------------
void
rrel_fm_reg_problem::fm_to_params(
  const bpgl_reg_fundamental_matrix<double>& fm,
  vnl_vector<double>& p ) const
{
  p.set_size(2);
  vnl_matrix_fixed<double,3,3> fm_vnl = fm.get_matrix();
  p(0) = fm_vnl(0,2);
  p(1) = fm_vnl(2,1);
}


//-------------------------------------------
void
rrel_fm_reg_problem::params_to_fm(
  const vnl_vector<double>& p,
  bpgl_reg_fundamental_matrix<double>& fm ) const
{
  fm.set_from_params( p(0), p(1) );
}


//--------------------------------------------
bool
rrel_fm_reg_problem::weighted_least_squares_fit(
  vnl_vector<double>& /*params*/,
  vnl_matrix<double>& /*norm_covar*/,
  const std::vector<double>* /*weights*/ ) const
{
  std::cerr << "rrel_fm_reg_problem::weighted_least_squares_fit was called, but is not implemented.\n";
  return false;
}


#endif // bpgl_fm_compute_reg_ransac_cxx_
