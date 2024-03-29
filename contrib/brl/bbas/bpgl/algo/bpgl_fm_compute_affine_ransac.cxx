// This is bbas/bpgl/algo/bpgl_fm_compute_affine_ransac.cxx
#ifndef bpgl_fm_compute_affine_ransac_cxx_
#define bpgl_fm_compute_affine_ransac_cxx_

#include <iostream>
#include <cmath>
#include "bpgl_fm_compute_affine_ransac.h"

#include <cassert>
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include "vgl/vgl_point_2d.h"
#include "vgl/vgl_homg_point_2d.h"
#include "vgl/vgl_homg_line_2d.h"
#include <vgl/algo/vgl_homg_operators_2d.h>
#include <vnl/algo/vnl_svd.h>
#include <vrel/vrel_ran_sam_search.h>
#include <vrel/vrel_muset_obj.h>


//------------------------------------------
bool
bpgl_fm_compute_affine_ransac::compute(
  const std::vector< vgl_point_2d<double> >& pr,
  const std::vector< vgl_point_2d<double> >& pl,
  vpgl_affine_fundamental_matrix<double>& fm )
{
  // Check that there are at least 5 points.
  if ( pr.size() < 4 || pl.size() < 4 ){
    std::cerr << "bpgl_fm_compute_ransac: Need at least 4 point pairs.\n"
             << "Number in each set: " << pr.size() << ", " << pl.size() << std::endl;
    return false;
  }

  // Check that the correspondence lists are the same size.
  if ( pr.size() != pl.size() ){
    std::cerr << "bpgl_fm_compute_affine_ransac: Need correspondence lists of same size.\n";
    return false;
  }

  // The following block is hacked from similar code in vrel_homography2d_est.
  auto* estimator = new vrel_fm_affine_problem( pr, pl );
  estimator->verbose = false;
  auto* ransac = new vrel_muset_obj((int)std::floor(pr.size()*.75));
  estimator->set_prior_scale( 1.0 );
  auto* ransam = new vrel_ran_sam_search;
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
bpgl_fm_compute_affine_ransac_params::bpgl_fm_compute_affine_ransac_params() :
  max_outlier_frac(0.5),
  desired_prob_good(0.99),
  max_pops(1),
  trace_level(0),
  residual_thresh(1)
{
}


//------------------------------------------
vrel_fm_affine_problem::vrel_fm_affine_problem(
  const std::vector< vgl_point_2d<double> >& pr,
  const std::vector< vgl_point_2d<double> >& pl ) :
  vrel_estimation_problem(4,4)
{
  assert( pr.size() == pl.size() );

  for ( unsigned int i=0; i < pr.size(); i++ )
  {
    pr_.push_back( pr[i] );
    pl_.push_back( pl[i] );
  }
}


//------------------------------------------
bool
vrel_fm_affine_problem::fit_from_minimal_set(
  const std::vector<int>& point_indices,
  vnl_vector<double>& params ) const
{
  if ( verbose ) std::cerr << "vrel_fm_affine_problem::fit_from_minimal_set\n";
  assert( point_indices.size() == 4 );

  vnl_matrix<double> S(4,5);
  for ( int i = 0; i < 4; i++ ){
    S(i,0) = pl_[point_indices[i]].x();
    S(i,1) = pl_[point_indices[i]].y();
    S(i,2) = pr_[point_indices[i]].x();
    S(i,3) = pr_[point_indices[i]].y();
    S(i,4) = 1.0;
  }
  vnl_svd<double> svdS( S );
  params = svdS.nullvector();

  // normalize parameters
  double norm = 1.0 / std::sqrt( params[0]*params[0] + params[1]*params[1] );
  params *= norm;

  if ( verbose ) std::cerr << "params: " << params << '\n';
  return true;
}


//------------------------------------------
void
vrel_fm_affine_problem::compute_residuals(
  const vnl_vector<double>& params,
  std::vector<double>& residuals ) const
{
  if ( verbose ) std::cerr << "vrel_fm_affine_problem::compute_residuals: ";

  vpgl_affine_fundamental_matrix<double> fm;
  params_to_fm(params, fm);

  if ( residuals.size() != pr_.size() )
    residuals.resize( pr_.size() );
  double ressum = 0;

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
    ressum+=residuals[i];
  }
  if ( verbose ) std::cerr << ressum << '\n';
}


//-------------------------------------------
void
vrel_fm_affine_problem::fm_to_params(
  const vpgl_affine_fundamental_matrix<double>& fm,
  vnl_vector<double>& p ) const
{
  vnl_matrix_fixed<double,3,3> F = fm.get_matrix();
  p.set_size(5);
  p(0) = F(0,2);
  p(1) = F(1,2);
  p(2) = F(2,0);
  p(3) = F(2,1);
  p(4) = F(2,2);
}


//-------------------------------------------
void
vrel_fm_affine_problem::params_to_fm(
  const vnl_vector<double>& p,
  vpgl_affine_fundamental_matrix<double>& fm ) const
{
  vnl_matrix_fixed<double,3,3> F( (double)0.0 );
  F(0,2) = p(0);
  F(1,2) = p(1);
  F(2,0) = p(2);
  F(2,1) = p(3);
  F(2,2) = p(4);
  fm.set_matrix(F);
}


//--------------------------------------------
bool
vrel_fm_affine_problem::weighted_least_squares_fit(
  vnl_vector<double>& /*params*/,
  vnl_matrix<double>& /*norm_covar*/,
  const std::vector<double>* /*weights*/ ) const
{
  std::cerr << "vrel_fm_affine_problem::weighted_least_squares_fit was called, but is not implemented.\n";
  return false;
}


#endif // bpgl_fm_compute_affine_ransac_cxx_
