// This is gel/mrc/vpgl/algo/vpgl_fm_compute_ransac.cxx
#ifndef vpgl_fm_compute_ransac_cxx_
#define vpgl_fm_compute_ransac_cxx_

#include "vpgl_fm_compute_ransac.h"
#include "vpgl_fm_compute_8_point.h"

#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h>
#include <vgl/algo/vgl_homg_operators_2d.h>
#include <vnl/algo/vnl_svd.h>
#include <rrel/rrel_ran_sam_search.h>
#include <rrel/rrel_muset_obj.h>


//------------------------------------------
bool
vpgl_fm_compute_ransac::compute(
  const vcl_vector< vgl_point_2d<double> >& pr,
  const vcl_vector< vgl_point_2d<double> >& pl,
  vpgl_fundamental_matrix<double>& fm )
{
  // Check that there are at least 8 points.
  if ( pr.size() < 8 || pl.size() < 8 ){
    vcl_cerr << "vpgl_fm_compute_ransac: Need at least 8 point pairs.\n"
             << "Number in each set: " << pr.size() << ", " << pl.size() << vcl_endl;
    return false;
  }

  // Check that the correspondence lists are the same size.
  if ( pr.size() != pl.size() ){
    vcl_cerr << "vpgl_fm_compute_ransac: Need correspondence lists of same size.\n";
    return false;
  }

  // The following block is hacked from similar code in rrel_homography2d_est.
  rrel_fm_problem* estimator = new rrel_fm_problem( pr, pl );
  estimator->verbose = false;
  rrel_muset_obj* ransac = new rrel_muset_obj((int)vcl_floor(pr.size()*.75));
  estimator->set_prior_scale( 1.0 );
  rrel_ran_sam_search* ransam = new rrel_ran_sam_search;
  ransam->set_trace_level(trace_level_);

  if(!gen_all_)
    ransam->set_sampling_params( max_outlier_frac_,
                                 desired_prob_good_, max_pops_);
  else
    ransam->set_gen_all_samples();

  bool ransac_succeeded = ransam->estimate( estimator, ransac );
  if ( ransac_succeeded )
  estimator->params_to_fm( ransam->params(), fm );

  // Get a list of the outliers.
  estimator->compute_residuals( ransam->params(), residuals );

  outliers = vcl_vector<bool>();
  for ( unsigned i = 0; i < pr.size(); i++ ){
    if ( residuals[i] > outlier_thresh_ )
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
rrel_fm_problem::rrel_fm_problem(
  const vcl_vector< vgl_point_2d<double> >& pr,
  const vcl_vector< vgl_point_2d<double> >& pl ) :
  rrel_estimation_problem(7,8)
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
rrel_fm_problem::fit_from_minimal_set(
  const vcl_vector<int>& point_indices,
  vnl_vector<double>& params ) const
{
  if ( verbose ) vcl_cerr << "rrel_fm_problem::fit_from_minimal_set\n";
  assert( point_indices.size() == 8 );

  vcl_vector< vgl_homg_point_2d<double> > set_pr, set_pl;
  for ( int i = 0; i < 8; i++ ){
    int index = point_indices[i];
    set_pr.push_back( vgl_homg_point_2d<double>( pr_[index] ) );
    set_pl.push_back( vgl_homg_point_2d<double>( pl_[index] ) );
  }

  vpgl_fundamental_matrix<double> fm;
  vpgl_fm_compute_8_point fmc8(true);
  if ( !fmc8.compute( set_pr, set_pl, fm ) )
    return false;

  fm_to_params( fm, params );
  if ( verbose ) vcl_cerr << "params: " << params << '\n';
  return true;
}


//------------------------------------------
void
rrel_fm_problem::compute_residuals(
  const vnl_vector<double>& params,
  vcl_vector<double>& residuals ) const
{
  if ( verbose ) vcl_cerr << "rrel_fm_problem::compute_residuals\n";

  vpgl_fundamental_matrix<double> fm;
  params_to_fm(params, fm);

  if ( residuals.size() != pr_.size() )
    residuals.resize( pr_.size() );

  // The residual for each correspondence is the sum of the squared distances from
  // the points to their epipolar lines.
  for ( unsigned i = 0; i < pr_.size(); i++ )
  {
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
rrel_fm_problem::fm_to_params(
  const vpgl_fundamental_matrix<double>& fm,
  vnl_vector<double>& p ) const
{
  p.set_size(9);
  vnl_matrix_fixed<double,3,3> fm_vnl = fm.get_matrix();
  for ( int r = 0; r < 3; r++ )
    for ( int c = 0; c < 3; c++ )
      p( 3*r + c ) = fm_vnl( r, c );
}


//-------------------------------------------
void
rrel_fm_problem::params_to_fm(
  const vnl_vector<double>& p,
  vpgl_fundamental_matrix<double>& fm ) const
{
  vnl_matrix_fixed<double,3,3> fm_vnl;
  for ( int r = 0; r < 3; r++ )
    for ( int c = 0; c < 3; c++ )
      fm_vnl( r, c ) = p( 3*r + c );
  fm.set_matrix( fm_vnl );
}


//--------------------------------------------
bool
rrel_fm_problem::weighted_least_squares_fit(
  vnl_vector<double>& /*params*/,
  vnl_matrix<double>& /*norm_covar*/,
  const vcl_vector<double>* /*weights*/ ) const
{
  vcl_cerr << "rrel_fm_problem::weighted_least_squares_fit was called, but is not implemented.\n";
  return false;
}


#endif // vpgl_fm_compute_ransac_cxx_
